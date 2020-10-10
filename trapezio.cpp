// Programação Paralela.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// MPI - Compilar: mpicc -g -Wall -o <executavel> <compilável>.cpp 
// MPI - Executar: mpiexec -n <nº de processos> ./<executavel> // p=1 não é serial

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>

typedef std::numeric_limits< double > dbl;

using namespace std;

double f(double x) { 
    //return 4 / (1 + pow(x, 2)); // Primitiva de ArcTg
    return pow(x, 2);
};
/*
double trapezioIntegralV1(double xa, double xb, int n, double (*f)(double)) {
    double x_i; // Passo do X
    double area_total = 0.0f; // Soma das areas
    double inc; // Incremento

    inc = (xb - xa) / n;

    for (int i = 1; i < n; i++) {
        x_i = xa + i * inc;
        area_total += (f(x_i) + f(x_i + inc)) * inc / 2;
    }

    return area_total;
};
*/
double trapezioIntegral(double xa, double xb, long long int n, double (*f)(double)) {
    double x_i; // Passo do X
    double area_total = 0.0f; // Soma das areas
    double inc; // Incremento

    inc = (xb - xa) / n;
    area_total = (f(xa) + f(xb)) / 2;

    for (long long int i = 1; i < n; i++) {
        x_i = xa + i * inc;
        area_total += f(x_i);
    }

    area_total = inc * area_total;

    return area_total;
};



int main(int argc, char* argv[]) {
    
    double area_total = trapezioIntegral(0.f, 40.f, (1000000- 1) / 2, f);
    
    cout.precision(dbl::max_digits10);
    cout << "Area aproximada da figura: " << area_total << "\n";

    //system("PAUSE");
    return 0;
}
