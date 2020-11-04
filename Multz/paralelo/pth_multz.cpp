#include <iostream>
#include <cmath>
#include <pthread.h>

// Definição das matrizes globais
int **fator_a, **fator_b, **produto;

// Declaração do tamanho da matriz e do número de threads
int tam, num_thread;

// Aloca memória a uma matriz e atribui o start_value a todas as posições 
int **genMatrix(int start_value) {
	int i,j,k;
	int *valores;
  int **matrix;

	// Alocação de memória
	valores = (int *)malloc(tam * tam * sizeof(int));
	matrix = (int **)malloc(tam * sizeof(int*));

	for (i = 0; i < tam; i++){
		matrix[i] = &(valores[i * tam]);
	}

  // Populando a matriz
	for(j=0; j<tam; j++){
		for(k=0; k<tam; k++){
			matrix[j][k] = start_value;
		}
	}

    return matrix;
}

// Imprime uma matriz
void imprime_matriz(int **mat)
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
	int i, j, k;
	long thread_id = (long)rank;
	printf("Thread ID: %ld\n", thread_id);
	int inicio, final;

	// Pode ser global ?? float??
	float passo = ceil((float)tam / num_thread);
	inicio = thread_id * passo;
	final = (thread_id + 1) * passo - 1;

	// Necessário ??
	if (final > tam)
	{
		final = tam - 1;
	}

	// Início da multiplicação
	printf("Inicio %d => Final %d \n", inicio, final);
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
	int i, j;
	pthread_t *threads;

	// Definição do número de processos
	num_thread = atoi(argv[1]);

	// Definição do tamanho da matriz
	tam = atoi(argv[2]);

	// Tratamento para evitar que o número de threads
	// seja maior que o tam da matriz
	if (num_thread > tam)
	{
		printf("O número de threads é maior que o tamanho da matriz. Por favor selecione um número menor de threads.\n");
		return 0;
	}

	// Alocando e declarando matrizes
	fator_a = genMatrix(1);
	fator_b = genMatrix(1);
	produto = genMatrix(0);

	// Alocação dinâmica de memória para threads
	threads = (pthread_t *)malloc(num_thread * sizeof(pthread_t));

	// Criação das threads e realização das parcelas de multiplicação em cada processo
	for (i = 0; i < num_thread; i++)
	{
		int* arg;
		pthread_create(&threads[i], NULL, PTH_MULTZ, (void*)(arg));
	}
	// Agrupa e espera até que todos os processos terminem de executar
	for (i = 0; i < num_thread; i++)
	{
		pthread_join(threads[i], NULL);
	}

	// Imprime as duas matrizes fatores
	imprime_matriz(fator_a);
	imprime_matriz(fator_b);

	// Imprime a matriz produto
	imprime_matriz(produto);

	// Libera a memória alocada para threads
	free(threads);

	return 0;
}