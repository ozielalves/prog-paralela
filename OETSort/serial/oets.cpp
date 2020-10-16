int main(int argc, char *argv[])
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    /* double xa = 0.;
    double xb = 30.;

    double area_total = trapezioIntegral(xa, xb, atoll(argv[1])); */

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