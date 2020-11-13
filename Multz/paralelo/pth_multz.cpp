#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// Definição das matrizes globais
long **fator_a, **fator_b, **produto;

// Declaração do tamanho da matriz e do número de threads
int size, num_threads;

// Aloca memória para uma matriz sizeXsize
long ** genMatrix( int size )
{
  /* Allocate 'size' * 'size' longs contiguously. */
  long * vals = (long *) malloc( size * size * sizeof(long) );

  /* Allocate array of long* with size 'size' */
  long ** ptrs = (long **) malloc( size * sizeof(long*) );

  int i;
  for (i = 0; i < size; ++i) {
    ptrs[ i ] = &vals[ i * size ];
  }

  return ptrs;
}

// Inicialização de uma matriz sizeXsize com valores semi-randomicos
void initMatrix( long **matrix, int size, int seed )
{
  int i, j;
  srandom(seed);

  for (i = 0; i < size; ++i) {
    for (j = 0; j < size; ++j) {
      matrix[ i ][ j ] = rand() % 10;
    }
  }
}

// Imprime uma matriz
void printMatrix( long **mat, int tam )
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

/**
 * Thread routine.
 * Each thread works on a portion of the 'fator_a'.
 * The start and end of the portion depend on the 'arg' which
 * is the ID assigned to threads sequentially. 
 */
void * PTH_MULTZ( void *arg )
{
  int i, j, k, tid, portion_size, row_start, row_end;
  long sum;
  
  tid = *(int *)(arg); // get the thread ID assigned sequentially.
  portion_size = size / num_threads;
  row_start = tid * portion_size;
  row_end = (tid+1) * portion_size;

  for (i = row_start; i < row_end; ++i) { // hold row index of 'fator_a'
    for (j = 0; j < size; ++j) { // hold column index of 'fator_b'
      sum = 0; // hold value of a cell
      /* one pass to sum the multiplications of corresponding cells
	 in the row vector and column vector. */
      for (k = 0; k < size; ++k) { 
	sum += fator_a[ i ][ k ] * fator_b[ k ][ j ];
      }
      produto[ i ][ j ] = sum;
    }
  }
}

int main( int argc, char *argv[] )
{
  struct timeval start, stop;
	gettimeofday(&start, 0);

  int i;
  long sum = 0;
  struct timeval tstart, tend;
  double exectime;
  pthread_t * threads;

  // Tratamento para nenhuma enrada de dado 
  if (argc != 3) {
    fprintf( stderr, "%s <numero de threads> <tamanho da matriz>\n", argv[0], argv[1] );
    return -1;
  }

  num_threads = atoi( argv[1] );
  size = atoi( argv[2] );

  // Tratamento para entrada de valores que gere uma divisão de problam exata
  if ( size % num_threads != 0 ) {
    fprintf( stderr, "tamanho %d deve ser um multiplo do numero de threads %d\n",
	     size, num_threads );
    return -1;
  }

  threads = (pthread_t *) malloc( num_threads * sizeof(pthread_t) );

  // Alocação de memória para as matrizes
  fator_a = genMatrix( size );
  fator_b = genMatrix( size );
  produto = genMatrix( size );
  
  // Inicialização das matrizes produto
  initMatrix( fator_a, size, 48 );
  initMatrix( fator_b, size, 48 );

  // Tratamento para uma impressão limpa em tela
  if ( size <= 48 ) {
    printf( "Matrix 1:\n" );
    printMatrix( fator_a, size );
    printf( "Matrix 2:\n" );
    printMatrix( fator_b, size );
  }

  // Início da multiplicação
  gettimeofday( &tstart, NULL );
  for ( i = 0; i < num_threads; ++i ) {
    int *tid;
    tid = (int *) malloc( sizeof(int) );
    *tid = i;
    pthread_create( &threads[i], NULL, PTH_MULTZ, (void *)tid );
  }

  for ( i = 0; i < num_threads; ++i ) {
    pthread_join( threads[i], NULL );
  }
  gettimeofday( &tend, NULL );
  
  if ( size <= 48 ) {
    printf( "Resultado:\n" );
    printMatrix( produto, size );
  }

  exectime = (tend.tv_sec - tstart.tv_sec) * 1000.0; // sec to ms
  exectime += (tend.tv_usec - tstart.tv_usec) / 1000.0; // us to ms   

  /* printf( "Number of MPI ranks: 0\tNumber of threads: %d\tExecution time:%.3lf sec\n",
          num_threads, exectime/1000.0); */

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

  // Liberação de memória contínua
  free(fator_a[0]);
  free(fator_b[0]);
  free(produto[0]);
  
  // Liberação de ponteiro para "vetor" contínuo de valores
  free( fator_a );
  free( fator_b );
  free( produto );

  return 0;
}