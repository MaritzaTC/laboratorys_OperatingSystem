// parallel.c And Sequential.c
// Universidad de Antioquia - Sistemas Operativos
// Miembros del equipo: Juan David Vasquez Ospina -  Maritza Tabarez Cárdenas

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define NUM_PROCESSES 3

void readMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows * cols; i++) {
        if (fscanf(fp, "%d", &matrix[i]) != 1) {
            fprintf(stderr, "Error: file '%s' has less data than expected (%d items expected).\n", filename, rows * cols);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
}

void writeMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not open output file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(fp, "%d ", matrix[i * cols + j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void sequentialMatrixMultiplication(int *A, int *B, int *C, int N, int M, int P) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            C[i * P + j] = 0;
            for (int k = 0; k < M; k++) {
                C[i * P + j] += A[i * M + k] * B[k * P + j];
            }
        }
    }
}

void parallelMatrixMultiplication(int *A, int *B, int *C, int N, int M, int P) {
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * N * P, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *sharedC = (int *)shmat(shmid, NULL, 0);
    if ((intptr_t)sharedC == -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    int rows_per_proc = N / NUM_PROCESSES;

    for (int p = 0; p < NUM_PROCESSES; p++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            int start_row = p * rows_per_proc;
            int end_row = (p == NUM_PROCESSES - 1) ? N : start_row + rows_per_proc;

            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < P; j++) {
                    sharedC[i * P + j] = 0;
                    for (int k = 0; k < M; k++) {
                        sharedC[i * P + j] += A[i * M + k] * B[k * P + j];
                    }
                }
            }

            shmdt(sharedC);
            exit(0);
        }
    }

    for (int p = 0; p < NUM_PROCESSES; p++) {
        int status;
        wait(&status);
    }

    for (int i = 0; i < N * P; i++) {
        C[i] = sharedC[i];
    }

    shmdt(sharedC);
    shmctl(shmid, IPC_RMID, NULL);
}

int main() {
    int N, M, P;

    printf("Enter dimensions of matrices A (N x M) and B (M x P)\n");
    printf("Enter N: "); scanf("%d", &N);
    printf("Enter M: "); scanf("%d", &M);
    printf("Enter P: "); scanf("%d", &P);

    int *A = malloc(N * M * sizeof(int));
    int *B = malloc(M * P * sizeof(int));
    int *C_seq = malloc(N * P * sizeof(int));
    int *C_par = malloc(N * P * sizeof(int));

    if (!A || !B || !C_seq || !C_par) {
        fprintf(stderr, "Error allocating memory.\n");
        exit(EXIT_FAILURE);
    }

    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

    // Secuencial
    clock_t start_seq = clock();
    sequentialMatrixMultiplication(A, B, C_seq, N, M, P);
    clock_t end_seq = clock();
    double time_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;

    // Paralelo
    clock_t start_par = clock();
    parallelMatrixMultiplication(A, B, C_par, N, M, P);
    clock_t end_par = clock();
    double time_par = (double)(end_par - start_par) / CLOCKS_PER_SEC;

    // Guardar resultado paralelo (ambos deberían ser iguales)
    writeMatrix("C.txt", C_par, N, P);

    // Comparar resultados
    int correct = 1;
    for (int i = 0; i < N * P; i++) {
        if (C_seq[i] != C_par[i]) {
            correct = 0;
            printf("Mismatch at index %d: seq = %d, par = %d\n", i, C_seq[i], C_par[i]);
            break;
        }
    }

    printf("Sequential time: %.4f seconds\n", time_seq);
    printf("Parallel time (%d processes): %.4f seconds\n", NUM_PROCESSES, time_par);
    if (correct)
        printf("Speedup: %.2f\n", time_seq / time_par);
    else
        printf("Mismatch between sequential and parallel results.\n");

    free(A);
    free(B);
    free(C_seq);
    free(C_par);

    return 0;
}


