#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

using namespace std;

sem_t semaphore;    // Semáforo para exclusão mútua
int *histogram;     // A soma de todos os histogramas locais
pthread_t *threads; // Array de Threads

__thread unsigned int SEED; // SEED para geração de números (rand_r())

typedef struct histogram_data
{
    unsigned long long n_numbers; // Quantidade de números a serem gerados
    int min, max;                 // Mínimo e Máximo do intervalo
    int n_bins;                   // Número de bins no histograma
    float interval;               // Passo de um bin para outro
    int mean;                     // Média utilizada na distribuição 
    int standard_deviation;       // Desvio padrão utilizado na distribuição
} histogram_data;

/*
* Valores próximos a média são mais prováveis
* O desvio padrão afeta a dispersão de valores gerados pela média
*/
float newDistribution(int mean, int standard_deviation)
{

    float r, v1, v2, random_number;

    r = 2;
    while (r >= 1)
    {
        v1 = (2 * ((float)rand_r(&SEED) / (float)RAND_MAX) - 1);
        v2 = (2 * ((float)rand_r(&SEED) / (float)RAND_MAX) - 1);
        r = (v1 * v1) + (v2 * v2);
    }

    r = sqrt(-2 * log(r) / r);

    random_number = mean + standard_deviation * v1 * v2;

    return (random_number);
}

// Gera um número semi-aleatório de bins
int genBins(int seed)
{
    srand(seed);
    return rand() % 1000;
}

// Calcula um histograma com base em um bloco de números
void *PTH_HIST(void *hist_numbers_arr)
{
    histogram_data *arg = (histogram_data *)hist_numbers_arr;
    unsigned long long i; // Variável auxiliar
    float num;            // Número gerado randomicamente 
    int bin_index;        // Identificador do bin

    // Determina seed dinâmica para geração de números
    SEED = (unsigned int)(time(NULL));

    // Alloca espaço para o histograma local (Array)
    int *local_histogram = (int *)calloc(arg->n_bins, sizeof(int));

    // Calcula o histograma para o bloco de dados
    for (i = 0; i < arg->n_numbers; i++)
    {
        num = newDistribution(arg->mean, arg->standard_deviation);
        bin_index = floor((num - arg->min) / arg->interval);
        local_histogram[bin_index] += 1;
    }

    /************************ INÍCIO DA ÁREA CRÍTICA ************************/

    sem_wait(&semaphore);

    for (i = 0; i < (arg->n_bins); ++i)
    {
        histogram[i] += local_histogram[i];
    }

    sem_post(&semaphore);

    /************************** FIM DA ÁREA CRÍTICA **************************/

    free(local_histogram);
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;

    float min, max;                     // Mínimo e Máximo do intervalo
    unsigned long long n_numbers;       // Tamanho da sequência de números aleatórios.
    int n_bins;                         // Número de bins no histograma
    int n_threads;                      // Número de threads
    float interval;                     // Intervalo entre cada Bin do histograma
    unsigned long long local_n_numbers; // Bloco para cada Thread
    int mean;                           // Média utilizada na distribuição 
    int standard_deviation;             // Desvio padrão utilizado na distribuição
    unsigned long long i, j;            // Variáveis auxiliares

    // Tratamento para entradas insuficiente
    if (argc != 3)
    {
        fprintf(stderr, "Modelo de parâmetros: <n_threads> <n_numbers>\n", argv[0]);
        return -1;
    }

    // Atribuição dos parâmetros
    n_threads = atoi(argv[1]);
    min = 1500.00;
    max = 1700.00;
    n_numbers = strtoull(argv[2], NULL, 10);
    mean = 3232 / 2;
    standard_deviation = 72;

    // Seed fixa para o número de Bins
    n_bins = genBins((unsigned int)(time(NULL)));

    // Definição do intervalo entre cada Bin
    interval = (max - min) / (float)n_bins;

    // Definição do tamanho do bloco para cada thread
    local_n_numbers = n_numbers / n_threads;

    // Inicialização do semáforo. Apenas 1 thread acessará a região crítica por vez
    sem_init(&semaphore, 0, 1);

    // Vetor de threads de tamanho passado por argv
    threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));

    // Inicializa o espaço referente aos dados base para cada bloco
    histogram_data *args = (histogram_data *)calloc(n_threads, sizeof(histogram_data));

    // Alocação do espaço referente a todos os histogramas locais
    histogram = (int *)calloc(n_bins, sizeof(int));

    gettimeofday(&start, 0);

    for (i = 0; i < n_threads; ++i)
    {
        // Atribuição do bloco de sequência para a thread processar
        args[i].n_numbers = local_n_numbers;
        args[i].min = min;
        args[i].max = max;
        args[i].n_bins = n_bins;
        args[i].interval = interval;
        args[i].mean = mean;
        args[i].standard_deviation = standard_deviation;

        // Instancia a thread para calcular o histograma deste bloco
        pthread_create(&threads[i], NULL, PTH_HIST,
                       (void *)&args[i]);
    }

    // Conclusão do processo paralelo
    for (i = 0; i < n_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // Adeus semáforo :c
    sem_destroy(&semaphore);

    gettimeofday(&stop, 0);

    // Impressão do histograma
    {
        int max_n_bins = 0;

        // Busca Bin máximo
        for (i = 0; i < n_bins; ++i)
        {
            if (histogram[i] > max_n_bins)
                max_n_bins = histogram[i];
        }

        // Imprime o array histograma
        printf(
            "\nIntervalo da sequencia: [%.2f, %.2f]\nTamanho da sequencia: %llu\nNumero de "
            "Bins: %d\n"
            "Intervalo: %.2f\n"
            "Media da distribuicao: %d\n"
            "Desvio padrao da distribuicao: %d\n\n",
            min, max, n_numbers, n_bins, interval, mean, standard_deviation);
        for (i = 0; i < n_bins; ++i)
        {
            float bin_index = (float)i * interval + min;
            printf("[%.2f, %.2f) |", bin_index, bin_index + interval);
            int row_width = floor(((float)histogram[i] / (float)max_n_bins) * 40.f);
            for (j = 0; j < row_width; ++j)
            {
                printf("#");
            }
            printf(" %d\n", histogram[i]);
        }
        /* printf("[");
        for (i = 0; i < n_bins; ++i)
        {
            float bin_index = (float)i * interval + min;
            printf("[%.2f, %.2f),", bin_index, bin_index + interval);
        }
        printf("]\n\n");
        printf("\n[");
        for (i = 0; i < n_bins; i++)
        {
            printf("%d,", histogram[i]);
        }
        printf("]\n");
        fflush(stdout); */
    }

    // Impressão de resultados em arquivo
    {
        FILE *fp;
        char outputFilename[] = "./paralelo/tempo_pth_hist.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp,
                "\tTempo: %1.2e \tIntervalo: [%.2f, %.2f] \tTamanho da sequencia: %llu \tNumero de "
                "Bins: %d \t"
                "Intervalo: %.2f \t"
                "Media da distribuicao: %d \t"
                "Desvio padrao da distribuicao: %d\n",
                ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
                min, max, n_numbers, n_bins, interval, mean, standard_deviation);
        /*         fprintf(fp, "\t[");
        for (i = 0; i < n_bins; ++i)
        {
            float bin_index = (float)i * interval + min;
            fprintf(fp,"[%.2f, %.2f),", bin_index, bin_index + interval);
        }
        fprintf(fp, "]\n");
        fprintf(fp, "\t[");
        for (i = 0; i < n_bins; i++)
        {
            fprintf(fp, "%d,", histogram[i]);
        }
        fprintf(fp, "]\n"); */
        fclose(fp);
    }
    // Liberação do espaço utilizado
    free(args);
    free(histogram);
    free(threads);

    // Fim :)
    pthread_exit(NULL);
}