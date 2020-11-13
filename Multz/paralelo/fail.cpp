#include <iostream>
#include <stdio.h>
#include <cmath>
#include <sys/time.h>
#include <cstring>
#include <pthread.h>

using namespace std;

// Definição das matrizes globais
int **fator_a, **fator_b; 
long long **produto;

// Declaração do tamanho da matriz e do número de threads
int tam, num_thread;

// Aloca memória a uma matriz e atribui o valores às posições semi-randomicamente
void genMatrix(int seed)
{
	int i, j, k;
	srandom(seed);

	if (seed != 0)
	{
		int *valores;

		// Alocação de memória
		valores = (int *)malloc(tam * tam * sizeof(int));
		fator_a = (int **)malloc(tam * sizeof(int *));

		for (i = 0; i < tam; i++)
		{
			fator_a[i] = &(valores[i * tam]);
		}

		// Populando a matriz
		for (j = 0; j < tam; j++)
		{
			for (k = 0; k < tam; k++)
			{
				fator_a[j][k] = rand() % 10;
			}
		}
	}
	else
	{
		long long *valores;

		// Alocação de memória
		valores = (long long *)calloc(tam, tam * tam * sizeof(long long));
		produto = (long long **)calloc(tam, tam * sizeof(long long *));

		for (i = 0; i < tam; i++)
		{
			produto[i] = &(valores[i * tam]);
		}

	}

}

// Imprime uma matriz
template <typename T>
void printMatrix(T **mat)
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

// Libera o espaço armazenado para as matrizes
void freeMatrix()
{
	free(fator_a[0]);
	free(produto[0]);
	free(fator_a);
	free(produto);
}

void *PTH_MULTZ(void *rank)
{
	int i, j, k;		// Variáveis auxiliáres
	long thread_id; // ID da thread atual
	int passo;			// Passo da multiplicação
	int inicio;			// Início do intervalo
	int final;			// Fim do intervalo

	thread_id = (long)rank;
	passo = tam / num_thread;
	inicio = thread_id * passo;
	final = (thread_id + 1) * passo - 1;
	
	if(final > tam){
		final = tam -1;
	}

	/* printf("\nPara Thread %ld: %d\n", thread_id, passo); */

	// Início da multiplicação
	/* 	printf("Inicio %d - Final %d \n", inicio, final); */

	for (i = inicio; i <= final; i++)
	{
		for (j = 0; j < tam; j++)
		{
			/* produto[i][j] = 0; */
			for (k = 0; k < tam; k++)
			{
				produto[i][j] += fator_a[i][k] * fator_b[k][j];
			}
		}
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	struct timeval start, stop;
	gettimeofday(&start, 0);
	
	int i, j; // Variáveis atuxiliares

	// Threads
	pthread_t *threads;

	// Definição do número de processos
	num_thread = atoi(argv[1]);

	// Definição do tamanho da matriz
	tam = atoi(argv[2]);

	// Tratamento das entradas
	if (num_thread > tam)
	{
		printf("O número de threads é maior que o tamanho da matriz. Por favor selecione um número menor de threads.\n");
		return 0;
	}
	else if (tam % num_thread != 0)
	{
		printf("Tamanho de problema não divisível pelo número de threads.\n");
		return 0;
	}
	else
	{
		/* Nada */
	}

	// Alocando e declarando a matriz fator_a
	genMatrix(42);

	// Uso do método memcpy para fazer uma cópia do fator_a no fator_b
	memcpy(&fator_b, &fator_a, sizeof(fator_a));

	// Alocando espaço dinamicamente para o produto da multiplicação (0 em todas as posições)
	genMatrix(0);

	// Alocação dinâmica de memória para threads
	threads = (pthread_t *)malloc(num_thread * sizeof(pthread_t));

	// Criação das threads e realização das parcelas de multiplicação em cada processo
	for (i = 0; i < num_thread; i++)
	{
		pthread_create(&threads[i], NULL, PTH_MULTZ, (void *)(i));
	}
	// Agrupa e espera até que todos os processos terminem de executar
	for (j = 0; j < num_thread; j++)
	{
		pthread_join(threads[i], NULL);
	}

	// Imprime as duas matrizes fatores

/* 	printMatrix(fator_a); */
/* 	printMatrix(fator_b); */

	// Imprime a matriz produto
/* 	printMatrix(produto); */

	// libera as memórias alocadas
	freeMatrix();
	free(threads);

	gettimeofday(&stop, 0);

	FILE *fp;
	char outputFilename[] = "./paralelo/tempo_pth_multz.txt";

	fp = fopen(outputFilename, "a");
	if (fp == NULL)
	{
		fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
		exit(1);
	}

	fprintf(fp, "\tTempo: %1.2e\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)));

	fclose(fp);
	
	return 0;
}