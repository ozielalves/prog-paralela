#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <random>
#include <sys/time.h>

using namespace std;

sem_t semaphore;    // Semáforo para exclusão mútua
int *histogram;     // A soma de todos os histogramas locais
pthread_t *threads; // Array de Threads

typedef struct calc_hist_numbers_arr_t
{
    float *numbers_arr_start; // Início do intervalo de números
    float *numbers_arr_end;   // Início do intervalo de números
    int min, max;             // Mínimo e Máximo do intervalo
    int n_bins;               // Número de bins no histograma
    float interval;           // Passo de um bin para outro
    int thread_id;            // ID da thread
} calc_hist_numbers_arr_t;

// Calcula um histograma com base em um bloco de números
void *PTH_HIST(void *hist_numbers_arr)
{
    calc_hist_numbers_arr_t *arg = (calc_hist_numbers_arr_t *)hist_numbers_arr;
    int i;

    // Alloca espaço para o histograma local (Array)
    int *local_histogram = (int *)calloc(arg->n_bins, sizeof(*local_histogram));

    // Calcula o histograma para o bloco de dados
    for (float *pos = arg->numbers_arr_start; pos != arg->numbers_arr_end; ++pos)
    {
        int bin_index = (*pos - arg->min) / arg->interval;
        local_histogram[bin_index] += 1;
    }

    /************************ INÍCIO DA ÁREA CRÍTICA ************************/

    sem_wait(&semaphore);

    // Garante a saída da thread e passa o histograma local ao pthread_join
    /* pthread_exit(local_histogram); */
    pthread_join(threads[arg->thread_id], (void **)&local_histogram);
    for (i = 0; i < (arg->n_bins); ++i)
    {
        histogram[i] += local_histogram[i];
    }

    sem_post(&semaphore);

    /************************** FIM DA ÁREA CRÍTICA **************************/
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    // Seed dinâmica para o número de Bins
    srand((unsigned int)48 /* time(NULL) */);

    /*
     * Valores próximos a média são mais prováveis
     * O desvio padrão afeta a dispersão de valores gerados pela média
     */
    random_device rd{};
    mt19937 gen{rd()};

    float min, max;          // Mínimo e Máximo do intervalo
    int n_numbers;           // Tamanho da sequência de números aleatórios.
    int n_bins;              // Número de bins no histograma
    int n_threads;           // Número de threads
    float interval;          // Intervalo entre cada Bin do histograma
    int local_n_numbers;     // Bloco para cada Thread
    int left_over_n_numbers; // Tamanho extra que será divido entre as threads depois
    float *numbers_arr;      // Sequencia de números gerados randomicamente no range [min, max]
    float *chunk_start;      // Início do bloco para a iteração
    int mean;                // Média utilizada na distribuição gaussiana
    int standard_deviation;  // Desvio padrão utilizado na distribuição gaussiana
    int i, j;                // Variáveis auxiliares

    // Tratamento para entradas insuficiente
    if (argc != 3)
    {
        fprintf(stderr, "Modelo de parâmetros: <n_threads> <n_numbers>\n", argv[0]);
        return -1;
    }

    // Atribuição dos parâmetros
    n_threads = atoi(argv[1]);
    min = 1200.00;
    max = 2000.00;
    n_numbers = atoi(argv[2]);
    mean = 3232 / 2;
    standard_deviation = 72;

    normal_distribution<> normal_dist{mean, standard_deviation};

    n_bins = rand() % 1000 + 1;

    // Definição do intervalo entre cada Bin
    interval = (max - min) / (float)n_bins;

    // Definição do tamanho do bloco para cada thread
    local_n_numbers = n_numbers / n_threads;

    // Resto da divisão que será processado depois
    left_over_n_numbers = n_numbers % n_threads;

    // Alocação de espaço para os números a serem gerados no range passado por argv
    numbers_arr = (float *)malloc(n_numbers * sizeof(*numbers_arr));

    // Gera a sequencia de números randômicos
    for (i = 0; i < n_numbers; ++i)
    {
        numbers_arr[i] = round(normal_dist(gen));
    }

    // Inicialização do semáforo. Apenas 1 thread acessará a região crítica por vez
    sem_init(&semaphore, 0, 1);

    // Vetor de threads de tamanho passado por argv
    threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));

    // Inicializa o espaço referente aos dados base para cada bloco
    calc_hist_numbers_arr_t *args = (calc_hist_numbers_arr_t *)calloc(n_threads, sizeof(*args));

    // Alocação do espaço referente a todos os histogramas locais
    histogram = (int *)calloc(n_bins, sizeof(*histogram));

    // Inicio do bloco para iteração
    chunk_start = numbers_arr;

    for (i = 0; i < n_threads; ++i)
    {
        /*
         * Adciona "um elemento" de "left_over_n_numbers" ao tamanho dos
         * blocos das threads de maneira decremental, até que este valor 
         * se esgote. Note que "left_over_n_numbers" jamais será maior ou
         * igual a "local_n_numbers". 
         */
        int chunk_start_length = local_n_numbers + !!left_over_n_numbers;

        // Atribuição do bloco de sequência para a thread processar
        args[i] = (calc_hist_numbers_arr_t){
            .numbers_arr_start = chunk_start,
            .numbers_arr_end = chunk_start + chunk_start_length,
            .min = min,
            .max = max,
            .n_bins = n_bins,
            .interval = interval,
            .thread_id = i,
        };

        // Avança o início do bloco para a próxima iteração
        chunk_start += chunk_start_length;

        if (left_over_n_numbers)
            --left_over_n_numbers;

        // Instancia a thread para calcular o histograma deste bloco
        pthread_create(&threads[i], NULL, PTH_HIST,
                       (void *)&args[i]);
    }

    // Adeus semáforo :c
    sem_destroy(&semaphore);

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
            "\nIntervalo da sequencia: [%.2f, %.2f]\nTamanho da sequencia: %d\nNumero de "
            "Bins: %d\n"
            "Intervalo: %.2f\n"
            "Media da distribuicao: %d\n"
            "Desvio padrao da distribuicao: %d\n\n",
            min, max, n_numbers, n_bins, interval, mean, standard_deviation);
        for (i = 0; i < n_bins; ++i)
        {
            float bin_index = (float)i * interval + min;
            printf("[%.2f, %.2f) |", bin_index, bin_index + interval);
            int row_width = ((float)histogram[i] / (float)max_n_bins) * 40.f;
            for (j = 0; j < row_width; ++j)
            {
                printf("#");
            }
            printf(" %d\n", histogram[i]);
        }
        fflush(stdout);
    }

    // Impressão de resultados em arquivo
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./paralelo/tempo_pth_hist.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp,
                "\tTempo: %1.2e \tIntervalo: [%.2f, %.2f] \tTamanho da sequencia: %d \tNumero de "
                "Bins: %d \t"
                "Intervalo: %.2f \t"
                "Media da distribuicao: %d \t"
                "Desvio padrao da distribuicao: %d\n",
                ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
                min, max, n_numbers, n_bins, interval, mean, standard_deviation);
        fprintf(fp, "\t[");
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
        fprintf(fp, "]\n");
        fclose(fp);
    }
    // Liberação do espaço utilizado
    free(numbers_arr);
    free(args);
    free(histogram);
    free(threads);

    // Fim :)
    pthread_exit(NULL);
}