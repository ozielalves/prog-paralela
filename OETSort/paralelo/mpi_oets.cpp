#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <sys/time.h>
#include "mpi.h"

void getArgs(int argc, char *argv[], int my_rank, double *n_ponteiro)
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

    int my_rank = 0; // Rank do meu processo
    int p = 0;       // Numero de processos
    char g_i;        // g ou i dependendo da escolha do usuário; g: gera lista aleatória para ordenação, i: ordena lista informada
    int *local_list; // Lista local => (size) = número de elementos locais * (size of int)
    int global_n;    // Número de elementos na Lista global
    int local_n;     // Número de elementos na Lista local (em cada processo)

    MPI_Init(&argc, &argv);

    // Rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Quantos processos então sendo usados
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Recebe a lista aser ordenada
    getArgs(argc, argv, &global_n, &local_n, &g_i, my_rank, p, comm);
   local_list = (int*) malloc(local_n*sizeof(int));

   // generate random list based on user input
   if (g_i == 'g') {
      Generate_list(local_list, local_n, my_rank);
      Print_local_lists(local_list, local_n, my_rank, p, comm);
   }
   // read in user defined list from command line
   else {
      Read_list(local_list, local_n, my_rank, p, comm);
   }

    // Bloqueia o processo até todos chegarem nesse ponto
    MPI_Barrier(MPI_COMM_WORLD);

    Sort(local_list, local_n, my_rank, p, comm);
    gettimeofday(&stop, 0);

    /* MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm); */
    
    #  ifdef DEBUG
    Print_local_lists(local_list, local_n, my_rank, p, comm);
    fflush(stdout);
    #  endif

    Print_global_list(local_list, local_n, my_rank, p, comm);

    free(local_list);  // Desaloca a memória para local_list

    if (my_rank == 0)
    {
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
    { /* Nothing */
    }

    MPI_Finalize();
}
