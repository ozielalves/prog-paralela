#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

using namespace std;

int *histogram; // O histograma

typedef struct histogram_data
{
    unsigned long long n_numbers; // Quantidade de números a serem gerados
    int min, max;                 // Mínimo e Máximo do intervalo
    int n_bins;                   // Número de bins no histograma
    float interval;               // Passo de um bin para outro
    int mean;                     // Média utilizada na distribuição gaussiana
    int standard_deviation;       // Desvio padrão utilizado na distribuição gaussiana
} histogram_data;

/*
* Gera um número randômico de acordo com a distribuição normal
* Valores próximos a média são mais prováveis
* O desvio padrão afeta a dispersão de valores gerados pela média
*/
float gaussDistribution(int mean, int standard_deviation)
{

    float r, v1, v2, random_number;

    r = 2;
    while (r >= 1)
    {
        v1 = (2 * ((float)rand() / (float)RAND_MAX) - 1);
        v2 = (2 * ((float)rand() / (float)RAND_MAX) - 1);
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
void HIST(histogram_data arg)
{
    unsigned long long i; // Variável auxiliar
    float num;            // Número randômico gerado (Distribuição Gaussiana)
    int bin_index;        // Identificador de bin

    // Determina seed dinâmica para geração de números
    srand((unsigned int)(time(NULL)));

    // Calcula o histograma
    for (i = 0; i < arg.n_numbers; i++)
    {   
        num = gaussDistribution(arg.mean, arg.standard_deviation);
        bin_index = floor((num - arg.min) / arg.interval);
        histogram[bin_index] += 1;
    }
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;

    float min, max;               // Mínimo e Máximo do intervalo
    unsigned long long n_numbers; // Tamanho da sequência de números aleatórios.
    int n_bins;                   // Número de bins no histograma
    float interval;               // Intervalo entre cada Bin do histograma
    int mean;                     // Média utilizada na distribuição gaussiana
    int standard_deviation;       // Desvio padrão utilizado na distribuição gaussiana
    unsigned long long i, j;      // Variáveis auxiliares

    // Tratamento para entradas insuficiente
    if (argc != 2)
    {
        fprintf(stderr, "Modelo de parâmetros: <n_numbers>\n", argv[0]);
        return -1;
    }

    // Atribuição dos parâmetros
    min = 1500.00;
    max = 1700.00;
    n_numbers = strtoull(argv[1], NULL, 10);
    mean = 3232 / 2;
    standard_deviation = 72;

    // Seed fixa para o número de Bins
    n_bins = genBins((unsigned int)(time(NULL)));

    // Definição do intervalo entre cada Bin
    interval = (max - min) / (float)n_bins;

    gettimeofday(&start, 0);

    // Alocação do espaço referente a todos os histogramas locais
    histogram = (int *)calloc(n_bins, sizeof(int));

    // Atribuição dos dados para o cálculo do histograma
    histogram_data arg;
    arg.n_numbers = n_numbers;
    arg.mean = mean;
    arg.standard_deviation = standard_deviation;
    arg.min = min;
    arg.max = max;
    arg.n_bins = n_bins;
    arg.interval = interval;

    HIST(arg);

    gettimeofday(&stop, 0);

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
            "\nTempo: %1.2e\nIntervalo da sequencia: [%.2f, %.2f]\nTamanho da sequencia: %llu\nNumero de "
            "Bins: %d\n"
            "Intervalo: %.2f\n"
            "Media da distribuicao: %d\n"
            "Desvio padrao da distribuicao: %d\n\n",
            ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
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
        printf("[");
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
        char outputFilename[] = "./serial/tempo_hist.txt";

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
        /* fprintf(fp, "\t[");
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
        fprintf(fp, "]\n"); */
        fclose(fp);
    }

    free(histogram);
}