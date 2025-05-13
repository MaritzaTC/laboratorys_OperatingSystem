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

int main() {
    int A[N * M];
    int B[M * P];

    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

    // Crear memoria compartida
    int shmid = shmget(IPC_PRIVATE, sizeof(int) * N * P, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    int *C = (int *)shmat(shmid, NULL, 0);

    clock_t start = clock();

    int rows_per_proc = N / NUM_PROCESOS;

    for (int p = 0; p < NUM_PROCESOS; p++) {
        pid_t pid = fork();
        if (pid == 0) {
            int start_row = p * rows_per_proc;
            int end_row = (p == NUM_PROCESOS - 1) ? N : start_row + rows_per_proc;

            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < P; j++) {
                    C[i * P + j] = 0;
                    for (int k = 0; k < M; k++) {
                        C[i * P + j] += A[i * M + k] * B[k * P + j];
                    }
                }
            }
            shmdt(C);
            exit(0);
        }
    }

    for (int p = 0; p < NUM_PROCESOS; p++) {
        wait(NULL);
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    writeMatrix("C.txt", C, N, P);
    printf("Parallel time: %.4f seconds\n", elapsed);

    shmdt(C);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
