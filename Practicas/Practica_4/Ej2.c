#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

void fProceso(int id, int cantidadDeProcesos, int N)
{
    // id: id del proceso
    // cantidadDeProcesos: cantidad de procesos
    // N: tamaño de la matriz
    int *A, *B;
    int *parteA;
    int total = N * N, parte = total / cantidadDeProcesos, filas = N / cantidadDeProcesos, root = 0;
    parteA = (int *)malloc(sizeof(int) * parte);
    if (id == root)
    {
        A = (int *)malloc(sizeof(int) * total);
        B = (int *)malloc(sizeof(int) * total);
        for (int i = 0; i < N * N; i++)
        {
            A[i] = 1;
        }
    }
    MPI_Scatter(A, parte, MPI_INT, parteA, parte, MPI_INT, root, MPI_COMM_WORLD);
    int min_local = 99999;
    int max_local = 0;
    int suma_local = 0;

    // Calcular los valores locales
    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int val = parteA[i * N + j];
            suma_local += val;
            if (val < min_local)
                min_local = val;
            if (val > max_local)
                max_local = val;
        }
    }

    // Variables finales (solo usadas en root)
    int min_global, max_global, suma_global;

    // Reducciones
    MPI_Reduce(&min_local, &min_global, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max_local, &max_global, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&suma_local, &suma_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Solo el root calcula el promedio y muestra resultados
    if (id == 0)
    {
        double promedio = (double)suma_global / (N * N);
        printf("Mínimo global: %d\n", min_global);
        printf("Máximo global: %d\n", max_global);
        printf("Promedio global: %.2f\n", promedio);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (A[i] * N + j < promedio)
                {
                    B[i * N + j] = min_global;
                }
                else
                {
                    if (A[i] * N + j > promedio)
                    {
                        B[i * N + j] = max_global;
                    }
                    else
                    {
                        B[i * N + j] = promedio;
                    }
                }
            }
        }
        free(A);
        free(parteA);
        free(B);
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
    // Parametros: [numero procesos, tamanio matriz, modo(secuencial o paralelo)]
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
        int *A, *B;
        int min = 99999, max = 0;
        double promedio = 0;
        A = (int *)malloc(sizeof(int) * N * N);
        B = (int *)malloc(sizeof(int) * N * N);
        for (int i = 0; i < N * N; i++)
        {
            A[i] = 1;
        }
        // encontramos el minimo, maximo y promedio de la matriz
        for (int i = 0; i < N * N; i++)
        {
            if (A[i] < min)
                min = A[i];
            if (A[i] > max)
                max = A[i];
            promedio += A[i];
        }
        promedio = promedio / (N * N);
        printf("El minimo es: %d\n", min);
        printf("El maximo es: %d\n", max);
        printf("El promedio es: %f\n", promedio);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (A[i] * N + j < promedio)
                {
                    B[i * N + j] = min;
                }
                else
                {
                    if (A[i] * N + j > promedio)
                    {
                        B[i * N + j] = max;
                    }
                    else
                    {
                        B[i * N + j] = promedio;
                    }
                }
            }
        }
        free(B);
        free(A);
        printf("El tiempo de ejecucion secuencial es: %f\n", dwalltime() - timetick);
    }
    else
    {
        timetick = dwalltime();
        MPI_Init(&argc, &argv);
        int id;
        int P;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &P);
        fProceso(id, P, N);
        MPI_Finalize();
        printf("El tiempo de ejecucion paralelo es: %f\n", dwalltime() - timetick);
    }
    return (0);
}
//speedup: 0.7468
//efficiency: 0.1867