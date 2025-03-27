#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
int N;
int T;
int *vector;
int X;
int ocurrencias = 0;

pthread_mutex_t mutex_ocurrencias = PTHREAD_MUTEX_INITIALIZER;

// Prototipos de funciones
double dwalltime();
void *funcion(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Uso: %s T N X\n", argv[0]);
        exit(1);
    }

    T = atoi(argv[1]);
    N = atoi(argv[2]);
    X = atoi(argv[3]);

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
        vector[i] = rand() % 10;
    }

    // Asegurar que X aparece al menos una vez en el vector
    vector[rand() % N] = X;

    // Imprimir el vector generado
    printf("Vector generado:\n");
    for (i = 0; i < N; i++)
    {
        printf("%d ", vector[i]);
    }
    printf("\n");

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
    printf("El número %d aparece %d veces en el vector.\n", X, ocurrencias);

    // Liberar memoria
    free(vector);
    return 0;
}

// Función ejecutada por los hilos
void *funcion(void *arg)
{
    int tid = *(int *)arg;

    // Calcular los límites del bloque de cada hilo
    int inicio = (tid * N) / T;
    int fin = ((tid + 1) * N) / T; // Fin exclusivo

    int ocurrencias_loc = 0;

    for (int i = inicio; i < fin; i++)
    {
        if (vector[i] == X)
        {
            ocurrencias_loc++;
        }
    }

    // Sumar la cuenta local al contador global con mutex
    pthread_mutex_lock(&mutex_ocurrencias);
    ocurrencias += ocurrencias_loc;
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
