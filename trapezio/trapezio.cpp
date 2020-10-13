#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

using namespace std;

double f(double x)
{
    return pow(x, 2);
};

double trapezioIntegral(double xa, double xb, long long int n)
{
    double x_i;             // Passo do X
    double area_total = 0.; // Soma das areas
    double inc;             // Incremento

    inc = (xb - xa) / n;
    area_total = (f(xa) + f(xb)) / 2;

    for (long long int i = 1; i < n; i++)
    {
        x_i = xa + i * inc;
        area_total += f(x_i);
    }

    area_total = inc * area_total;

    return area_total;
};

int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    double xa = 0.;
    double xb = 30.;

    double area_total = trapezioIntegral(xa, xb, atoll(argv[1]));

    gettimeofday(&stop, 0);
    FILE *fp;
    char outputFilename[] = "./trapezio/tempo_trapezio.txt";

    fp = fopen(outputFilename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
        exit(1);
    }

    fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)), area_total);

    fclose(fp);

    return 0;
}
