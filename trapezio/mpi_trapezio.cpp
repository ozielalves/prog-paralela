// Para compilar: mpicc -g -o mpi_trapezio mpi_trapezio.cpp -lstdc++ -lm

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <sys/time.h>
#include "mpi.h"

float f(float x)
{
    return pow(x, 2);
}

float trapezioIntegral(float local_a, float local_b, int local_n, float inc)
{

    float integral;
    float x;
    int i;

    integral = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    for (i = 1; i <= local_n - 1; i++)
    {
        x = x + inc;
        integral = integral + f(x);
    }
    integral = integral * inc;
    return integral;
}

int main(int argc, char **argv)
{
    int my_rank;           // Rank do meu processo
    int p;                 // Numero de processos
    float xa = 0.;         // X Início da figura
    float xb = 40.;        // X Fim da figura
    int n = atoi(argv[1]); // Numero de mini trapezios
    float inc;             // Incremento (Base do Trapezio)
    float local_a;         // X Início da figura LOCAL
    float local_b;         // X Fim da figura LOCAL
    int local_n;           // Numero de mini trapezios LOCAL

    float area_relativa;        // Area relativa ao intervalo
    float area_total;           // Area total
    struct timeval start, stop; // Intervalo de tempo calculado ao fim

    gettimeofday(&start, 0);

    MPI_Init(&argc, &argv);

    // Rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Quantos processos então sendo usados
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // O incremento e n serão os mesmo para todos os processos
    inc = (xb - xa) / n; 
    local_n = n / p;

    // O tamanho de cada intervalo de processo será (local_n * inc)
    local_a = xa + my_rank * local_n * inc;
    local_b = local_a + local_n * inc;
    area_relativa = trapezioIntegral(local_a, local_b, local_n, inc);

    // Soma as integrais calculadas por cada processo
    MPI_Reduce(&area_relativa, &area_total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./trapezio/tempo_mpi_trapezio.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Can't open output file %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)), area_total);

        fclose(fp);
    }
    else
    { /* Nothing */}

    MPI_Finalize();
}
