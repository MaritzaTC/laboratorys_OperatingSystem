#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 256

// Función para ejecutar un comando y capturar su salida
double execute_and_capture_time(const char *command, const char *search_for) {
    FILE *fp;
    char buffer[MAX_BUFFER];
    double time = 0.0;

    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }

    // Buscar la línea que contiene el tiempo y extraerlo
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, search_for)) {
            // Extraer el tiempo de la salida
            if (sscanf(buffer, "%*s %lf", &time) == 1) {
                break;
            }
        }
    }
    fclose(fp);

    return time;
}

int main() {
    // Comandos para ejecutar los programas secuenciales y paralelos
    const char *sequential_command = "./sequential";
    const char *parallel_command = "./parallel_matrix_multiply";

    // Ejecutar los programas y capturar los tiempos
    double sequential_time = execute_and_capture_time(sequential_command, "Sequential time");
    double parallel_time = execute_and_capture_time(parallel_command, "Parallel time");

    // Calcular el speedup
    double speedup = sequential_time / parallel_time;

    printf("Speedup: %.2fx\n", speedup);

    return 0;
}
