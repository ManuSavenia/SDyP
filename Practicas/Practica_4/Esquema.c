#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
fProcesoTipoA()
{
    // Función que implementa el comportamiento de los procesos de tipo A
}
fProcesoTipoB()
{
    // Función que implementa el comportamiento de los procesos de tipo B
}
fProcesoTipoC()
{
    // Función que implementa el comportamiento de los procesos de tipo C
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
    }
    else
    {
        MPI_Init(&argc, &argv);
        int id;
        int cantidadDeProcesos;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
        if (id == 0)
            fProcesoTipoA();
        else if (id >= 1 && id <= 3)
            fProcesoTipoB();
        else
            fProcesoTipoC();
        MPI_Finalize();
    }
    return (0);
}