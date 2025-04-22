#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <string.h>
void fProcesoTipoA(int id, int P, int N, char *texto)
{
    int L = strlen(texto);
    char palabras[N][50];
    int contadorLocal[N];
    int desde = id * (N / P);
    int hasta = (id == P - 1) ? N : desde + (N / P);

    for (int i = 0; i < N; i++)
        contadorLocal[i] = 0;
    if (id == 0)
    {
        int indice = 0;
        char palabraActual[50];
        int pos = 0;
        for (int i = 0; i <= L; i++)
        {
            if (texto[i] == ' ' || texto[i] == '\0')
            {
                if (pos > 0)
                {
                    palabraActual[pos] = '\0';
                    strcpy(palabras[indice], palabraActual);
                    indice++;
                    pos = 0;
                }
            }
            else
            {
                if (pos < 49)
                    palabraActual[pos++] = texto[i];
            }
        }
    }
    MPI_Bcast(palabras, N * 50, MPI_CHAR, 0, MPI_COMM_WORLD);

    for (int i = desde; i < hasta; i++)
    {
        if (strlen(palabras[i]) == 0)
            continue;

        contadorLocal[i] = 1;
        for (int j = i + 1; j < N; j++)
        {
            if (strcmp(palabras[i], palabras[j]) == 0)
            {
                contadorLocal[i]++;
                strcpy(palabras[j], "");
            }
        }
    }
    int contadorGlobal[N];
    MPI_Reduce(contadorLocal, contadorGlobal, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (id == 0)
    {
        char palabrasMAX[5][50];
        int contMAX[5], indicesMAX[5];

        for (int i = 0; i < 5; i++)
        {
            int max = 0;
            for (int j = 0; j < N; j++)
            {
                if (contadorGlobal[j] > max)
                {
                    max = contadorGlobal[j];
                    indicesMAX[i] = j;
                    contMAX[i] = max;
                    strcpy(palabrasMAX[i], palabras[j]);
                }
            }
            contadorGlobal[indicesMAX[i]] = 0;
        }

        printf("Top 5 palabras más repetidas:\n");
        for (int i = 0; i < 5; i++)
        {
            printf("La palabra '%s' se repite %d veces\n", palabrasMAX[i], contMAX[i]);
        }
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
    // Parametros: [numero procesos, texto, cantPalabras(N), modo(secuencial o paralelo)]
    //  Modo 0: secuencial
    //  Modo 1: paralelo
    if (argc != 5)
    {
        printf("Error en los argumentos\n");
        printf("Uso: %s <numero_procesos> <texto> <Npalabras> <modo>\n", argv[0]);
        return 1;
    }

    int modo = atoi(argv[4]);
    int N = atoi(argv[3]);
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

        char *texto = argv[2];
        int L = strlen(texto);
        int contadorPalabras[N];
        char palabras[N][50];
        char palabrasMAX[5][50];
        int indicesMAX[5], contMAX[5];
        int indice = 0;
        char palabraActual[50] = "";
        for (int i = 0; i < N; i++)
            contadorPalabras[i] = 1;

        int pos = 0;
        for (int i = 0; i <= L; i++)
        {
            if (texto[i] == ' ' || texto[i] == '\0')
            {
                if (pos > 0)
                {
                    palabraActual[pos] = '\0';
                    strcpy(palabras[indice], palabraActual);
                    indice++;
                    pos = 0;
                }
            }
            else
            {
                if (pos < 49)
                    palabraActual[pos++] = texto[i];
            }
        }
        for (int i = 0; i < indice; i++)
        {
            if (strlen(palabras[i]) == 0)
                continue;

            for (int j = i + 1; j < indice; j++)
            {
                if (strcmp(palabras[i], palabras[j]) == 0)
                {
                    contadorPalabras[i]++;
                    strcpy(palabras[j], "");
                }
            }
        }
        for (int i = 0; i < 5; i++)
        {
            int max = 0;
            for (int j = 0; j < indice; j++)
            {
                if (contadorPalabras[j] > max && strlen(palabras[j]) > 0)
                {
                    max = contadorPalabras[j];
                    indicesMAX[i] = j;
                    contMAX[i] = max;
                    strcpy(palabrasMAX[i], palabras[j]);
                }
            }
            contadorPalabras[indicesMAX[i]] = 0;
        }

        for (int i = 0; i < 5; i++)
        {
            printf("La palabra '%s' se repite %d veces\n", palabrasMAX[i], contMAX[i]);
        }

        printf("Tiempo de ejecucion secuencial: %f segundos\n", dwalltime() - timetick);
    }

    else
    {
        timetick = dwalltime();
        MPI_Init(&argc, &argv);
        int id;
        int cantidadDeProcesos;
        MPI_Comm_rank(MPI_COMM_WORLD, &id);
        MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
        fProcesoTipoA(id, cantidadDeProcesos, N, argv[2]);
        MPI_Finalize();
        printf("Tiempo de ejecucion paralelo: %f segundos\n", dwalltime() - timetick);
    }
    return (0);
}