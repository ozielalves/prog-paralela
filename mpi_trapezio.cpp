// Para compilar: mpicc -g -o mpi_trapezio mpi_trapezio.cpp -lstdc++ -lm

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include "mpi.h"

void getData(float* xa_ptr, float* xb_ptr, int* n_ptr, int my_rank) {

    if (my_rank == 0) {
        //printf("Entre com os pontos a, b, e n\n");
        scanf("%f %f %d", xa_ptr, xb_ptr, n_ptr);
    }
    MPI_Bcast(xa_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(xb_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

float trapezioIntegral(float local_a, float local_b, int local_n, float  inc) {

    float integral;
    float x;
    int i;

    float f(float x); // Função que vamos integrar

    integral = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    for (i = 1; i <= local_n - 1; i++) {
        x = x + inc;
        integral = integral + f(x);
    }
    integral = integral * inc;
    return integral;
}


float f(float x) { 
    return 4 / (1 + pow(x, 2)); // Primitiva de ArcTg
}

int main(int argc, char** argv) {
    int         my_rank;   // Rank do meu processo          
    int         p;         // Numero de processos   
    float       xa;        // X Início da figura             
    float       xb;        // X Fim da figura           
    int         n;         // Numero de mini trapezios      
    float       inc;       // Incremento (Base do Trapezio)
    float       local_a;   // X Início da figura LOCAL   
    float       local_b;   // X Fim da figura LOCAL
    int         local_n;   // Numero de mini trapezios LOCAL 
                                     
    float       area_relativa;  // Area relativa ao intervalo
    float       area_total;     // Area total
    //int         source;  
    //MPI_Status  status;

    MPI_Init(&argc, &argv);

    // Rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Quantos processos então sendo usados
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    getData(&xa, &xb, &n, my_rank);

    inc = (xb - xa) / n; // O incremento e n serão os mesmo para todos os processos
    local_n = n / p;  

    // O tamanho de cada intervalo de processo será (local_n * h) 
    local_a = xa + my_rank * local_n * inc;
    local_b = local_a + local_n * inc;
    area_relativa = trapezioIntegral(local_a, local_b, local_n, inc);

    // Soma as integrais calculadas por cada processo
    MPI_Reduce(&area_relativa, &area_total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    /* Print the result */
/*     if (my_rank == 0) {
        printf("Com n = %d mini trapezios, a estimativa\n",
            n);
        printf("para integral de %f para %f = %f\n",
            xa, xb, area_total);
    } */
    // Exportar tempo - TODO
    MPI_Finalize();

}
