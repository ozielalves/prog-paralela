#include <stdio.h>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

int merge(int *ina, int lena, int *inb, int lenb, int *out)
{
    int i, j;
    int outcount = 0;

    for (i = 0, j = 0; i < lena; i++)
    {
        while ((inb[j] < ina[i]) && j < lenb)
        {
            out[outcount++] = inb[j++];
        }
        out[outcount++] = ina[i];
    }
    while (j < lenb)
        out[outcount++] = inb[j++];

    return 0;
}

// Ordena a Lista usando Odd and Even Sort
void oddEvenSort(int *list, int n)
{
    bool isSorted = false; // Flag que indica se a lista está ordenada
    int i; 

    while (!isSorted)
    {
        isSorted = true;

        // Bubble sort nos elementos começando com index ímpar (Odd)
        for (i = 1; i <= n - 2; i = i + 2)
        {
            if (list[i] > list[i + 1])
            {
                swap(list[i], list[i + 1]);
                isSorted = false;
            }
        }

        // Bubble sort nos elementos começando com index par (Even)
        for (i = 0; i <= n - 2; i = i + 2)
        {
            if (list[i] > list[i + 1])
            {
                swap(list[i], list[i + 1]);
                isSorted = false;
            }
        }
    }

    return;
}

void printStatus(int my_rank, int iter, char *txt, int *la, int n)
{
    printf("[%d] %s iter %d: <", my_rank, txt, iter);
    for (int j = 0; j < n - 1; j++)
        printf("%d,", la[j]);
    printf("%d>\n", la[n - 1]);
}

void MPI_Pairwise_Exchange(int local_n, int *local_list, int sendrank, int recvrank,
                           MPI_Comm comm)
{

    /*
     * the sending rank just sends the data and waits for the results;
     * the receiving rank receives it, sorts the combined data, and returns
     * the correct half of the data.
     */
    int my_rank; // Rank dos meus processos
    int remote[local_n]; //
    int all[2 * local_n];
    const int mergetag = 1;
    const int sortedtag = 2;

    MPI_Comm_rank(comm, &my_rank);
    if (my_rank == sendrank)
    {
        MPI_Send(local_list, local_n, MPI_INT, recvrank, mergetag, MPI_COMM_WORLD);
        MPI_Recv(local_list, local_n, MPI_INT, recvrank, sortedtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(remote, local_n, MPI_INT, sendrank, mergetag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge(local_list, local_n, remote, local_n, all);

        int theirstart = 0, mystart = local_n;
        if (sendrank > my_rank)
        {
            theirstart = local_n;
            mystart = 0;
        }
        MPI_Send(&(all[theirstart]), local_n, MPI_INT, sendrank, sortedtag, MPI_COMM_WORLD);
        for (int i = mystart; i < mystart + local_n; i++)
            local_list[i - mystart] = all[i];
    }
}

// Ordenação local seguida de swap executado em todos os processos
int MPI_OETS(int n, int *list, MPI_Comm comm)
{
    int my_rank, p, i;
    int root_rank = 0; 
    int *local_list; // Lista local

    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &p);

    local_list = (int *)calloc(n / p, sizeof(int)); // Aloca e inicializa

    // Divide list em partes iguais "local_list" para cada processo
    MPI_Scatter(list, n / p, MPI_INT, local_list, n / p, MPI_INT, root_rank, comm);

    // Ordena a lista local
    oddEvenSort(local_list, n / p);

    // Permutação Odd - Even
    for (i = 1; i <= p; i++)
    {
        printStatus(my_rank, i, "before", local_list, n / p);
        
        // i e o rank do meu processo sao ambos pares ou impares (Even)
        if ((my_rank + i) % 2 == 0)
        {   
            if (my_rank < p - 1)
            {
                MPI_Pairwise_Exchange(n / p, local_list, my_rank, my_rank + 1, comm);
            }
        }
        // Odd
        else if (my_rank > 0)
        {
            MPI_Pairwise_Exchange(n / p, local_list, my_rank - 1, my_rank, comm);
        }
        else {
            /* Nothing */
        }
    }

    printStatus(my_rank, i - 1, "after", local_list, n / p);

    // gather local_list to list
    MPI_Gather(local_list, n / p, MPI_INT, list, n / p, MPI_INT,
               root_rank, comm);

    if (my_rank == root_rank)
        printStatus(my_rank, i, " all done ", list, n);

    return MPI_SUCCESS;
}

// Gera lista de n elementos com números randomicos com números randomicos
void genList(int *list, int n)
{
    srandom(42);
    for (int i = 0; i < n; i++)
    {
        list[i] = random() % 100;
    }
}

int main(int argc, char **argv)
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int n;     // Numero de elementos na lista
    int *list; // Lista para ser ordenada

    MPI_Init(&argc, &argv);

    n = atoi(argv[1]);                     // Tamanho da lista
    list = (int *)malloc(n * sizeof(int)); // Aloca o tamanho da lista

    // Gera lista
    genList(list, n);

    // Ordena a lista
    MPI_OETS(n, list, MPI_COMM_WORLD);

    gettimeofday(&stop, 0);

    /* FILE *fp;
    char outputFilename[] = "tempo_mpi_oets.txt";

    fp = fopen(outputFilename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
        exit(1);
    }

    fprintf(fp, "\tTempo: %1.2e\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)));

    fclose(fp); */
    
    free(list);
    
    MPI_Finalize();

    return 0;
}