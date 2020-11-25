#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <random>

using namespace std;

typedef struct calc_hist_numbers_arr_t
{
    float *numbers_arr_start; // Início do intervalo de números
    float *numbers_arr_end;   // Início do intervalo de números
    int min, max;             // Mínimo e Máximo do intervalo
    int n_bins;               // Número de bins no histograma
    float interval;           // Passo de um bin para outro
} calc_hist_numbers_arr_t;

// Calcula um histograma com base em um bloco de números
void *HIST(void *hist_numbers_arr)
{
    calc_hist_numbers_arr_t *arg = (calc_hist_numbers_arr_t *)hist_numbers_arr;

    // Alloca espaço para o histograma local (Array)
    int *local_histogram = (int *)calloc(arg->n_bins, sizeof(*local_histogram));

    // Calcula o histograma para o bloco de dados
    for (float *pos = arg->numbers_arr_start; pos != arg->numbers_arr_end; ++pos)
    {
        int bin_index = (*pos - arg->min) / arg->interval;
        local_histogram[bin_index] += 1;
    }

    // Garante a saída da thread e passa o histograma local ao pthread_join
    pthread_exit(local_histogram);
}

int main(int argc, char *argv[])
{
    // Seed dinâmica
    /* srand((unsigned int)time(NULL)); */

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
    int *histogram;          // A soma de todos os histogramas locais

    // Tratamento para entradas insuficiente
    if (argc != 6)
    {
        fprintf(stderr, "Modelo de parâmetros: <n_threads> <min> <max> <n_numbers> <n_bins>\n", argv[0]);
        return -1;
    }

    // Atribuição dos parâmetros
    n_threads = atoi(argv[1]);
    min = atof(argv[2]);
    max = atof(argv[3]);
    n_numbers = atoi(argv[4]);
    n_bins = atoi(argv[5]);

    normal_distribution<> normal_dist{(max / 2), 100};

    // Definição do intervalo entre cada Bin
    interval = (max - min) / (float)n_bins;

    // Definição do tamanho do bloco para cada thread
    local_n_numbers = n_numbers / n_threads;

    // Resto da divisão que será processado depois
    left_over_n_numbers = n_numbers % n_threads;

    // Alocação de espaço para os números a serem gerados no range passado por argv
    numbers_arr = (float *)malloc(n_numbers * sizeof(*numbers_arr));

    // Gera a sequencia de números randômicos
    for (int i = 0; i < n_numbers; ++i)
    {
        numbers_arr[i] = round(normal_dist(gen)); /* (float)rand() / (float)RAND_MAX * (max - min) + min; */
    }

    // Vetor de threads de tamanho passado por argv
    pthread_t threads[n_threads];

    // Inicializa o espaço referente aos dados base para cada bloco
    calc_hist_numbers_arr_t *args = (calc_hist_numbers_arr_t *)calloc(n_threads, sizeof(*args));

    // Alocação do espaço referente a todos os histogramas locais
    histogram = (int *)calloc(n_bins, sizeof(*histogram));

    // Inicio do bloco para iteração
    chunk_start = numbers_arr;

    for (int i = 0; i < n_threads; ++i)
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
        };

        // Avança o início do bloco para a próxima iteração
        chunk_start += chunk_start_length;

        if (left_over_n_numbers)
            --left_over_n_numbers;

        // Instancia a thread para calcular o histograma deste bloco
        pthread_create(&threads[i], NULL, HIST,
                       (void *)&args[i]);
    }

    // Concatenação de todos os histogramas locais
    for (long i = 0; i < n_threads; ++i)
    {
        int *local_histogram = NULL;
        /*
         * Aguarda a conclusão do job para a thread "i" e soma o histograma local
         * ao histograma global 
         */
        pthread_join(threads[i], (void **)&local_histogram);
        for (int i = 0; i < n_bins; ++i)
        {
            histogram[i] += local_histogram[i];
        }
        free(local_histogram);
    }

    // Impressão do histograma
    {
        int max_n_bins = 0;

        // Busca Bin máximo
        for (int i = 0; i < n_bins; ++i)
        {
            if (histogram[i] > max_n_bins)
                max_n_bins = histogram[i];
        }

        // Imprime o array histograma
        printf(
            "\nIntervalo da sequencia: [%.2f, %.2f]\nTamanho da sequencia: %d\nNumero de "
            "Bins: %d\n"
            "Intervalo: %.2f\n\n",
            min, max, n_numbers, n_bins, interval);
        for (int i = 0; i < n_bins; ++i)
        {
            float bin_index = (float)i * interval + min;
            printf("[%.2f, %.2f) |", bin_index, bin_index + interval);
            int row_width = ((float)histogram[i] / (float)max_n_bins) * 40.f;
            for (int j = 0; j < row_width; ++j)
            {
                printf("#");
            }
            printf(" %d\n", histogram[i]);
        }
        fflush(stdout);
    }

    // Liberação do espaço utilizado
    free(numbers_arr);
    free(args);
    free(histogram);

    // Fim !!!
    pthread_exit(NULL);
}