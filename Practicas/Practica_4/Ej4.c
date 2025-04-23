#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
void merge(int *arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int *arr, int l, int r)
{
    if (l < r)
    {
        int m = (l + r) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void mergeArrays(int *a, int sizeA, int *b, int sizeB, int *dest)
{
    int i = 0, j = 0, k = 0;
    while (i < sizeA && j < sizeB)
    {
        if (a[i] <= b[j])
            dest[k++] = a[i++];
        else
            dest[k++] = b[j++];
    }
    while (i < sizeA)
        dest[k++] = a[i++];
    while (j < sizeB)
        dest[k++] = b[j++];
}

void fProcesoTipoA(int N, int id, int P)
{
    int localSize = N / P;
    int *V = NULL;
    int *localData = (int *)malloc(localSize * sizeof(int));

    if (id == 0)
    {
        V = (int *)malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
        {
            V[i] = N - i;
        }
    }

    // Distribuir partes del vector
    MPI_Scatter(V, localSize, MPI_INT, localData, localSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso ordena su bloque
    mergeSort(localData, 0, localSize - 1);

    // Mezcla en árbol binario
    int step = 1;
    while (step < P)
    {
        if (id % (2 * step) == 0)
        {
            if (id + step < P)
            {
                int recvSize;
                MPI_Recv(&recvSize, 1, MPI_INT, id + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int *recvData = (int *)malloc(recvSize * sizeof(int));
                MPI_Recv(recvData, recvSize, MPI_INT, id + step, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *merged = (int *)malloc((localSize + recvSize) * sizeof(int));
                mergeArrays(localData, localSize, recvData, recvSize, merged);
                free(localData);
                free(recvData);
                localData = merged;
                localSize += recvSize;
            }
        }
        else
        {
            int partner = id - step;
            MPI_Send(&localSize, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Send(localData, localSize, MPI_INT, partner, 1, MPI_COMM_WORLD);
            free(localData);
            return;
        }
        step *= 2;
    }

    if (id == 0)
    {
        printf("Primeros 20 elementos ordenados:\n");
        for (int i = 0; i < (N < 20 ? N : 20); i++)
        {
            printf("%d ", localData[i]);
        }
        printf("\n");
        free(localData);
        free(V);
    }
}

double dwalltime()
{
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}

int main(int argc, char *argv[])
{
    // Parametros: [numero procesos, tamanio vector, modo(secuencial o paralelo)]
    //  Modo 0: secuencial
    //  Modo 1: paralelo
    if (argc != 4)
    {
        printf("Error en los argumentos\n");
        printf("Uso: %s <numero_procesos> <tamanio_matriz> <modo>\n", argv[0]);
        return 1;
    }

    int modo = atoi(argv[3]);
    int N = atoi(argv[2]);
    double timetick;

    if (N <= 0)
    {
        printf("El tamanio de la matriz debe ser mayor a 0\n");
        return 1;
    }
    if (modo != 0 && modo != 1)
    {
        printf("Modo no valido. Debe ser 0 (secuencial) o 1 (paralelo)\n");
        return 1;
    }
    if (modo == 0)
    {
        timetick = dwalltime();
        int *vector = (int *)malloc(sizeof(int) * N);

        // Inicializar el vector en orden inverso (peor caso)
        for (int i = 0; i < N; i++)
        {
            vector[i] = N - i;
        }

        mergeSort(vector, 0, N - 1);

        printf("Vector ordenado secuencialmente.\n");
        free(vector);
        printf("Tiempo en segundos secuencial: %f\n", dwalltime() - timetick);
    }
    else
    {
        timetick = dwalltime();
        MPI_Init(&argc, &argv);
        int id;
        int cantidadDeProcesos;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
        fProcesoTipoA(N, id, cantidadDeProcesos);
        MPI_Finalize();
        printf("Tiempo en segundos paralelo %f\n", dwalltime() - timetick);
    }
    return (0);
}
//speedup: 0.1239
// eficiencia: 0.0309