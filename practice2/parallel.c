// parallel.c
// Universidad de Antioquia - Sistemas Operativos
// Miembros del equipo: Juan David Vasquez Ospina -  Maritza Tabarez Cárdenas

#include <semaphore.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define N 2
#define M 3
#define P 2
#define NUM_PROCESOS 2

void readMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("File not found");
        exit(1);
    }
    for (int i = 0; i < rows * cols; i++) {
        fscanf(fp, "%d", &matrix[i]);
    }
    fclose(fp);
}

void writeMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%d ", matrix[i * cols + j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void parallelMatrixMultiplication(int num_processes, int *A, int *B, int *C) {
    // create shared memory for the result matrix
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * N * P, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
    int *C_shared = (int *)shmat(shmid, NULL, 0);

    //create shared memory for the mutex and current row
    sem_t *mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *current_row = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *current_row = 0;
    sem_init(mutex, 1, 1);  // 1 = compartido entre procesos

    clock_t start = clock();

    for (int p = 0; p < num_processes; p++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            // Multiplicación de la fila `row` de A con la matriz B
            while (1) {
                sem_wait(mutex);
                int row = (*current_row)++;
                sem_post(mutex);

                if (row >= N) break;

                // Multiplicación de la fila `row` de A con la matriz B
                for (int j = 0; j < P; j++) {
                    C_shared[row * P + j] = 0;
                    for (int k = 0; k < M; k++) {
                        C_shared[row * P + j] += A[row * M + k] * B[k * P + j];
                    }
                }
            }
            shmdt(C_shared);
            exit(0);
        }
    }

    // wait for all child processes to finish
    for (int p = 0; p < num_processes; p++) {
        wait(NULL);
    }

    clock_t end = clock();
    double parallel_time = (double)(end - start) / CLOCKS_PER_SEC;

    writeMatrix("C_parallel.txt", C_shared, N, P);
    printf("Parallel time with %d processes: %.4f seconds\n", num_processes, parallel_time);
    return parallel_time;
    // clean up
    shmdt(C_shared);
    shmctl(shmid, IPC_RMID, NULL);
    munmap(mutex, sizeof(sem_t));
    munmap(current_row, sizeof(int));
    
}

int main() {
    int A[N * M];
    int B[M * P];
    int C[N * P]; 

    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

    // run sequential multiplication
    clock_t start = clock();
    sequentialMatrixMultiplication(A, B, C);
    clock_t end = clock();
    double sequential_time = (double)(end - start) / CLOCKS_PER_SEC;
    writeMatrix("C_sequential.txt", C, N, P);
    printf("Sequential time: %.4f seconds\n", sequential_time);
    // run parallel multiplication with different number of processes
    int num_processes[] = {1, 2, 4, 8};  
     for (int i = 0; i < 4; i++) {
        double parallel_time = parallelMatrixMultiplication(num_processes[i], A, B, C);

        // calculate speedup
        double speedup = sequential_time / parallel_time;
        printf("Speedup with %d processes: %.2fx\n", num_processes[i], speedup);
    }

    
    return 0;
}
