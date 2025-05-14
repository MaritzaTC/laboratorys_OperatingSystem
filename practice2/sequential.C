// sequential.c
// Universidad de Antioquia - Sistemas Operativos
// Miembros del equipo: Juan David Vasquez Ospina -  Maritza Tabarez Cárdenas

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void readMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows * cols; i++) {
        if (fscanf(fp, "%d", &matrix[i]) != 1) {
            fprintf(stderr, "Error: file '%s' contains less data than expected ( %d items expect).\n", filename, rows * cols);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
}

void writeMatrix(const char *filename, int *matrix, int rows, int cols) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error the output file could not be opened'%s'\n", filename);
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

int main() {
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

    if (A == NULL || B == NULL || C == NULL) {
        fprintf(stderr, "Error could not allocate memory .\n");
        free(A);
        free(B);
        free(C);
        exit(EXIT_FAILURE);
    }

    readMatrix("A.txt", A, N, M);
    readMatrix("B.txt", B, M, P);

    clock_t start = clock();

    sequentialMatrixMultiplication(A, B, C, N, M, P);

    clock_t end = clock();
    double sequential_time = (double)(end - start) / CLOCKS_PER_SEC;

    writeMatrix("C.txt", C, N, P);

    printf(" Sequential time:  %.4f  seconds\n", sequential_time);
    // Free dynamically allocated memory
    free(A);
    free(B);
    free(C);

    return 0;
}
