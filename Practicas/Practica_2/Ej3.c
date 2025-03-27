#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
int N;
int T;
int *vector;
int min;
int max;

pthread_mutex_t mutex_ocurrencias = PTHREAD_MUTEX_INITIALIZER;

// Prototipos de funciones
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
    if (T <= 0 || N <= 0)
    {
        printf("Error: T y N deben ser mayores que 0.\n");
        exit(1);
    }

    pthread_t misThreads[T];
    int threads_ids[T];
    int i;
    double timetick;

    // Reservar memoria para el vector
    vector = (int *)malloc(sizeof(int) * N);
    if (vector == NULL)
    {
        printf("Error al asignar memoria para el vector.\n");
        exit(1);
    }

    // Inicializar el generador de números aleatorios
    srand(time(NULL));

    // Llenar el vector con valores aleatorios entre 0 y 9
    for (i = 0; i < N; i++)
    {
        vector[i] = rand() % 10000;
    }

    // Iniciar la cuenta de tiempo
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

    printf("\nTiempo en segundos: %f\n", dwalltime() - timetick);
    printf("Mínimo: %d\n", min);
    printf("Máximo: %d\n", max);
    // Liberar memoria
    free(vector);
    return 0;
}

// Función ejecutada por los hilos
void *funcion(void *arg)
{
    int tid = *(int *)arg;
    int local_min = 999999;
    int local_max = 0;
    // Calcular los límites del bloque de cada hilo
    int inicio = (tid * N) / T;
    int fin = ((tid + 1) * N) / T;

    int ocurrencias_loc = 0;

    for (int i = inicio; i < fin; i++)
    {
        if (vector[i] >= local_max)
        {
            local_max = vector[i];
        }
        else if (vector[i] <= local_min)
        {
            local_min = vector[i];
        }
    }

    // Sumar la cuenta local al contador global con mutex
    pthread_mutex_lock(&mutex_ocurrencias);

    if (local_min < min)
    {
        min = local_min;
    }
    if (local_max > max)
    {
        max = local_max;
    }

    pthread_mutex_unlock(&mutex_ocurrencias);

    pthread_exit(NULL);
}

// Función para calcular el tiempo
double dwalltime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
