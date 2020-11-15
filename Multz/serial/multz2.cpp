#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <sys/time.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// Aloca memória a uma matriz e atribui o valores às posições semi-randomicamente
int **genMatrix(int size, int seed)
{
    int i, j, k;
    int *valores;
    int **matrix;
    srandom(seed);

    // Alocação de memória
    valores = (int *)malloc(size * size * sizeof(int));
    matrix = (int **)malloc(size * sizeof(int *));

    for (i = 0; i < size; i++)
    {
        matrix[i] = &(valores[i * size]);
    }

    if (seed != 0)
    {
        // Populando a matriz
        for (j = 0; j < size; j++)
        {
            for (k = 0; k < size; k++)
            {
                matrix[j][k] = rand() % 10;
            }
        }
    }
    else
    {
        return matrix;
    }

    return matrix;
}

// Imprime uma matriz
void printMatrix(int size, int **mat)
{
    int i, j;

    for (i = 0; i < size; i++)
    {
        printf("\n\t| ");
        for (j = 0; j < size; j++)
        {
            printf("%4d", mat[i][j]);
        }
        printf("    |");
    }
    printf("\n\n");
}

// Multiplica duas matrizes não contemplando o princípio da
void MULTZ(int size, int **fator_a, int **fator_b, int **produto)
{
    int i, j, k;                           // Variáveis auxiliares
    vector<int> rows(size), columns(size); // Vetor de Linhas e de Colunas da matriz
    int sum = 0;                           // Armazena o produto da multiplicação

    // Populando vectors de linhas e colunas
    for (i = 0; i < size; i++)
    {
        rows[i] = i;
        columns[i] = i;
    }

    // Aleatorizando as linhas e colunas
    random_shuffle(rows.begin(), rows.end());
    random_shuffle(columns.begin(), columns.end());

    // Para cada linha na matriz "fator_a"
    for (i = 0; i < size; i++)
    {
        // Para cada coluna na matriz "fator_b"
        for (j = 0; j < size; j++)
        {
            sum = 0;
            for (k = 0; k < size; k++)
            {
                sum += fator_a[rows[i]][k] * fator_b[k][columns[j]];
            }
            produto[rows[i]][columns[j]] = sum;
        }
    }
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int **fator_a, **fator_b, **produto; // Matrizes
    int size;                            // Tamanho das matrizes

    // Definindo o sizeanho da matriz
    size = atoi(argv[1]);

    // Alocando espaço dinamicamente ao fator_a
    fator_a = genMatrix(size, 42);

    // Uso do método memcpy para fazer uma cópia do fator_a no fator_b
    memcpy(&fator_b, &fator_a, sizeof(fator_a));

    produto = genMatrix(size, 0);

    MULTZ(size, fator_a, fator_b, produto);

    // Tratamento para uma impressão limpa em tela
    if (size <= 48)
    {
        // Imprime as matrizes
        printf("Matrix 1:\n");
        printMatrix(size, fator_a);
        printf("Matrix 2:\n");
        printMatrix(size, fator_b);
        printf("Resultado:\n");
        printMatrix(size, produto);
    }

    gettimeofday(&stop, 0);

    FILE *fp;
    char outputFilename[] = "./serial/tempo_multz2.txt";

    fp = fopen(outputFilename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
        exit(1);
    }

    fprintf(fp, "\tTempo: %1.2e\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)));

    fclose(fp);

    // Liberção de memória alocada durante a execução
    free(fator_a);
    free(produto);

    return 0;
}