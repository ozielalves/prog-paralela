#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <sys/time.h>
#include "mpi.h"

using namespace std;

// Recebe os argumentos passados por linha de comando
void getArgs(int argc, char *argv[], int *local_n_ptr, int *global_n_ptr, int my_rank, int p)
{

   if (my_rank == 0)
   {
      /*       if (argc != 3) {
         Usage(argv[0]);
         *global_n_ptr = -1;  /* Bad args, quit 
      } else {
         *gi_p = argv[1][0];
         if (*gi_p != 'g' && *gi_p != 'i') {
            Usage(argv[0]);
            *global_n_ptr = -1;  /* Bad args, quit 
         } else { */
      *global_n_ptr = strtol(argv[1], NULL, 10);
      // Divisao do T. do problema pela qtd. de processos nao e exata
      if (*global_n_ptr % p != 0)
      {
         printf("O tamanho do problema deve ser divisível por p");
         exit(-1);
      }
      /* } */
   }

   MPI_Bcast(global_n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);

   if (*global_n_ptr <= 0)
   {
      MPI_Finalize();
      exit(-1);
   }

   *local_n_ptr = *global_n_ptr / p;
}

// Gera uma lista aleatória de tamanho n ou "tamanho"
void genList(int *list, int tamanho, int my_rank)
{
   int *list_aux = NULL;
   srandom(42);
   for (size_t i = 0; i < tamanho; i++)
   {
      list_aux[i] = random() % 100;
   }

   MPI_Scatter(list_aux, tamanho, MPI_INT, list, tamanho, MPI_INT, 0, MPI_COMM_WORLD);

   if (my_rank == 0){
      free(list_aux);
   }
}

// Imprime a lista global gerada pela união das parcelas de cada processo
void printGlobalList(int *local_list, int local_n, int my_rank, int p)
{
   int *list_aux = NULL;
   int i, n;

   if (my_rank == 0)
   {
      n = p * local_n;
      list_aux = (int *)malloc(n * sizeof(int));
      MPI_Gather(local_list, local_n, MPI_INT, list_aux, local_n, MPI_INT, 0, MPI_COMM_WORLD);
      printf("Global list:\n");
      for (i = 0; i < n; i++)
         printf("%d ", list_aux[i]);
      printf("\n\n");
      free(list_aux);
   }
   else
   {
      MPI_Gather(local_list, local_n, MPI_INT, list_aux, local_n, MPI_INT, 0, MPI_COMM_WORLD);
   }
}

// Compara 2 inteiros "a" e "b" 
/* int cmp(const void* a_ptr, const void* b_ptr) {
   int a = *((int*)a_ptr);
   int b = *((int*)b_ptr);

   if (a < b)
      return -1;
   else if (a == b)
      return 0;
   else
      return 1;
} */

void Merge_split_low(int local_A[], int temp_B[], int temp_C[],
        int local_n) {
   int ai, bi, ci;

   ai = 0;
   bi = 0;
   ci = 0;
   while (ci < local_n) {
      if (local_A[ai] <= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci++; ai++;
      } else {
         temp_C[ci] = temp_B[bi];
         ci++; bi++;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

void Merge_split_high(int local_A[], int temp_B[], int temp_C[],
        int local_n) {
   int ai, bi, ci;

   ai = local_n-1;
   bi = local_n-1;
   ci = local_n-1;
   while (ci >= 0) {
      if (local_A[ai] >= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci--; ai--;
      } else {
         temp_C[ci] = temp_B[bi];
         ci--; bi--;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

void Print_list(int local_A[], int local_n, int rank) {
   int i;
   printf("%d: ", rank);
   for (i = 0; i < local_n; i++)
      printf("%d ", local_A[i]);
   printf("\n");
}

void Print_local_lists(int local_A[], int local_n,
         int my_rank, int p) {
   int*       A;
   int        q;
   MPI_Status status;

   if (my_rank == 0) {
      A = (int*) malloc(local_n*sizeof(int));
      Print_list(local_A, local_n, my_rank);
      for (q = 1; q < p; q++) {
         MPI_Recv(A, local_n, MPI_INT, q, 0, MPI_COMM_WORLD, &status);
         Print_list(A, local_n, q);
      }
      free(A);
   } else {
      MPI_Send(local_A, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);
   }
}

// Ordena a Lista usando Odd and Even Sort 
void oddEvenSort(int *list, int n) 
{ 
    bool isSorted = false; // Flag que indica se a lista está ordenada
  
    while (!isSorted) 
    { 
        isSorted = true; 
  
        // Bubble sort nos elementos começando com index ímpar (Odd)
        for (size_t i = 1; i <= n-2; i = i+2) 
        { 
            if (list[i] > list[i+1]) 
             { 
                swap(list[i], list[i+1]); 
                isSorted = false; 
              } 
        } 
  
        // Bubble sort nos elementos começando com index par (Even)
        for (size_t i = 0; i<=n-2; i=i+2) 
        { 
            if (list[i] > list[i+1]) 
            { 
                swap(list[i], list[i+1]); 
                isSorted = false; 
            } 
        } 
    } 
  
    return; 
}

void oddEvenIter(int local_list[], int temp_A[], int temp_B[],
        int local_n, int phase, int even_partner, int odd_partner,
        int my_rank, int p) {
   MPI_Status status;

   if (phase % 2 == 0) {  /* Even phase, odd process <-> rank-1 */
      if (even_partner >= 0) {
         MPI_Sendrecv(local_list, local_n, MPI_INT, even_partner, 0,
            temp_A, local_n, MPI_INT, even_partner, 0, MPI_COMM_WORLD,
            &status);
         if (my_rank % 2 != 0)
            Merge_split_high(local_list, temp_A, temp_B, local_n);
         else
            Merge_split_low(local_list, temp_A, temp_B, local_n);
      }
   } else { /* Odd phase, odd process <-> rank+1 */
      if (odd_partner >= 0) {
         MPI_Sendrecv(local_list, local_n, MPI_INT, odd_partner, 0,
            temp_A, local_n, MPI_INT, odd_partner, 0, MPI_COMM_WORLD,
            &status);
         if (my_rank % 2 != 0)
            Merge_split_low(local_list, temp_A, temp_B, local_n);
         else
            Merge_split_high(local_list, temp_A, temp_B, local_n);
      }
   }
}

void Sort(int *local_list, int local_n, int my_rank, int p) {
   int phase; 
   int *temp_A, *temp_B;
   int even_partner;  /* phase is even or left-looking */
   int odd_partner;   /* phase is odd or right-looking */

   /* Temporary storage used in merge-split */
   temp_A = (int*) malloc(local_n*sizeof(int));
   temp_B = (int*) malloc(local_n*sizeof(int));

   /* Find partners:  negative rank => do nothing during phase */
   if (my_rank % 2 != 0) {
      even_partner = my_rank - 1;
      odd_partner = my_rank + 1;
      if (odd_partner == p) odd_partner = -1;  // Idle during odd phase
   } else {
      even_partner = my_rank + 1;
      if (even_partner == p) even_partner = -1;  // Idle during even phase
      odd_partner = my_rank-1;
   }

   // Ordena a lista local
   oddEvenSort(local_list, local_n);

   for (phase = 0; phase < p; phase++)
      oddEvenIter(local_list, temp_A, temp_B, local_n, phase,
             even_partner, odd_partner, my_rank, p);

   free(temp_A);
   free(temp_B);
}

int main(int argc, char **argv)
{
   struct timeval start, stop; // Intervalo de tempo calculado ao fim
   gettimeofday(&start, 0);

   int my_rank = 0; // Rank do meu processo
   int p = 0;       // Numero de processos
   int *local_list; // Lista local => (size) = número de elementos locais * (size of int)
   int global_n;    // Número de elementos na Lista global
   int local_n;     // Número de elementos na Lista local (em cada processo)

   MPI_Init(&argc, &argv);

   // Rank do processo
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   // Quantos processos então sendo usados
   MPI_Comm_size(MPI_COMM_WORLD, &p);

   // Recebe a lista aser ordenada
   getArgs(argc, argv, &global_n, &local_n, my_rank, p);

   local_list = (int*) malloc(local_n * sizeof(int));

   // Gera lista
   genList(local_list, global_n, my_rank);

   // Bloqueia o processo até todos chegarem nesse ponto
   MPI_Barrier(MPI_COMM_WORLD);

   Sort(local_list, local_n, my_rank, p);
   gettimeofday(&stop, 0);

   printGlobalList(local_list, local_n, my_rank, p);

   free(local_list); // Desaloca a memória para local_list

   /* if (my_rank == 0)
   {
      FILE *fp;
      char outputFilename[] = "tempo_mpi_trapezio.txt";

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
   { /* Nothing 
   } */

   MPI_Finalize();
}
