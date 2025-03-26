#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

int N = 100;  // Dimensión por defecto de las matrices

// Prototipos de funciones
double getValor(double *matriz, int fila, int columna, int orden);
void setValor(double *matriz, int fila, int columna, int orden, double valor);
double dwalltime();

void* funcion(void *arg) {
    int tid = *(int*)arg;
    printf("Hilo id:%d\n", tid);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int T = atoi(argv[1]);
    pthread_t misThreads[T];
    int threads_ids[T];
    double *A, *B, *C;
    int i, j, k;
    int check = 1;
    double timetick;
    

    for (int id = 0; id < T; id++) {
        threads_ids[id] = id;
        pthread_create(&misThreads[id], NULL, &funcion, (void*)&threads_ids[id]);
    }

    for (int id = 0; id < T; id++) {
        pthread_join(misThreads[id], NULL);
    }

    // Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0)) {
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    // Aloca memoria para las matrices
    A = (double*)malloc(sizeof(double) * N * N);
    B = (double*)malloc(sizeof(double) * N * N);
    C = (double*)malloc(sizeof(double) * N * N);

    // Inicializa las matrices A y B en 1, el resultado será una matriz con todos sus valores en N
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            setValor(A, i, j, ORDENXFILAS, 1);
            setValor(B, i, j, ORDENXFILAS, 1);
        }
    }

    // Multiplicación de matrices sin funciones auxiliares
    timetick = dwalltime();
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            C[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[k + j * N];
            }
        }
    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick);

    // Verifica el resultado
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            check = check && (getValor(C, i, j, ORDENXFILAS) == N);
        }
    }

    if (check) {
        printf("Multiplicacion de matrices resultado correcto\n");
    } else {
        printf("Multiplicacion de matrices resultado erroneo\n");
    }

    free(A);
    free(B);
    free(C);
    return 0;
}

// Retorna el valor de la matriz en la posición fila y columna según el orden que esté ordenada
double getValor(double *matriz, int fila, int columna, int orden) {
    if (orden == ORDENXFILAS) {
        return matriz[fila * N + columna];
    } else {
        return matriz[fila + columna * N];
    }
}

// Establece el valor de la matriz en la posición fila y columna según el orden que esté ordenada
void setValor(double *matriz, int fila, int columna, int orden, double valor) {
    if (orden == ORDENXFILAS) {
        matriz[fila * N + columna] = valor;
    } else {
        matriz[fila + columna * N] = valor;
    }
}

// Para calcular tiempo
double dwalltime() {
    double sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}
