#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <random>

typedef std::numeric_limits<double> dbl;

using namespace std;

double calcPi(int termos) {
    /* double pi = 0;
    long long int aux = 1;
    for (long long int i = 1; i <= termos; i++) {
        if (i % 2 != 0) pi += 4 / aux;
        else pi -= 4 / aux;
        aux += 2;
    }
    return pi; */
    // Gerador Mersene twist, SEED: 42
    mt19937 mt(42);
    // Numero real pseudo-aleatorio
    uniform_real_distribution<double> linear_r(0.f, 1.f);

    int pi = 0;
    for(int i = 0; i < termos; i++) {
        
        double x = linear_r(mt);
        double y = linear_r(mt);

/*         cout << "["<< y << ", " << x << "] "; */

        if(x * x + y * y < 1.0){
            pi++;
        }
    }
    return (double) (4.0 * pi / termos);
}

int main(int argc, char* argv[]) {

    double pi = calcPi(5000000);
    
    cout.precision(dbl::max_digits10);
    cout << "\nO valor de Pi: " << pi << "\n";
    
    return 0;
}