// parallel.c
// Universidad de Antioquia - Sistemas Operativos
// Miembros del equipo: Juan David Vasquez Ospina -  Maritza Tabarez Cárdenas

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>


#define NUM_PROCESOS 4

void readMatrix(const char *filename, int *matrix, int rows, int cols)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows * cols; i++)
    {
       if (fscanf(fp, "%d", &matrix[i]) != 1) {
            fprintf(stderr, "Error: file '%s' contains less data than expected ( %d items expect).\n", filename, rows * cols);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
}

void writeMatrix(const char *filename, int *matrix, int rows, int cols)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error the output file could not be opened'%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fprintf(fp, "%d ", matrix[i * cols + j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main()
{
    int N, M, P;

    // Read matrix dimensions (this can be modified based on your needs)
    printf("Enter dimensions of matrices A (N x M) and B (M x P)\n");
    printf("Enter N: ");
    scanf("%d", &N);
    printf("Enter M: ");
    scanf("%d", &M);
    printf("Enter P: ");
    scanf("%d", &P);

     // Dynamically allocate memory for matrices A, B, and C
    int *A = (int *)malloc(N * M * sizeof(int));
    int *B = (int *)malloc(M * P * sizeof(int));
    int *C = (int *)malloc(N * P * sizeof(int));

      if (!A || !B || !C) {
        perror("Memory allocation failed");
        exit(1);
    }
    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

       // Create shared memory segment for matrix C
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * N * P, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    int *sharedC = (int *)shmat(shmid, NULL, 0);
    if ((intptr_t)sharedC == -1)
    {
        perror("shmat");
        exit(1);
    }

    clock_t start = clock();

    int rows_per_proc = N / NUM_PROCESOS;

    for (int p = 0; p < NUM_PROCESOS; p++)
    {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            // Clean up before exiting
            shmdt(sharedC);
            shmctl(shmid, IPC_RMID, NULL);
            free(A);
            free(B);
            free(C);
            exit(1);
        }

        if (pid == 0)  // Child process
        {
            int start_row = p * rows_per_proc;
            int end_row = (p == NUM_PROCESOS - 1) ? N : start_row + rows_per_proc;

            for (int i = start_row; i < end_row; i++)
            {
                for (int j = 0; j < P; j++)
                {
                    sharedC[i * P + j] = 0;
                    for (int k = 0; k < M; k++)
                    {
                        sharedC[i * P + j] += A[i * M + k] * B[k * P + j];
                    }
                }
            }

            shmdt(sharedC);  // Detach shared memory before exiting
            exit(0);  // Exit child process gracefully
        }
    }

    // Parent process waits for all children
    int status;
    for (int p = 0; p < NUM_PROCESOS; p++)
    {
        pid_t child_pid = wait(&status);
        if (child_pid == -1) {
            perror("wait failed");
            // Handle error and clean up
            shmdt(sharedC);
            shmctl(shmid, IPC_RMID, NULL);
            free(A);
            free(B);
            free(C);
            exit(1);
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child process %d failed with exit status %d\n", child_pid, WEXITSTATUS(status));
            shmdt(sharedC);
            shmctl(shmid, IPC_RMID, NULL);
            free(A);
            free(B);
            free(C);
            exit(1);
        }
    }

    clock_t end = clock();
    double parallel_time = (double)(end - start) / CLOCKS_PER_SEC;

    writeMatrix("C.txt", sharedC, N, P);
    printf("Parallel time: %.4f seconds\n", parallel_time);

    shmdt(sharedC);  // Detach the shared memory
    shmctl(shmid, IPC_RMID, NULL);  // Clean up the shared memory

    // Free dynamically allocated memory
    free(A);
    free(B);
    free(C);

    return 0;
}
