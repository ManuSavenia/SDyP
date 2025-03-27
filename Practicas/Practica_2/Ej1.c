#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

// Variables globales
int N;
int T;
double *A, *B, *C;

// Prototipos de funciones
double getValor(double *matriz, int fila, int columna, int orden);
void setValor(double *matriz, int fila, int columna, int orden, double valor);
double dwalltime();
void *funcion(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s T N\n", argv[0]);
        exit(1);
    }

    T = atoi(argv[1]);
    N = atoi(argv[2]);

    if (T <= 0 || N <= 0 || N % T != 0)
    {
        printf("Error: N debe ser múltiplo de T y ambos mayores que 0.\n");
        exit(1);
    }

    pthread_t misThreads[T];
    int threads_ids[T];
    int i, j;
    int check = 1;
    double timetick;

    // Reserva de memoria para matrices
    A = (double *)malloc(sizeof(double) * N * N);
    B = (double *)malloc(sizeof(double) * N * N);
    C = (double *)malloc(sizeof(double) * N * N);

    // Inicialización de A y B
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            setValor(A, i, j, ORDENXFILAS, 1);    // A almacenada por filas
            setValor(B, i, j, ORDENXCOLUMNAS, 1); // B almacenada por columnas
        }
    }

    // Multiplicación de matrices con hilos
    timetick = dwalltime();

    for (int id = 0; id < T; id++)
    {
        threads_ids[id] = id;
        pthread_create(&misThreads[id], NULL, &funcion, (void *)&threads_ids[id]);
    }

    for (int id = 0; id < T; id++)
    {
        pthread_join(misThreads[id], NULL);
    }

    printf("Tiempo en segundos: %f\n", dwalltime() - timetick);

    // Verificación del resultado
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            check = check && (getValor(C, i, j, ORDENXFILAS) == N);
        }
    }

    if (check)
    {
        printf("Multiplicación de matrices resultado correcto\n");
    }
    else
    {
        printf("Multiplicación de matrices resultado erróneo\n");
    }

    // Liberación de memoria
    free(A);
    free(B);
    free(C);
    return 0;
}

// Función ejecutada por los hilos
void *funcion(void *arg)
{
    int tid = *(int *)arg;
    int inicio = (tid * N) / T;
    int fin = ((tid + 1) * N) / T;

    for (int i = inicio; i < fin; i++)
    {
        for (int j = 0; j < N; j++)
        {
            C[i * N + j] = 0;
            for (int k = 0; k < N; k++)
            {
                C[i * N + j] += A[i * N + k] * B[k + j * N]; // Acceso directo a memoria
            }
        }
    }
    pthread_exit(NULL);
}

// Retorna el valor de la matriz según el orden de almacenamiento
double getValor(double *matriz, int fila, int columna, int orden)
{
    if (orden == ORDENXFILAS)
    {
        return matriz[fila * N + columna];
    }
    else
    {
        return matriz[fila + columna * N];
    }
}

// Establece un valor en la matriz según el orden de almacenamiento
void setValor(double *matriz, int fila, int columna, int orden, double valor)
{
    if (orden == ORDENXFILAS)
    {
        matriz[fila * N + columna] = valor;
    }
    else
    {
        matriz[fila + columna * N] = valor;
    }
}

// Función para calcular el tiempo
double dwalltime()
{
    double sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}
