#include<bits/stdc++.h>
#include <sys/time.h> 
using namespace std; 
  
// Ordena a Lista usando Odd and Even Sort 
void oddEvenSort(int *list, int n) 
{ 
    bool isSorted = false; // Flag que indica se a lista está ordenada
  
    while (!isSorted) 
    { 
        isSorted = true; 
  
        // Bubble sort nos elementos começando com index ímpar (Odd)
        for (size_t i = 1; i <= n-2; i = i+2) 
        { 
            if (list[i] > list[i+1]) 
             { 
                swap(list[i], list[i+1]); 
                isSorted = false; 
              } 
        } 
  
        // Bubble sort nos elementos começando com index par (Even)
        for (size_t i = 0; i<=n-2; i=i+2) 
        { 
            if (list[i] > list[i+1]) 
            { 
                swap(list[i], list[i+1]); 
                isSorted = false; 
            } 
        } 
    } 
  
    return; 
} 

void genList(int *list, int tamanho) {

    srandom(42);
    for (size_t i = 0; i < tamanho; i++)
    {   
        list[i] = random() % 100 ;
    }
} 
  
// Imprime uma lista de tamanho n
void printList(int *list, int tamanho) 
{ 
    for (size_t i = 0; i < tamanho; i++)
    {
       cout << list[i] << " "; 
    }
   cout << "\n"; 
} 
  
int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int n = atoi(argv[1]); // Tamanho da lista
    int *list = (int*) malloc(n * sizeof(int)); // Aloca o tamanho da lista

    // Gera lista
    genList(list, n);
    
    // Printa a lista desordenada
    printList(list, n); 
    
    oddEvenSort(list, n); 
    
    // Printa a lista ordenada
    printList(list, n); 

    gettimeofday(&stop, 0);
    /* FILE *fp;
    char outputFilename[] = "./trapezio/tempo_trapezio.txt";

    fp = fopen(outputFilename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
        exit(1);
    }

    fprintf(fp, "\tTempo: %1.2e \tLista ordenada: %f\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)), area_total);

    fclose(fp); */

    free(list);

    return 0;
}