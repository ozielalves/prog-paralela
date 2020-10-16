#include <iostream>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include <sys/time.h>
#include <random>

using namespace std;

double calcPi(int termos)
{

    // Gerador Mersene twist, SEED: 42
    mt19937 mt(42);

    // Numero real pseudo-aleatorio
    uniform_real_distribution<double> linear_r(0.f, 1.f);

    double sparc = 0;
    for (int i = 1; i < termos; i++)
    {
        double x = linear_r(mt);
        double y = linear_r(mt);

        if (x * x + y * y < 1.0)
        {
            sparc++;
        }
    }

    return sparc;
}

int main(int argc, char **argv)
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int my_rank;
    int p;
    int termos = atoll(argv[1]);
    int termos_local;
    int inicial_local;
    double acertos_parc;
    double acertos;

    MPI_Init(&argc, &argv);

    // Rank do meu processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Descobre quantos processos estao em uso
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Divisao interna
    termos_local = termos / p;

    // Bloqueia o processo até todos chegarem nesse ponto
    MPI_Barrier(MPI_COMM_WORLD);

    acertos_parc = calcPi(termos_local);

    // Soma o numero de acertos por cada processo
    MPI_Reduce(&acertos_parc, &acertos, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0)
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./pi/tempo_mpi_pi.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n",
                ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
                (double)4 * acertos / termos);

        fclose(fp);
    }
    else
    { /* Nothing */ }

    MPI_Finalize();
}