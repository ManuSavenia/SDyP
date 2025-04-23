#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
void fProcesoTipoA(int N)
{
    int id, P;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    int *V = NULL, *R = NULL;
    int *localR;
    int desde, hasta;

    int chunk = N / P;
    desde = id * chunk;
    hasta = (id == P - 1) ? N : desde + chunk;

    if (id == 0)
    {
        V = (int *)malloc(sizeof(int) * N);
        for (int i = 0; i < N; i++)
        {
            V[i] = i;
        }
    }

    // Broadcast del vector completo
    if (id != 0)
    {
        V = (int *)malloc(sizeof(int) * N);
    }
    MPI_Bcast(V, N, MPI_INT, 0, MPI_COMM_WORLD);

    localR = (int *)malloc(sizeof(int) * (hasta - desde));

    // Calcular distancia al doble para cada V[i] en [desde, hasta)
    for (int i = desde; i < hasta; i++)
    {
        int aux = V[i] * 2;
        int dist = 0, hay = 0;
        for (int j = i + 1; j < N; j++)
        {
            if (V[j] == aux)
            {
                localR[i - desde] = j - i;
                hay = 1;
                break;
            }
            if (V[j] > aux)
            {
                break;
            }
            dist++;
        }
        if (!hay)
            localR[i - desde] = -1;
    }

    // Preparar estructuras para Gatherv
    int *recvcounts = NULL;
    int *displs = NULL;

    if (id == 0)
    {
        R = (int *)malloc(sizeof(int) * N);
        recvcounts = (int *)malloc(sizeof(int) * P);
        displs = (int *)malloc(sizeof(int) * P);

        for (int i = 0; i < P; i++)
        {
            int d = i * (N / P);
            int h = (i == P - 1) ? N : d + (N / P);
            recvcounts[i] = h - d;
            displs[i] = d;
        }
    }

    // Juntar todos los resultados en R
    MPI_Gatherv(localR, hasta - desde, MPI_INT, R, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (id == 0)
    {
        free(R);
        free(recvcounts);
        free(displs);
    }

    free(V);
    free(localR);
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
        printf("El tamanio del vector debe ser mayor a 0\n");
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
        int *V, *R, aux, y, dist, hay;
        V = (int *)malloc(sizeof(int) * N);
        R = (int *)malloc(sizeof(int) * N);
        for (int i = 0; i < N; i++)
        {
            V[i] = i;
        }
        for (int i = 0; i < N; i++)
        {
            aux = V[i] * 2;
            y = i;
            dist = 0;
            hay = 0;
            for (int j = y + 1; j < N; j++)
            {
                if (V[j] == aux)
                {
                    R[i] = dist;
                    hay = 1;
                    break;
                }
                if (V[j + 1] > aux)
                {
                    hay = 0;
                    break;
                }
                dist++;
            }
            if (!hay)
            {
                R[i] = -1;
            }
        }
        printf("El tiempo de ejecucion secuencial es: %f\n", dwalltime() - timetick);
    }
    else
    {
        timetick = dwalltime();
        MPI_Init(&argc, &argv);
        int id;
        int cantidadDeProcesos;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
        fProcesoTipoA(N);
        MPI_Finalize();
        printf("El tiempo de ejecucion paralelo es: %f\n", dwalltime() - timetick);
    }
    return (0);
}
//speedup: 1.833
//efficiency: 0.611