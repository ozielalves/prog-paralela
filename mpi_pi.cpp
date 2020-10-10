#include <iostream>
#include "mpi.h"

using namespace std;

void getData(double* termos_ptr, int my_rank) {

    if (my_rank == 0) {
       cout << "Entre com o numero de pontos\n";
       cin >> termos_ptr;
    }

    MPI_Bcast(termos_ptr, 1, MPI_double, 0, MPI_COMM_WORLD);
}

/* long double calcPi(long long int termos, unsigned inicial) {
    long double sparc = 0;
    long long int aux = 1;
    for (long long int i = inicial; i < inicial + termos; i++) {
        if (i % 2 != 0) sparc += 4 / aux;
        else sparc -= 4 / aux;
        aux += 2;
    }
    return sparc;
} */

long double calcPi(long long int termos, unsigned inicial) {

    double sparc = 0;
    for(int i = inicial; i < inicial + termos; i++) {
        double x = rand();
        double y = rand();

        if(x * x + y * y < 1.0){
            sparc++;
        }
    }
    return (double) (4.0 * sparc / termos);
}


main(int argc, char** argv) {
    int         my_rank;         
    int         p;  
    int         termos;
    int         termos_local;
    int         inicial_local;
    long double pi_parc;
    long double pi;


    int         source;  
    MPI_Status  status;

    MPI_Init(&argc, &argv);

    // Rank do meu processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Descobre quantos processos estao em uso
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    getData(&termos, my_rank);

    // Divisao interna
    termos_local = termos/p;

    inicial_local = 1 + my_rank * termos_local;
    pi_parc = calcPi(termos_local, inicial_local);

    // Junta a integral calculada por cada processo
    MPI_Reduce(&pi_parc, &pi, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        cout << "Com " << termos << " termos, o Pi estimado:\n";
        cout <<  pi;
    }

    MPI_Finalize();
}