#include <iostream>
#include <sys/time.h>
#include <fstream>

using namespace std;

// função para imprimir as matrizes
void imprime_matriz(int tam, int **mat){
	int i, j;
	
	// loop para imprimir
	for (i = 0; i < tam; i++) {
		printf("\n\t| ");
		for (j = 0; j < tam; j++){
			printf("%4d", mat[i][j]);
		}
		printf("    |");
	}
	printf("\n\n");
}

// Aloca memória a uma matriz e atribui o start_value a todas as posições 
int **genMatrix(int tam, int start_value) {
	int i,j,k;
	int *valores;
    int **matrix;

	// alocando memória
	valores = (int *)malloc(tam * tam * sizeof(int));
	matrix = (int **)malloc(tam * sizeof(int*));

	for (i = 0; i < tam; i++){
		matrix[i] = &(valores[i * tam]);
	}

    // populando matrix com valor 1
	for(j=0; j<tam; j++){
		for(k=0; k<tam; k++){
			matrix[j][k] = start_value;
		}
	}

    return matrix;
}

void MULTZ(int tam, int **fator_a, int **fator_b, int **produto){
	int i, j, k;
	int inicio, final;

	inicio = 0;
	final = tam;

	// multiplicação
	printf("Inicio %d => Final %d \n", inicio, final );
	for (i = inicio; i < final; i++)
	{  
		for (j = 0; j < tam; j++)
		{
			/* produto[i][j] = 0; */
			for ( k = 0; k < tam; k++){
				produto[i][j] += fator_a[i][k] * fator_b[k][j];
			}
		}
	}
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    // Definição das matrizes fatores e produto
    int **fator_a, **fator_b, **produto;

    // definindo tamanho da matriz
    int tam;

	// definindo o tamanho da matriz
	tam = atoi(argv[1]);

	// alocando matrizes
	fator_a = genMatrix(tam, 1);
	fator_b = genMatrix(tam, 1);
	produto = genMatrix(tam, 0);

    MULTZ(tam, fator_a, fator_b, produto);

	// imprime as matrizes 
	imprime_matriz(tam, fator_a);
	imprime_matriz(tam, fator_b);

	//imprime resultado
	imprime_matriz(tam, produto);

    gettimeofday(&stop, 0);

    FILE *fp;
    char outputFilename[] = "./serial/tempo_multz.txt";

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