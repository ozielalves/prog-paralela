#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <sys/time.h>
#include "mpi.h"

void getImput(int argc, char *argv[], int my_rank, double *n_ponteiro)
{
    // Determina o valor de n
    if (my_rank == 0)
    {
        *n_ponteiro = atoi(argv[1]);
    }

    // Destribui para os processos
    MPI_Bcast(n_ponteiro, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

int main(int argc, char **argv)
{
    struct timeval start, stop; // Intervalo de tempo calculado ao fim
    gettimeofday(&start, 0);

    int my_rank = 0;           // Rank do meu processo
    int p = 0;                 // Numero de processos
    const double xa = 0.;      // X Início da figura
    const double xb = 30.;     // X Fim da figura
    double n = 0.;             // Numero de mini trapezios
    double inc = 0.;           // Incremento (Base do Trapezio)
    double local_a = 0.;       // X Início da figura LOCAL
    double local_b = 0.;       // X Fim da figura LOCAL
    long long int local_n = 0; // Numero de mini trapezios LOCAL

    double area_relativa = 0.; // Area relativa ao intervalo
    double area_total = 0.;    // Area total

    MPI_Init(&argc, &argv);

    // Rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Quantos processos então sendo usados
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Destribui o valor de n para todos os processos
    getImput(argc, argv, my_rank, &n);

/*     // O incremento e local_n serão os mesmo para todos os processos
    inc = (xb - xa) / n;
    local_n = n / p;

    // O tamanho de cada intervalo de processo será (local_n * inc)
    local_a = xa + my_rank * (local_n * inc);
    local_b = local_a + (local_n * inc); */

    // Bloqueia o processo até todos chegarem nesse ponto
    MPI_Barrier(MPI_COMM_WORLD);

/*     area_relativa = trapezioIntegral(local_a, local_b, local_n); */

    // Soma as integrais calculadas por cada processo
    MPI_Reduce(&area_relativa, &area_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./trapezio/tempo_mpi_trapezio.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)), area_total);

        fclose(fp);
    }
    else
    { /* Nothing */ }

    MPI_Finalize();
}
