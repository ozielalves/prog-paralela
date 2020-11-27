#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <random>
#include <sys/time.h>

using namespace std;

int *histogram; // O histograma

typedef struct histogram_data
{
    float *numbers_arr_start; // Início do intervalo de números
    float *numbers_arr_end;   // Início do intervalo de números
    int min, max;             // Mínimo e Máximo do intervalo
    int n_bins;               // Número de bins no histograma
    float interval;           // Passo de um bin para outro
} histogram_data;

// Calcula um histograma com base em um bloco de números
void HIST(histogram_data arg)
{
    for (float *pos = arg.numbers_arr_start; pos != arg.numbers_arr_end; ++pos)
    {
        int bin_index = (*pos - arg.min) / arg.interval;
        histogram[bin_index] += 1;
    }
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

    float min, max;         // Mínimo e Máximo do intervalo
    int n_numbers;          // Tamanho da sequência de números aleatórios.
    int n_bins;             // Número de bins no histograma
    float interval;         // Intervalo entre cada Bin do histograma
    float *numbers_arr;     // Sequencia de números gerados randomicamente no range [min, max]
    float *chunk_start;     // Início do bloco para a iteração
    int mean;               // Média utilizada na distribuição gaussiana
    int standard_deviation; // Desvio padrão utilizado na distribuição gaussiana
    int i, j;               // Variáveis auxiliares

    // Tratamento para entradas insuficiente
    if (argc != 2)
    {
        fprintf(stderr, "Modelo de parâmetros: <n_numbers>\n", argv[0]);
        return -1;
    }

    // Atribuição dos parâmetros
    min = 1200.00;
    max = 2000.00;
    n_numbers = atoi(argv[1]);
    mean = 3232 / 2;
    standard_deviation = 72;

    normal_distribution<> normal_dist{mean, standard_deviation};

    n_bins = rand() % 1000 + 1;

    // Definição do intervalo entre cada Bin
    interval = (max - min) / (float)n_bins;

    // Alocação de espaço para os números a serem gerados no range passado por argv
    numbers_arr = (float *)malloc(n_numbers * sizeof(*numbers_arr));

    // Gera a sequencia de números randômicos
    for (i = 0; i < n_numbers; ++i)
    {
        numbers_arr[i] = round(normal_dist(gen));
    }

    // Alocação do espaço referente a todos os histogramas locais
    histogram = (int *)calloc(n_bins, sizeof(*histogram));

    // Inicio do bloco para iteração
    chunk_start = numbers_arr;

    int chunk_start_length = n_numbers;

    // Atribuição do bloco de sequência para a thread processar
    histogram_data arg = (histogram_data){
        .numbers_arr_start = chunk_start,
        .numbers_arr_end = chunk_start + chunk_start_length,
        .min = min,
        .max = max,
        .n_bins = n_bins,
        .interval = interval,
    };
    printf("arg.array_start: %d", arg.numbers_arr_start);

    HIST(arg);

    // Impressão do histograma em tela
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
        char outputFilename[] = "./serial/tempo_hist.txt";

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
            fprintf(fp, "[%.2f, %.2f),", bin_index, bin_index + interval);
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
    free(histogram);
}