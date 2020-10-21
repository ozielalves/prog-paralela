#include <stdio.h>
#include <bits/stdc++.h>
#include <sys/time.h>
#include <mpi.h>

using namespace std;

/*
 * arg list_a: Lista 
 * arg len_list_a: Lista 
 * arg list_b: Lista 
 * arg len_list_b: Lista 
 * arg merged_list: Output 
 */

int merge(int *list_a, int len_list_a, int *list_b, int len_list_b, int *merged_list)
{
    int i, j;
    int aux = 0;

    for (i = 0, j = 0; i < len_list_a; i++)
    {
        while ((list_b[j] < list_a[i]) && j < len_list_b)
        {
            merged_list[aux++] = list_b[j++];
        }
        merged_list[aux++] = list_a[i];
    }
    while (j < len_list_b)
        merged_list[aux++] = list_b[j++];

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

/*
 * O rank remetente envia o elemento para fazer o swap e aguardao o retorno 
 * O rank destinatario recebe o elemento, ordena o novo array e retorna o elemento pertencente a o remetente.
 */
void MPI_SWAP(int local_n, int *local_list, int snd_rank, int rcv_rank, MPI_Comm comm)
{

    /*
     * the sending rank just sends the data and waits for the results;
     * the receiving rank receives it, sorts the combined data, and returns
     * the correct half of the data.
     */
    int my_rank;                  // Rank dos meus processos
    int aux_list[local_n];          //
    int merged_list[2 * local_n]; // Lista auxiliar atribuida na função Merge
    const int merge_tag = 1;      // Identifica a comunicação
    const int sorted_tag = 2;     // Identifica a comunicação

    MPI_Comm_rank(comm, &my_rank);

    // Comunicação com o rank vizinho, o rank remetente não está ocioso
    if (my_rank == snd_rank)
    {
        // A rotina é bloqueada até que o rank destinatario receba a o dado
        MPI_Send(local_list, local_n, MPI_INT, rcv_rank, merge_tag, MPI_COMM_WORLD);
        // MPI Status nao necessario para esta rotina
        MPI_Recv(local_list, local_n, MPI_INT, rcv_rank, sorted_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(aux_list, local_n, MPI_INT, snd_rank, merge_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        merge(local_list, local_n, aux_list, local_n, merged_list);

        int start = 0;
        int end = local_n;

        // Fase ODD? EVEN? Mantém o menor elemento na esquerda e o maior elemento é enviado ao remetente
        if (snd_rank > my_rank)
        {
            start = local_n; 
            end = 0;
        }
        else
        {
            /* Nothing */
        }

        // Envia o elemento ao rementente
        MPI_Send(&(merged_list[start]), local_n, MPI_INT, snd_rank, sorted_tag, MPI_COMM_WORLD);

        // Atualiza local_list
        for (int i = end; i < end + local_n; i++)
        {
            local_list[i - end] = merged_list[i];
        }
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

        // Fase par (Even)
        if ((my_rank + i) % 2 == 0)
        {
            if (my_rank < p - 1)
            {
                MPI_SWAP(n / p, local_list, my_rank, my_rank + 1, comm);
            }
        }
        // Fase ímpar (Odd)
        else if (my_rank > 0)
        {
            MPI_SWAP(n / p, local_list, my_rank - 1, my_rank, comm);
        }
        else
        {
            /* Nothing */
        }
    }

    printStatus(my_rank, i - 1, "after", local_list, n / p);

    // gather local_list to list
    MPI_Gather(local_list, n / p, MPI_INT, list, n / p, MPI_INT,
               root_rank, comm);

    // Fim da linha
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