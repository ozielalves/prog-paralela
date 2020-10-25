#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

/*
 * Merge nas duas listas locais de forma a preservar a ordenação
 * 
 * arg list_rcv: local_list_rcv
 * arg len_list_rcv: tamanho de local_list_rcv
 * arg list_snd: local_list_snd
 * arg len_list_snd: tamanho de local_list_snd
 * arg merged_list: lista auxiliar resultante 
 */

int Merge(int *local_list_rcv, int len_local_list_rcv, int *local_list_snd,
          int len_local_list_snd, int *merged_list)
{
    int i, j;
    int aux = 0;

    for (i = 0, j = 0; i < len_local_list_rcv; i++)
    {
        // Garantia de ordem
        while ((local_list_snd[j] < local_list_rcv[i]) && j < len_local_list_snd)
        {
            merged_list[aux++] = local_list_snd[j++];
        }
        merged_list[aux++] = local_list_rcv[i];
    }
    while (j < len_local_list_snd)
    {
        merged_list[aux++] = local_list_snd[j++];
    }

    return 0;
}

/*
 * Join nas duas listas locais (Ordem não preservada)
 * 
 * arg list_rcv: local_list_rcv
 * arg len_list_rcv: tamanho de local_list_rcv
 * arg list_snd: local_list_snd
 * arg len_list_snd: tamanho de local_list_snd
 * arg merged_list: lista auxiliar resultante 
 */

int Join(int *local_list_rcv, int len_local_list_rcv, int *local_list_snd,
          int len_local_list_snd, int *merged_list)
{
    int i, j = 0;
    int aux = 0;

    
    while (i < len_local_list_rcv)
    {
        merged_list[aux++] = local_list_snd[i++];
    }
    // Sem garantia de ordem
    while (j < len_local_list_snd)
    {
        merged_list[aux++] = local_list_snd[j++];
    }

    return 0;
}          

// Ordena a Lista usando Odd and Even Transposition Sort
void oddEvenSort(int *list, int n)
{
    bool isSorted = false; // Flag que indica se a lista está ordenada
    int i;

    while (!isSorted)
    {
        isSorted = true;

        // Fase ímpar (Odd)
        for (i = 1; i <= n - 2; i = i + 2)
        {
            if (list[i] > list[i + 1])
            {
                swap(list[i], list[i + 1]);
                isSorted = false;
            }
        }

        // Fase par (Even)
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

// Imprime o processo de comunicação e resultado final para Debug
void printStatus(int my_rank, int iter, char *txt, int *list, int n)
{
    cout << "Processo " << my_rank << txt << " da comunicacao " << iter << ": [";

    for (int j = 0; j < n - 1; j++)
    {
        cout << list[j] << ",";
    }

    cout << list[n - 1] << "]" << endl;
}

/*
 * O rank remetente envia os dados para fazer o swap e aguarda o retorno. 
 * O rank destinatario recebe os dados, ordena o novo array e retorna 
 * a outra metade que cabe ao rank remetente.
 */
void MPI_SWAP(int local_n, int *local_list, int snd_rank, int rcv_rank, MPI_Comm comm)
{
    int my_rank;                  // Rank dos meus processos
    const int merge_id = 1;       // Identifica a comunicação
    const int sorted_id = 2;      // Identifica a comunicação
    int aux_list[local_n];        // Lista auxiliar cópia da lista local do rank remetente
    int merged_list[2 * local_n]; // Lista auxiliar oriunda do Merge(local_n_rcv, local_n_snd)

    MPI_Comm_rank(comm, &my_rank);

    // Na fase Odd da comunicação
    if (my_rank == snd_rank)
    {
        // A rotina é bloqueada até que o rank destinatario receba a o dado
        MPI_Send(local_list, local_n, MPI_INT, rcv_rank, merge_id, MPI_COMM_WORLD);
        // MPI Status nao necessario para esta rotina
        MPI_Recv(local_list, local_n, MPI_INT, rcv_rank, sorted_id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else
    {
        // Recebe local_list referente ao rank remetente
        MPI_Recv(aux_list, local_n, MPI_INT, snd_rank, merge_id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Uniao da local_list do rank destinatario com a local_list do rank remetente (aux_list)
        Merge(local_list, local_n, aux_list, local_n, merged_list);
        //Join(local_list, local_n, aux_list, local_n, merged_list);
        
        // Ordenação pós join
        //oddEvenSort(merged_list, 2 * local_n);

        int start = 0;
        int end = local_n;

        // Garante que o Maior elemento suba && Menor elemento desca
        if (my_rank < snd_rank)
        {
            start = local_n;
            end = 0;
        }
        else
        {
            /* Nada */
        }

        // Envia a parte que cabe ao rementente já ordenada, após o Merge
        MPI_Send(&(merged_list[start]), local_n, MPI_INT, snd_rank, sorted_id, MPI_COMM_WORLD);

        // Atualiza local_list
        for (int i = end; i < end + local_n; i++)
        {
            // O rank destinatario fica com os Maiores elementos
            local_list[i - end] = merged_list[i];
        }
    }
}

// Ordenação local seguida de comunicacao-swap
void MPI_OETS(int n, int *list, MPI_Comm comm)
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
        /* printStatus(my_rank, i, " Antes", local_list, n / p); */

        // Fase ímpar (Odd)
        if ((my_rank + i) % 2 == 0)
        {
            if (my_rank < p - 1)
            {
                MPI_SWAP(n / p, local_list, my_rank, my_rank + 1, comm);
            }
        }
        // Fase par (Even)
        else if (my_rank > 0)
        {
            MPI_SWAP(n / p, local_list, my_rank - 1, my_rank, comm);
        }
        else
        {
            /* Nada */
        }
    }

    /* printStatus(my_rank, i - 1, " Depois", local_list, n / p); */

    // Reune cada local_list na lista principal agora de forma ordeanda
    MPI_Gather(local_list, n / p, MPI_INT, list, n / p, MPI_INT, root_rank, comm);

    // Fim da linha
    /* if (my_rank == root_rank)
        printStatus(my_rank, i, "Lista Ordenada", list, n); */

    /*     return MPI_SUCCESS; */
}

// Gera lista de n elementos com números randomicos com números randomicos
void genList(int *list, int n)
{
    srandom(42);
    for (int i = 0; i < n; i++)
    {
        list[i] = random() % 1000;
    }
}

int main(int argc, char **argv)
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int n;       // Numero de elementos na lista
    int *list;   // Lista para ser ordenada
    int my_rank; // Rank do meu processo

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    n = atoi(argv[1]);                     // Tamanho da lista
    list = (int *)malloc(n * sizeof(int)); // Aloca o tamanho da lista

    // Gera lista
    genList(list, n);

    // Ordena a lista
    MPI_OETS(n, list, MPI_COMM_WORLD);

    gettimeofday(&stop, 0);

    if (my_rank == 0) 
    {
        FILE *fp;
        char outputFilename[] = "./paralelo/tempo_mpi_oets.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)));

        fclose(fp);
    }
    else
    {
        /* Nada */
    }
    
    free(list);

    MPI_Finalize();

    return 0;
}