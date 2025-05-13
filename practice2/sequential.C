// sequential.c
// Universidad de Antioquia - Sistemas Operativos
// Miembros del equipo: Juan David Vasquez Ospina -  Maritza Tabarez Cárdenas

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 2
#define M 3
#define P 2

void readMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("File no found");
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
    int C[N * P];

    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

    clock_t start = clock();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            C[i * P + j] = 0;
            for (int k = 0; k < M; k++) {
                C[i * P + j] += A[i * M + k] * B[k * P + j];
            }
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    writeMatrix("C.txt", C, N, P);

    printf("Sequencial time: %.4f seconds\n", elapsed);
    return 0;
}
