# 📊 Multiplicación de Matrices con Procesos Paralelos en C y Go

**Universidad de Antioquia**  
*Curso: Sistemas Operativos*  
*Práctica #2 - Multiplicación de matrices con procesos*

## 👥 Integrantes

* [x] [Maritza Tabarez Cárdenas](https://github.com/MaritzaTC)
* [x] [Juan David Vasquez Ospina](https://github.com/JuanVasquezO)

## 🎯 Objetivos

<details>
<summary>Ver objetivos del proyecto</summary>
<br><br>

- Comprender la creación de procesos usando `fork()`.
- Implementar cómputo paralelo dividiendo la carga de trabajo entre múltiples procesos (en C) o gorutinas (en Go) usando APIs del sistema operativo.
- Aprender y aplicar mecanismos de comunicación entre procesos (IPC), como pipes o memoria compartida (en C), y canales o estructuras para compartir memoria (en Go).
- Analizar y comparar el rendimiento entre las implementaciones secuenciales y paralelas de la multiplicación de matrices.

</details>

---

## 📝 Descripción del problema

<details>
<summary>Ver descripción completa</summary>
<br><br>

Desarrollar un programa que multiplique dos matrices grandes:

* Matriz A (N×M)
* Matriz B (M×P)
* Resultado: Matriz C (N×P)

</details>

# 🖥️ Implementación en C

## 🔍 Versiones implementadas

<details>
<summary>Ver versiones</summary>
<br><br>

* Versión secuencial (`sequential.c`)
* Versión paralela con procesos (`parallel.c`) usando memoria compartida
* Version secuencial y paralela (`sequientialAndParallel.c`)

</details>

## ⚙️ Compilación y ejecución

<details>
<summary>Instrucciones de compilación</summary>
<br><br>

Usar `gcc` para compilar:

```bash
gcc sequential.c -o sequential
gcc parallel.c -o parallel
gcc sequientialAndParallel.c -o parallel_matrix_multiply
```

</details>

<details>
<summary>Instrucciones de ejecución</summary>
<br><br>

```bash
./sequential
./parallel
```

Al ejecutar, el programa pedirá las dimensiones de las matrices (N, M, P) y leerá los datos desde los archivos A.txt y B.txt. El resultado se escribirá en C.txt.

</details>

## 📁 Entrada y Salida

<details>
<summary>Ver detalles</summary>
<br><br>

* **Entrada**: Archivos A.txt, A\_small.txt, A\_big y B.txt, B\_small.txt, B\_big con los valores de las matrices.
* **Salida**: Archivo C.txt con la matriz resultado.

</details>

## 🔄 Mecanismo IPC en C

<details>
<summary>Ver justificación técnica</summary>
<br><br>

Para la implementación paralela, se utilizó memoria compartida (Shared Memory) mediante las funciones `shmget()` y `shmat()` de la API de System V.

Esta elección se basa en:

* **Velocidad**: La memoria compartida permite el acceso directo a la región común de memoria, lo que evita copias innecesarias de datos.
* **Simplicidad en agregación**: Los procesos hijos escriben directamente en la matriz C, eliminando la necesidad de combinar resultados tras el cálculo.
* **Escalabilidad**: Es más eficiente que otros mecanismos como pipes para grandes volúmenes de datos.

</details>

## 📊 Análisis de Rendimiento en C

<details>
<summary>Ver capturas de pantalla de resultados</summary>
<br><br>

![alt text](<Screenshot 2025-05-14 183333.png>)
![alt text](<Screenshot 2025-05-14 190748.png>)
![alt text](<Screenshot 2025-05-14 195626.png>)


</details>

<details>
<summary>Ver tabla de tiempos y speedup</summary>
<br><br>

| N   | M   | P   | Nº Procesos | Tiempo Secuencial (s) | Tiempo Paralelo (s) | Speedup |
| --- | --- | --- | ----------- | --------------------- | ------------------- | ------- |
| 12  | 15  | 10  | 1           | 0.0001                | 0.0019              | 0.05    |
| 12  | 15  | 10  | 2           | 0.0001                | 0.0027              | 0.03    |
| 12  | 15  | 10  | 3           | 0.0001                | 0.0037              | 0.02    |
| 12  | 15  | 10  | 4           | 0.0001                | 0.0045              | 0.02    |
| 2   | 3   | 2   | 1           | 0.0001                | 0.0015              | 0.05    |
| 2   | 3   | 2   | 2           | 0.0001                | 0.0028              | 0.02    |
| 2   | 3   | 2   | 3           | 0.0001                | 0.0036              | 0.02    |
| 2   | 3   | 2   | 4           | 0.0001                | 0.0044              | 0.02    |
| 400 | 360 | 400 | 1           | 1.0425                | 0.0386              | 26.99   |
| 400 | 360 | 400 | 2           | 0.8743                | 0.0212              | 41.26   |
| 400 | 360 | 400 | 3           | 0.9131                | 0.0347              | 26.34   |
| 400 | 360 | 400 | 4           | 0.9994                | 0.0491              | 20.33   |

</details>

<details>
<summary>Ver gráfico de Speedup vs Número de Procesos</summary>
<br><br>

![alt text](Figure_1.png)


</details>

### 📈 Interpretación de resultados en C

<details>
<summary>Para matrices pequeñas (2x3 x 3x2 y 12x15 x 15x10)</summary>
<br><br>

* El speedup es muy bajo, incluso menor que 1 en algunos casos, lo que indica que la versión paralela no es eficiente para estos tamaños pequeños.
* De hecho, el tiempo paralelo es incluso mayor que el secuencial, reflejado en speedups menores a 1 (0.05, 0.02, etc.).
* Esto se debe a que la sobrecarga de crear y gestionar procesos, sincronizar y dividir el trabajo supera cualquier beneficio que se obtenga al paralelizar la multiplicación de matrices tan pequeñas.
* Además, aumentar el número de procesos no mejora significativamente el rendimiento; en algunos casos, lo empeora.

</details>

<details>
<summary>Para matrices grandes (400x360 x 360x400)</summary>
<br><br>

* El speedup es mucho mayor, llegando a valores superiores a 20 o 40 en algunos casos, lo que indica que la paralelización sí da un gran beneficio.
* Esto es esperado porque con matrices grandes la carga de trabajo es mayor, por lo que dividir la tarea entre procesos reduce mucho el tiempo total.
* Sin embargo, el speedup no crece de forma perfectamente lineal con el número de procesos: hay un máximo (41.26 con 2 procesos) y luego baja.
* Esta caída puede ser causada por la sobrecarga de comunicación y sincronización entre procesos cuando se usan más de dos procesos, o limitaciones de hardware (como número de núcleos físicos disponibles).
* Aún así, la paralelización es claramente beneficiosa para cargas de trabajo grandes.

</details>

---

# 🚀 Implementación en Go

## 📋 Estructura del Programa

<details>
<summary>Ver estructura completa</summary>
<br><br>

#### Funciones para manejo de archivos:

* `readMatrixFromFile`: Lee los datos de la matriz desde un archivo de texto.
* `writeMatrixToFile`: Escribe la matriz resultante en un archivo de texto.

#### Multiplicación de matrices:

* `workerMultiply`: Función que realiza la multiplicación parcial de matrices en una goroutine y envía los resultados a través de un pipe.
* `multiplyMatricesParallelWithPipes`: Función que crea las goroutines (workers), los pipes, distribuye el trabajo entre los workers y recoge los resultados de los pipes para construir la matriz final.
* `multiplyMatricesSequential`: Función que realiza la multiplicación de matrices de forma secuencial.

#### Función main:

* Procesa los argumentos de la línea de comandos: nombres de los archivos de entrada para las matrices A y B, y el número de workers (goroutines).
* Lee las matrices A y B desde los archivos.
* Realiza la multiplicación de matrices de forma secuencial y paralela.
* Imprime los tiempos de ejecución de ambas versiones y el speedup obtenido con la versión paralela.
* Escribe la matriz resultado en un archivo.

</details>


## ⚙️ ejecución

<details>
<summary>Instrucciones de ejecución</summary>
<br><br>


```bash
go run matrix_mul.go A.txt B.txt 2
```

A.txt y B.txt son los nombres de las matrices por multiplicar, y el numero siguiente a este es el numero de procesos por el cual hacer la comparación

</details>

## 📊 Análisis de Rendimiento en Go

<details>
<summary>Ver tabla de tiempos</summary>
<br><br>

| N   | M   | P   | Nº Procesos | Tiempo total (s) |
| --- | --- | --- | ----------- | ---------------- |
| 400 | 360 | 400 | 1           | 0.12201          |
| 400 | 360 | 400 | 2           | 0.218811         |
| 400 | 360 | 400 | 3           | 0.163926         |
| 400 | 360 | 400 | 4           | 0.143176         |
| 400 | 360 | 400 | 5           | 0.146358         |
| 400 | 360 | 400 | 6           | 0.131248         |
| 400 | 360 | 400 | 7           | 0.118516         |
| 400 | 360 | 400 | 8           | 0.124494         |
| 400 | 360 | 400 | 9           | 0.118718         |

</details>

<details>
<summary>Ver gráfico de Tiempo vs Número de Procesos</summary>
<br><br>

![Tiempo vs Procesos](https://github.com/user-attachments/assets/4d54ea9b-580d-43d4-9d1d-c49a4d7d5ce0)

</details>


## 🔄 Mecanismo IPC en Go

<details>
<summary>Ver detalles técnicos</summary>
<br><br>

La implementación paralela en Go utiliza pipes para la comunicación entre las goroutines (workers). Cada worker calcula una porción de la matriz resultado y envía los resultados (índices de la celda y valor calculado) a través de un pipe. La función principal lee los resultados de los pipes y construye la matriz resultado final.

#### Paralelismo con Goroutines

Go utiliza goroutines para lograr la ejecución paralela. La carga de trabajo se divide entre las goroutines, permitiendo que múltiples porciones de la matriz se calculen simultáneamente.

</details>

---



# 🔄 Comparación entre implementaciones

<details>
<summary>C vs Go</summary>
<br><br>

Aunque ambas implementaciones buscan paralelizar la multiplicación de matrices, utilizan diferentes enfoques y mecanismos de IPC:

#### C:

* Utiliza procesos separados y memoria compartida para la comunicación.
* Los procesos hijos escriben los resultados directamente en una región de memoria compartida.

#### Go:

* Utiliza goroutines (que son más ligeras que los procesos) y pipes para la comunicación.
* Los workers envían los resultados a través de pipes, y la goroutine principal los recoge.

Ambos enfoques tienen como objetivo mejorar el rendimiento al dividir la carga de trabajo, pero difieren en la forma en que se gestionan los procesos/goroutines y cómo se comunican.

</details>

# 📚 Referencias

<details>
<summary>Ver referencias</summary>
<br><br>

* OSTEP: Processes API Chapter.
* Linux man pages: `fork()`, `shmget()`, `pipe()`

</details>
