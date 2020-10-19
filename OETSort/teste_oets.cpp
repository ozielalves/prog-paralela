#include <stdio.h>
/* #include <stdlib.h> */
#include<bits/stdc++.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

int merge(int *ina, int lena, int *inb, int lenb, int *out) {
    int i,j;
    int outcount=0;

    for (i=0,j=0; i<lena; i++) {
        while ((inb[j] < ina[i]) && j < lenb) {
            out[outcount++] = inb[j++];
        }
        out[outcount++] = ina[i];
    }
    while (j<lenb)
        out[outcount++] = inb[j++];

    return 0;
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

void printstat(int rank, int iter, char *txt, int *la, int n) {
    printf("[%d] %s iter %d: <", rank, txt, iter);
    for (int j=0; j<n-1; j++)
        printf("%d,",la[j]);
    printf("%d>\n", la[n-1]);
}

void MPI_Pairwise_Exchange(int localn, int *locala, int sendrank, int recvrank,
                           MPI_Comm comm) {

    /*
     * the sending rank just sends the data and waits for the results;
     * the receiving rank receives it, sorts the combined data, and returns
     * the correct half of the data.
     */
    int rank;
    int remote[localn];
    int all[2*localn];
    const int mergetag = 1;
    const int sortedtag = 2;

    MPI_Comm_rank(comm, &rank);
    if (rank == sendrank) {
        MPI_Send(locala, localn, MPI_INT, recvrank, mergetag, MPI_COMM_WORLD);
        MPI_Recv(locala, localn, MPI_INT, recvrank, sortedtag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(remote, localn, MPI_INT, sendrank, mergetag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge(locala, localn, remote, localn, all);

        int theirstart = 0, mystart = localn;
        if (sendrank > rank) {
            theirstart = localn;
            mystart = 0;
        }
        MPI_Send(&(all[theirstart]), localn, MPI_INT, sendrank, sortedtag, MPI_COMM_WORLD);
        for (int i=mystart; i<mystart+localn; i++)
            locala[i-mystart] = all[i];
    }
}

int MPI_OddEven_Sort(int n, int *a, int root, MPI_Comm comm)
{
    int rank, p, i;
    int *local_list;

    MPI_Comm_rank(comm, &rank); 
    MPI_Comm_size(comm, &p);

    local_list = (int *) calloc(n / p, sizeof(int)); // Aloca e inicializa


    // scatter the array a to local_list
    MPI_Scatter(a, n / p, MPI_INT, local_list, n / p, MPI_INT, root, comm);
    
    // sort local_list
    oddEvenSort(local_list, n / p);

    //odd-even part
    for (i = 1; i <= p; i++) {

        printstat(rank, i, "before", local_list, n/p);

        if ((i + rank) % 2 == 0) {  // means i and rank have same nature
            if (rank < p - 1) {
                MPI_Pairwise_Exchange(n / p, local_list, rank, rank + 1, comm);
            }
        } else if (rank > 0) {
            MPI_Pairwise_Exchange(n / p, local_list, rank - 1, rank, comm);
        }

    }

    printstat(rank, i-1, "after", local_list, n/p);

    // gather local_list to a
    MPI_Gather(local_list, n / p, MPI_INT, a, n / p, MPI_INT,
           root, comm);

    if (rank == root)
        printstat(rank, i, " all done ", a, n);

    return MPI_SUCCESS;
}

// Gera lista de n elementos com números randomicos com números randomicos
void genList(int *list, int n) {

    srandom(42);
    for (size_t i = 0; i < n; i++)
    {   
        list[i] = random() % 100;
    }
} 

int main(int argc, char **argv) {

    int n; // Numero de elementos na lista
    int *list; // Lista para ser ordenada

    MPI_Init(&argc, &argv);

    n = atoi(argv[1]); // Tamanho da lista
    list = (int*) malloc(n * sizeof(int)); // Aloca o tamanho da lista e inicializa
    
    // Gera lista
    genList(list, n);

    MPI_OddEven_Sort(n, list, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    return 0;
}