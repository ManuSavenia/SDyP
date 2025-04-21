#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <string.h>
fProcesoTipoA()
{
    // Función que implementa el comportamiento de los procesos de tipo A
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
    // Parametros: [numero procesos, texto, cantPalabras(N) modo(secuencial o paralelo)]
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
        char *texto = atoi(argv[1]);
        int L = strlen(texto);
        char aux,palabras[N];
        int contador[N];
        for(int i = 0; i < N; i++)
        {
            palabras[i] = '\0';
            contador[i] = 0;
        }
        int i = 0,j = 0;
        while(aux != '\0')
        {
            aux = texto[i];
            if(aux == ' ')
            {
                palabras[j] = '\0';
                contador[j]++;
                j++;
            }
            else
            {
                palabras[j] = aux;
                strcat(palabras[j], temp);
            }
            i++;
        }
    }
    else
    {
        MPI_Init(&argc, &argv);
        int id;
        int cantidadDeProcesos;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
        fProcesoTipoA();
        MPI_Finalize();
    }
    return (0);
}