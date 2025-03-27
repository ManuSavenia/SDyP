#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
int N;
int T;
int *vector;
double prom;

pthread_mutex_t mutex_prom = PTHREAD_MUTEX_INITIALIZER;

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
    prom = 0;

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

    // Inicializar el vector con valores aleatorios entre 1 y 100
    srand(time(NULL));
    for (i = 0; i < N; i++)
    {
        vector[i] = rand() % 100 + 1; // Valores entre 1 y 100
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

    // Dividir la suma total por N para obtener el promedio
    prom = prom / N;

    printf("Promedio: %f\n", prom);
    printf("Tiempo en segundos: %f\n", dwalltime() - timetick);

    // Liberar memoria
    free(vector);
    return 0;
}

// Función ejecutada por los hilos
void *funcion(void *arg)
{
    int tid = *(int *)arg;
    double suma_local = 0;

    // Calcular los límites del bloque de cada hilo
    int inicio = (tid * N) / T;
    int fin = ((tid + 1) * N) / T;

    for (int i = inicio; i < fin; i++)
    {
        suma_local += vector[i];
    }

    // Sumar la cuenta local al contador global con mutex
    pthread_mutex_lock(&mutex_prom);
    prom += suma_local; // Sumar la suma local sin dividir por N aún
    pthread_mutex_unlock(&mutex_prom);

    pthread_exit(NULL);
}

// Función para calcular el tiempo
double dwalltime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
