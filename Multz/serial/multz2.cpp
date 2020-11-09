#include <iostream>
#include <cstring>
#include <sys/time.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// Imprime uma matriz
void printMatrix(int tam, int **mat)
{
    int i, j;

    for (i = 0; i < tam; i++)
    {
        printf("\n\t| ");
        for (j = 0; j < tam; j++)
        {
            printf("%4d", mat[i][j]);
        }
        printf("    |");
    }
    printf("\n\n");
}

// Aloca memória a uma matriz e atribui o valores às posições semi-randomicamente
int **genMatrix(int tam, int seed)
{
    int i, j, k;
    int *valores;
    int **matrix;
    srandom(seed);

    // Alocação de memória
    valores = (int *)malloc(tam * tam * sizeof(int));
    matrix = (int **)malloc(tam * sizeof(int *));

    for (i = 0; i < tam; i++)
    {
        matrix[i] = &(valores[i * tam]);
    }

    if (seed != 0)
    {
        // Populando a matriz
        for (j = 0; j < tam; j++)
        {
            for (k = 0; k < tam; k++)
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

// Multiplica duas matrizes não contemplando o princípio da 
void MULTZ(int tam, int **fator_a, int **fator_b, int **produto)
{
    int i, j, k;
    vector<int> linhas(tam);
    vector<int> colunas(tam);
    
    // Populando vectors de linhas e colunas
    for (i=0; i<tam; i++)
    {
        linhas[i] = i;
        colunas[i] = i;
    } 

    // Aleatorizando as linhas e colunas
    random_shuffle(linhas.begin(), linhas.end());
    random_shuffle(colunas.begin(), colunas.end());    

    /* printf("Inicio %d => Final %d \n", inicio, final); */
    for (i = 0; i < tam; i++)
    {
        for (j = 0; j < tam; j++)
        {
            produto[linhas[i]][colunas[j]] = 0;
            for (k = 0; k < tam; k++)
            {
                produto[linhas[i]][colunas[j]] += fator_a[linhas[i]][k] * fator_b[k][colunas[j]];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int **fator_a, **fator_b, **produto; // Matrizes
    int tam;                             // Tamanho das matrizes

    // Definindo o tamanho da matriz
    tam = atoi(argv[1]);

    // Alocando espaço dinamicamente ao fator_a
    fator_a = genMatrix(tam, 42);

    // Uso do método memcpy para fazer uma cópia do fator_a no fator_b
    memcpy(&fator_b, &fator_a, sizeof(fator_a));

    produto = genMatrix(tam, 0);

    MULTZ(tam, fator_a, fator_b, produto);

    // Imprime as matrizes
    printMatrix(tam, fator_a);
    printMatrix(tam, fator_b);

    // Imprime resultado
    printMatrix(tam, produto);

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