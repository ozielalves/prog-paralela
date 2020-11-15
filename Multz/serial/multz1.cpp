#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <sys/time.h>
#include <fstream>

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

// Multiplica duas matrizes contemplando o princípio da localidade
void MULTZ(int size, int **fator_a, int **fator_b, int **produto)
{
    int i, j, k;            // Variáveis auxiliares
    int row_start, row_end; // Inicio e fim da linha da matriz
    int sum;                // Armazena o produto da multiplicação

    row_start = 0;
    row_end = size;

    // Para cada linha na matriz "fator_a"
    for (i = row_start; i < row_end; i++)
    {
        // Para cada coluna na matriz "fator_b"
        for (j = 0; j < size; j++)
        {
            sum = 0;
            for (k = 0; k < size; k++)
            {
                sum += fator_a[i][k] * fator_b[k][j];
            }
            produto[i][j] = sum;
        }
    }
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int **fator_a, **fator_b, **produto; // Matrizes
    int size;                            // Tamanho das matrizes

    // Definindo o tamanho da matriz
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
    char outputFilename[] = "./serial/tempo_multz1.txt";

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