#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <sys/time.h>
#include <random>

typedef std::numeric_limits<double> dbl;

using namespace std;

double calcPi(int termos)
{
    // Gerador Mersene twist, SEED: 42
    mt19937 mt(42);
    // Numero real pseudo-aleatorio
    uniform_real_distribution<double> linear_r(0.f, 1.f);

    int pi = 0;
    for (int i = 0; i < termos; i++)
    {

        double x = linear_r(mt);
        double y = linear_r(mt);

        if (x * x + y * y < 1.0)
        {
            pi++;
        }
    }
    return (double)(4.0 * pi / termos);
}

int main(int argc, char *argv[])
{
    struct timeval start, stop;

    gettimeofday(&start, 0);

    double pi = calcPi(atoi(argv[1])); //atoi: int atol:long atoll: long long

    gettimeofday(&stop, 0);

    FILE *fp;
    char outputFilename[] = "./pi/tempo_pi.txt";

    fp = fopen(outputFilename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
        exit(1);
    }

    fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n", ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)), pi);

    fclose(fp);

    return 0;
}