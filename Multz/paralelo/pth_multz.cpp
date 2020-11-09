#include <iostream>
#include <cmath>
#include <cstring>
#include <pthread.h>

using namespace std;

// Definição das matrizes globais
int **fator_a, **fator_b, **produto;

// Declaração do tamanho da matriz e do número de threads
int tam, num_thread;

// Aloca memória a uma matriz e atribui o valores às posições semi-randomicamente
int **genMatrix(int seed)
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

// Imprime uma matriz
void printMatrix(int **mat)
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

void *PTH_MULTZ(void *rank)
{
	int i, j, k;		// Variáveis auxiliáres
	long thread_id; // ID da thread atual
	float passo;		// Passo da multiplicação
	int inicio;			// Início do intervalo
	int final;			// Fim do intervalo

	// Pode ser global ?? float??
	thread_id = (long)rank;
	passo = ceil((float)tam / num_thread);
	inicio = thread_id * passo;
	final = (thread_id + 1) * passo - 1;

	printf("Thread ID: %ld\n", thread_id);

	// Necessário ??
	/* 	if (final > tam)
	{
		final = tam - 1;
	} */

	// Início da multiplicação
	printf("Inicio %d - Final %d \n", inicio, final);

	for (i = inicio; i <= final; i++)
	{
		for (j = 0; j < tam; j++)
		{
			produto[i][j] = 0;
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
	int i, j;						// Variáveis atuxiliares
	pthread_t *threads; // Threads

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

	// Alocando e declarando matrizes
	fator_a = genMatrix(42);

	// Uso do método memcpy para fazer uma cópia do fator_a no fator_b
	memcpy(&fator_b, &fator_a, sizeof(fator_a));

	// Alocando espaço dinamicamente para o produto da multiplicação (0 em todas as posições)
	produto = genMatrix(0);

	// Alocação dinâmica de memória para threads
	threads = (pthread_t *)malloc(num_thread * sizeof(pthread_t));

	// Criação das threads e realização das parcelas de multiplicação em cada processo
	for (i = 0; i < num_thread; i++)
	{
		pthread_create(&threads[i], NULL, PTH_MULTZ, (void *)(i));
	}
	// Agrupa e espera até que todos os processos terminem de executar
	for (j = 0; i < num_thread; i++)
	{
		pthread_join(threads[i], NULL);
	}

	// Imprime as duas matrizes fatores
	printMatrix(fator_a);
	printMatrix(fator_b);

	// Imprime a matriz produto
	printMatrix(produto);

	// Liberção de memória alocada durante a execução
	free(threads);
	free(fator_a);
	free(produto);

	return 0;
}