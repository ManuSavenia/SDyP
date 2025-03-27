#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
int N;
int T;
int *vector;
int monotonico = 1;

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

    for (i = 0; i < N; i++)
    {
        vector[i] = 1; // Valores entre 1 y 100
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

    printf("Tiempo en segundos: %f\n", dwalltime() - timetick);
    printf("El vector es monótonico: %s\n", monotonico ? "Sí" : "No");
    // Liberar memoria
    free(vector);
    return 0;
}

// Función ejecutada por los hilos
void *funcion(void *arg)
{
    int tid = *(int *)arg;
    int monotonico_loc = 1;
    int asc, des;
    // Calcular los límites del bloque de cada hilo
    int inicio = (tid * N) / T;
    int fin = ((tid + 1) * N) / T;

    if (vector[inicio] < vector[inicio + 1])
    {
        asc = 1;
        des = 0;
    }
    else
    {
        asc = 0;
        des = 1;
    }
    for (int i = inicio; i < fin; i++)
    {
        if (asc)
        {
            if (vector[i] > vector[i + 1])
            {
                monotonico_loc = 0;
            }
        }
        else
        {
            if (vector[i] < vector[i + 1])
            {
                monotonico_loc = 0;
            }
        }
    }

    // Sumar la cuenta local al contador global con mutex
    pthread_mutex_lock(&mutex_prom);
    monotonico = monotonico && monotonico_loc;
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
