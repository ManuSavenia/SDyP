#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <string.h>
int *mm(int *parteA, int *B, int *parteC, int N, int filas)
{
    int i, j, k;
    int iN;
    int jN;
    int suma;
    for (i = 0; i < filas; i++)
    {
        iN = i * N;
        for (j = 0; j < N; j++)
        {
            jN = j * N;
            suma = 0;
            for (k = 0; k < N; k++)
            {
                suma += parteA[iN + k] * B[k + jN];
            }
            parteC[iN + j] = suma;
        }
    }

    return parteC;
}

void fProcesoC(int id, int cantidadDeProcesos, int N)
{
    // id: id del proceso
    // cantidadDeProcesos: cantidad de procesos
    // N: tamaño de la matriz
    int *A, *B, *C;
    int *parteA, *parteC;
    int total = N * N, parte = total / cantidadDeProcesos, filas = N / cantidadDeProcesos, root = 0;
    parteA = (int *)malloc(sizeof(int) * parte);
    parteC = (int *)malloc(sizeof(int) * parte);
    B = (int *)malloc(sizeof(int) * total);
    if (id == root)
    {
        A = (int *)malloc(sizeof(int) * total);
        C = (int *)malloc(sizeof(int) * total);
        for (int i = 0; i < N * N; i++)
        {
            A[i] = 1;
            B[i] = 1;
            C[i] = 0;
        }
    }
    MPI_Scatter(A, parte, MPI_INT, parteA, parte, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(B, total, MPI_INT, root, MPI_COMM_WORLD);
    parteC = mm(parteA, B, parteC, N, filas);
    MPI_Gather(parteC, parte, MPI_INT, C, parte, MPI_INT, root, MPI_COMM_WORLD);
}

void fProcesoPP(int id, int cantidadDeProcesos, int N)
{
    int total = N * N;
    int filas = N / cantidadDeProcesos;
    int cantElemPorProceso = filas * N;
    int root = 0;

    int *A = NULL, *B = NULL, *C = NULL;
    int *parteA = (int *)malloc(sizeof(int) * cantElemPorProceso);
    int *parteC = (int *)malloc(sizeof(int) * cantElemPorProceso);
    B = (int *)malloc(sizeof(int) * total); // Cada proceso necesita B

    if (id == root)
    {
        A = (int *)malloc(sizeof(int) * total);
        C = (int *)malloc(sizeof(int) * total);

        // Inicializar A y B
        for (int i = 0; i < total; i++)
        {
            A[i] = 1;
            B[i] = 1;
            C[i] = 0;
        }

        // Enviar partes de A y toda B a cada proceso
        for (int i = 1; i < cantidadDeProcesos; i++)
        {
            MPI_Send(&A[i * cantElemPorProceso], cantElemPorProceso, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(B, total, MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        // Root también copia su parte
        memcpy(parteA, A, sizeof(int) * cantElemPorProceso);
    }
    else
    {
        // Recibir parte de A y la matriz B
        MPI_Recv(parteA, cantElemPorProceso, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, total, MPI_INT, root, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Calcular parte de C
    mm(parteA, B, parteC, N, filas);

    // Enviar resultados al root
    if (id == root)
    {
        memcpy(C, parteC, sizeof(int) * cantElemPorProceso);
        MPI_Status status;
        for (int i = 1; i < cantidadDeProcesos; i++)
        {
            MPI_Recv(&C[i * cantElemPorProceso], cantElemPorProceso, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
        }
    }
    else
    {
        MPI_Send(parteC, cantElemPorProceso, MPI_INT, root, 2, MPI_COMM_WORLD);
    }

    // Liberar memoria
    free(parteA);
    free(parteC);
    free(B);
    if (id == root)
    {
        free(A);
        free(C);
    }
}

// Para calcular tiempo
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
    // Parametros [numero procesos, tamanio matriz, modo(secuencial o paralelo), operaciones colectivas/ punto a punto]
    // Modo 0: secuencial
    // Modo 1: paralelo
    // Operaciones colectivas: 0
    // Operaciones punto a punto: 1
    if (argc != 5)
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
    // Modo secuencial
    {
        timetick = dwalltime();
        int *A;
        int *B;
        int *C;
        A = (int *)malloc(sizeof(int) * N * N);
        B = (int *)malloc(sizeof(int) * N * N);
        C = (int *)malloc(sizeof(int) * N * N);

        // inicializo las matrices en 1
        for (int i = 0; i < N * N; i++)
        {
            A[i] = 1;
            B[i] = 1;
            C[i] = 0;
        }
        int iN;
        int jN;
        int suma;
        for (int i = 0; i < N; i++)
        {
            iN = i * N;
            for (int j = 0; j < N; j++)
            {
                jN = j * N;
                suma = 0;
                for (int k = 0; k < N; k++)
                {
                    suma += A[iN + k] * B[k + jN];
                }
                C[iN + j] = suma;
            }
        }
        printf("Tiempo en segundos secuencial %f\n", dwalltime() - timetick);
        free(A);
        free(B);
        free(C);
    }
    else
    {
        if (modo == 1)
        // Modo paralelo
        {
            timetick = dwalltime();
            MPI_Init(&argc, &argv);
            int id;
            int cantidadDeProcesos = atoi(argv[1]);
            int C_PP = atoi(argv[4]);
            MPI_Comm_rank(MPI_COMM_WORLD, &id);
            MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
            if (C_PP == 0)
                fProcesoC(id, cantidadDeProcesos, N);
            else
                fProcesoPP(id, cantidadDeProcesos, N);
            MPI_Finalize();
            if (C_PP == 0)
                printf("Tiempo en segundos paralelo C %f\n", dwalltime() - timetick);
            else
                printf("Tiempo en segundos paralelo PP %f\n", dwalltime() - timetick);
        }
    }

    return (0);
}