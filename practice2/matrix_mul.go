package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

// Función para leer una matriz desde un archivo
func readMatrixFromFile(filename string) ([][]float64, int, int, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, 0, 0, fmt.Errorf("error al abrir el archivo %s: %v", filename, err)
	}
	defer file.Close()

	var matrix [][]float64
	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		rowData := strings.Fields(scanner.Text())
		row := make([]float64, len(rowData))
		for i, value := range rowData {
			row[i], err = strconv.ParseFloat(value, 64)
			if err != nil {
				return nil, 0, 0, fmt.Errorf("error al convertir el valor: %v", err)
			}
		}
		matrix = append(matrix, row)
	}

	if err := scanner.Err(); err != nil {
		return nil, 0, 0, fmt.Errorf("error al leer el archivo: %v", err)
	}

	if len(matrix) == 0 {
		return nil, 0, 0, fmt.Errorf("la matriz está vacía")
	}

	rows := len(matrix)
	cols := len(matrix[0])
	return matrix, rows, cols, nil
}

// Función para escribir una matriz en un archivo
func writeMatrixToFile(filename string, matrix [][]float64) error {
	file, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("error al crear el archivo %s: %v", filename, err)
	}
	defer file.Close()

	rows := len(matrix)
	cols := len(matrix[0])

	// Escribir dimensiones como primera línea
	fmt.Fprintf(file, "%d %d\n", rows, cols)

	// Escribir datos de la matriz
	for _, row := range matrix {
		for _, val := range row {
			fmt.Fprintf(file, "%f ", val)
		}
		fmt.Fprintln(file)
	}

	return nil
}

// Función que realiza la multiplicación parcial de matrices y envía resultados por la pipe
// Función que realiza la multiplicación en cada worker
func workerMultiply(A, B [][]float64, workerID, K int, pw *os.File, M, N, P int) {
	defer pw.Close()

	startRow := (workerID * N) / K
	endRow := ((workerID + 1) * N) / K

	for i := startRow; i < endRow; i++ {
		for j := 0; j < P; j++ {
			sum := 0.0
			for k := 0; k < M; k++ {
				sum += A[i][k] * B[k][j]
			}
			// Escribir el resultado al pipe
			fmt.Fprintf(pw, "%d %d %f\n", i, j, sum)
		}
	}
}

// Función para realizar la multiplicación con pipes
func multiplyMatricesParallelWithPipes(A, B [][]float64, K int) [][]float64 {
	N := len(A)    // Número de filas de la matriz A
	P := len(B[0]) // Número de columnas de la matriz B
	M := len(A[0]) // Número de columnas de la matriz A

	// Crear matriz resultado
	C := make([][]float64, N)
	for i := range C {
		C[i] = make([]float64, P)
	}

	// Crear pipes para cada worker
	var pipes []*os.File
	var workers []*sync.WaitGroup
	for i := 0; i < K; i++ {
		pr, pw, err := os.Pipe()
		if err != nil {
			fmt.Println("Error al crear pipe:", err)
			return nil
		}
		pipes = append(pipes, pr)
		wg := &sync.WaitGroup{}
		wg.Add(1)
		workers = append(workers, wg)
		go workerMultiply(A, B, i, K, pw, M, N, P)
	}

	// Leer los resultados de las pipes
	for i := 0; i < K; i++ {
		go func(i int) {
			defer workers[i].Done()
			scanner := bufio.NewScanner(pipes[i])
			for scanner.Scan() {
				line := scanner.Text()
				// Parsear y almacenar el resultado en la matriz C
				var row, col int
				var value float64
				_, err := fmt.Sscanf(line, "%d %d %f", &row, &col, &value)
				if err != nil {
					fmt.Println("Error al leer desde la pipe:", err)
					return
				}
				C[row][col] = value
			}
			if err := scanner.Err(); err != nil {
				fmt.Println("Error al leer desde pipe:", err)
			}
		}(i)
	}

	// Esperar a que todos los workers terminen
	for _, wg := range workers {
		wg.Wait()
	}

	return C
}

// Versión secuencial para comparación
func multiplyMatricesSequential(A, B [][]float64) [][]float64 {
	N, M := len(A), len(A[0])
	P := len(B[0])

	C := make([][]float64, N)
	for i := range C {
		C[i] = make([]float64, P)
	}

	for i := 0; i < N; i++ {
		for j := 0; j < P; j++ {
			for k := 0; k < M; k++ {
				C[i][j] += A[i][k] * B[k][j]
			}
		}
	}

	return C
}

func main() {
	if len(os.Args) != 4 {
		fmt.Println("Uso: go run matrix_multiply.go <A_file> <B_file> <num_workers>")
		return
	}

	fileA := os.Args[1]
	fileB := os.Args[2]
	K, err := strconv.Atoi(os.Args[3])
	if err != nil {
		fmt.Printf("Error al convertir el número de workers: %v\n", err)
		return
	}

	if K <= 0 {
		fmt.Println("El número de workers debe ser mayor que cero")
		return
	}

	A, N, M, err := readMatrixFromFile(fileA)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Matriz A: %d x %d\n", N, M)

	B, MCheck, P, err := readMatrixFromFile(fileB)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Matriz B: %d x %d\n", MCheck, P)

	// Verificar si las matrices son multiplicables
	if M != MCheck {
		fmt.Printf("Error: Las dimensiones de las matrices no son compatibles para la multiplicación (%d != %d)\n", M, MCheck)
		return
	}

	// Ajustar K si es mayor que el número de filas
	if K > N {
		fmt.Printf("Ajustando número de workers de %d a %d (número de filas)\n", K, N)
		K = N
	}

	// Multiplicación secuencial para comparación
	fmt.Println("Iniciando multiplicación secuencial...")
	startSeq := time.Now()
	_ = multiplyMatricesSequential(A, B) // Usamos _ para ignorar el resultado si no lo necesitamos
	endSeq := time.Now()
	seqTime := endSeq.Sub(startSeq).Seconds()
	fmt.Printf("Tiempo de multiplicación secuencial: %.6f segundos\n", seqTime)

	// Multiplicación paralela
	fmt.Printf("Iniciando multiplicación paralela con %d workers...\n", K)
	startPar := time.Now()
	CPar := multiplyMatricesParallelWithPipes(A, B, K)
	endPar := time.Now()
	parTime := endPar.Sub(startPar).Seconds()
	fmt.Printf("Tiempo de multiplicación paralela (%d workers): %.6f segundos\n", K, parTime)

	// Calcular speedup
	speedup := seqTime / parTime
	fmt.Printf("Speedup: %.2fx\n", speedup)

	// Escribir el resultado en un archivo
	err = writeMatrixToFile("C.txt", CPar)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Resultado escrito en C.txt\n")
}
