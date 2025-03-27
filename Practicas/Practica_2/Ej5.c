#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

// Variables globales
int N, T;
int *vector;
pthread_mutex_t mutex_merge = PTHREAD_MUTEX_INITIALIZER;

// Prototipos de funciones
double dwalltime();
void *funcion(void *arg);
void merge(int *arr, int left, int mid, int right);
void mergeSort(int *arr, int left, int right);
void mergeSections();

// Estructura para pasar argumentos a los hilos
typedef struct
{
    int id;
    int inicio;
    int fin;
} ThreadArgs;

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
    ThreadArgs thread_args[T];
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
        vector[i] = rand() % 100 + 1;
    }

    // Iniciar la cuenta de tiempo
    timetick = dwalltime();

    // Crear hilos
    for (int id = 0; id < T; id++)
    {
        thread_args[id].id = id;
        thread_args[id].inicio = (id * N) / T;
        thread_args[id].fin = ((id + 1) * N) / T - 1;

        pthread_create(&misThreads[id], NULL, funcion, &thread_args[id]);
    }

    // Esperar a que los hilos terminen
    for (int id = 0; id < T; id++)
    {
        pthread_join(misThreads[id], NULL);
    }

    // Fusionar las secciones ordenadas por cada hilo
    mergeSections();

    printf("Tiempo en segundos: %f\n", dwalltime() - timetick);

    // Liberar memoria
    free(vector);
    return 0;
}

// Función ejecutada por los hilos (cada hilo ordena su parte del vector)
void *funcion(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    mergeSort(vector, args->inicio, args->fin);
    pthread_exit(NULL);
}

// Implementación de Merge Sort
void mergeSort(int *arr, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Fusiona dos mitades ordenadas en un solo array ordenado
void merge(int *arr, int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Crear arreglos temporales
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    // Copiar datos
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Fusionar
    i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    // Copiar elementos restantes
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    // Liberar memoria
    free(L);
    free(R);
}

// Fusiona las secciones ordenadas del vector en una única lista ordenada
void mergeSections()
{
    int size = N / T;
    for (int i = 1; i < T; i++)
    {
        int left = 0;
        int right = ((i + 1) * size) - 1;
        int mid = (i * size) - 1;
        if (right >= N)
            right = N - 1;
        merge(vector, left, mid, right);
    }
}

// Función para calcular el tiempo
double dwalltime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
