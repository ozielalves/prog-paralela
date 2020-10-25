# Análise de Algoritmos Paralelos e Seriais
## Odd-Even Transposition Sort
Universidade Federal do Rio Grande do Norte ([UFRN](http://http://www.ufrn.br)), 2020.

#### Análise por:
- [Oziel Alves](https://github.com/ozielalves/)

## Sumário
+ [Introdução](#introdução)
  + [Objetivos](#objetivos)
  + [Dependências](#dependências)
    + [G++ Compiler](#g-compiler)
    + [MPI](#mpi---message-passing-interface)
  + [Compilação e Execução](#compilação-e-execução)
    + [Arquivo com Resultados](#arquivo-com-resultados)
    + [Condições de Testes](#condições-de-testes)
      + [Informações sobre a máquina utilizada](#informações-sobre-a-máquina-utilizada)
  + [Apresentação do Algoritmo](#apresentação-do-algoritmo)
    + [Odd-Even Transposition Sort](#odd-even-transposition-sort-1)
      + [Serial](#serial)
      + [Paralelo](#paralelo)
+ [Desenvolvimento](#desenvolvimento)      
  + [Corretude](#corretude)
  + [Gráficos](#gráficos)
  + [Serial e Paralelo - Tempo x Tamanho do Problema](#serial-e-paralelo---tempo-x-tamanho-do-problema)
  + [Tamanho do Problema - Tempo x Cores](tamanho-do-problema---tempo-x-cores)
  + [Análise de Speedup](#análise-de-speedup)
  + [Análise de Eficiência](#análise-de-eficiência)
+ [Conclusão](#conclusão)
  + [Considerações Finais](#considerações-finais)
  + [Softwares utilizados](#softwares-utilizados)

## Introdução

### Objetivos
Analisar e avaliar o comportamento, eficiência e speedup dos algoritmos em relação ao seu tempo de execução, tamanho do problema analisado e resultados obtidos. Os cenários irão simular a execução dos algoritmos para 1 (serial), 2, 4 e 8 cores, com 4 tamanhos de problema definidos empiricamente. Os limites de tamanhos foram estabelecidos com o objetivo de atingir o tempo mínimo de execução determinado pela [referência](https://github.com/ozielalves/prog-paralela/tree/master/referencia) desta Análise.

### Dependências
#### G++ Compiler
É necessário para a compilação dos programam, visto que são feitos em c++.
```bash
# Instalação no Ubuntu 20.04 LTS:
sudo apt-get install g++
```

#### MPI - Message Passing Interface
É necessário para a compilação e execução dos códigos paralelos.
```bash
# Instalação no Ubuntu 20.04 LTS:
sudo apt-get install -y mpi 
```
### Compilação e Execução
Instaladas as dependências, basta executar o shellcript determinado para a devida bateria de execuções na raiz do repositório:<br>
Serão realizadas **5 execuções** com **4 tamanhos de problema** , em **3 quantidades de cores** (2, 4 e 8).
```bash
# Para o algorítimo de ordenação serial
./OETS_serial_start.sh
```
```bash
# Para o algorítimo de ordenação paralelo
./OETS_paralelo_start.sh
```

**Obs.:** Caso seja necessário conceder permissão máxima para os scripts, execute `chmod 777 [NOME DO SCRIPT].sh`.

### Arquivo com Resultados 
Após o termino das execuções do script é possível ter acesso aos arquivos de tempo `.txt` na pasta `serial` ou `paralelo`, de acordo com o script executado. Os dados coletados foram utilizados para realização desta análise.

### Condições de Testes
#### Informações sobre a máquina utilizada
+ **Dell Inspiron 14-inc 7460**

+ **Processador**: Intel Core i7 7500U (até 3.5 GHz) Dual Core Cache 4M. (FSB)4 GT/s OPI (Integra HyperThreading para trabalhar com até 4 threads de uma vez)

+ **Número de Cores/Threads**: 2/4

+ **Memória**: 8 GB tipo DDR4 – 2133MHz

+ **Sistema**: Ubuntu 20.04.1 LTS


### Apresentação dos Algoritmos

#### Odd-Even Transposition Sort
O algoritmo Odd-Even Transposition ordena n elementos em `n` fases (n é par), cada fase requer `n / 2` operações de troca de comparação. Esse algoritmo alterna entre duas fases, **Odd** (ímpares) e **Even** (pares). Seja `[a1, a2, ..., an]` uma lista a ser ordenada. Durante a fase Odd, os elementos com índices ímpares são comparados com seus vizinhos direitos e, se estiverem fora da sequência, são trocados; assim, os pares `(a 1, a2), (a3, a 4), ... , (an-1, an)` são trocados por comparação (assumindo que n é par). Da mesma forma, durante a fase Even, os elementos com índices pares são comparados com seus vizinhos direitos, e se eles estiverem fora de sequência, eles são trocados; assim, os pares `(a2, a3), (a4, a5), ... , (an-2, an-1)` são trocados por comparação. Após `n` fases de trocas Odd-Even, a lista é ordenada. Cada fase do algoritmo (Odd ou Even) requer comparações **`Q(n)`**, e há um total de n fases; assim, a complexidade sequencial é **`Q(n²)`**.<br><br>
**Referência**: Introduction to Parallel Computing-Ananth Gramma (2nd Edition)


#### Serial
Dado um número `n` de elementos para criação de uma lista com inteiros randômicos, a seguinte sub-rotina é implementada: 

1. É alocada memória referente ao tamanho `n` da lista a ser ordenada em `list`.

2. Em seguida, a função `genList` se responsabiliza por popular `list` com números inteiros pseudoaleatórios.

3. Feito isto, a função `oddEvenSort` pode da inicio ao processo de ordenação.

A implementação da função `oddEvenSort` é apresentada abaixo:
```bash
void oddEvenSort(int *list, int n) 
{ 
    bool isSorted = false; # Flag que indica se a lista está ordenada
  
    while (!isSorted) 
    { 
        isSorted = true; 
  
        # Fase ímpar (Odd)
        for (size_t i = 1; i <= n-2; i = i+2) 
        { 
            if (list[i] > list[i+1]) 
             { 
                swap(list[i], list[i+1]); 
                isSorted = false; 
              } 
        } 
  
        # Fase par (Even)
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
```

#### Paralelo
Ainda chamando o numero total de pontos a serem definidos como `termos`, a seguinte sub-rotina é implementada:  

1. O tamanho do problema, `termos` é lido por por linha de comando.

2. É iniciada a comunicação paralela.

3. `termos_local` recebe `termos` dividido pela quantidade de processos.

4.  `termos_local` é passado como parametro para o cáculo parcial dos acertos, usando a função já apresentada, `calcPi`, e armazenado em cada processo como `acertos_parc`.

5. Ao termino da execução de cada processo , `acertos_parc` é somado a `acertos`.

6. Quando todos os processos finalizam a contagem de acertos, todos os acertos parciais são somados a `acertos`, então, é fechada a comunicação MPI e impresso o valor do resultado final multiplicado por 4 e dividido por `termos`.

**Obs.:** Vale salientar que, por escolha particular, a multiplicação e divisão no número de acertos foi realizada apenas na impressão do resultado. Diferentemente do que acontece naturalmente na função `calcPi`, no código paralelo é retornado apenas a quantidade de acertos.

A implementação do Paralelismo é apresentada abaixo:
```bash
int main(int argc, char **argv)
{
    struct timeval start, stop;
    gettimeofday(&start, 0);

    int my_rank;
    int p;
    int termos = atoll(argv[1]);
    int termos_local;
    int inicial_local;
    double acertos_parc;
    double acertos;

    MPI_Init(&argc, &argv);

    # Rank do meu processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    # Descobre quantos processos estao em uso
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    # Divisao interna
    termos_local = termos / p;

    # Bloqueia o processo até todos chegarem nesse ponto
    MPI_Barrier(MPI_COMM_WORLD);

    acertos_parc = calcPi(termos_local);

    # Soma o numero de acertos por cada processo
    MPI_Reduce(&acertos_parc, &acertos, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0)
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./pi/tempo_mpi_pi.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n",
                ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
                (double)4 * acertos / termos);

        fclose(fp);
    }
    else
    { /* Nothing */ }

    MPI_Finalize();
}
```

#### Paralelo
Para implementação da regrado do trapézio de modo paralelo, é preciso primeiro identificar as tarefas necessárias e mapear as tarefas para todos os processos. Sendo assim, é preciso: 
1. Encontrar a área de muitos trapézios individuais, o retorno destas áreas parciais serão atribuidos localmente à `area_relativa`.
2. Somar essas áreas, a soma total será atribuida à `area_total`.

Intuitivamente, conforme aumentamos o número de trapézios, receberemos uma previsão mais precisa da integral calculada. Assim, estaremos usando mais trapézios do que cores neste problema, é preciso dividir os cálculos para calcular as áreas dos mini-trapézios. O procedimento será realizado atribuindo a cada processo um subintervalo que contém o número de trapézios, obtidos a partir do cálculo do número total de trapézios `n`, dividido pelo número de processos. Isso pressupõe que o número total de trapézios é igualmente divisível pelo número de processos. Cada processo aplicará a regra do trapézio ao seu subintervalo. Por último, o processo mestre soma as estimativas.

<br>
Dado um `n`, tal que representa o número de trapézios a dividir o intervalo, a seguinte sub-rotina é implementada:  

1. É iniciada a comunicação paralela.

2. `n` é passado como argumento para a função auxiliar `setSize` junto ao rank do processo, `my_rank`, para destribuir o tamanho do problema para todos os processos usando `MPI_Bcast`.

3. O incremento é calculado pela divisão por `n` do resultado da subtração de `xb` por `xa`.

4. O número de trapézios a ser calculados por cada processo, `local_n`, é definido através da divisão de `n` pelo númeor de processos, `p`.

5. Cada processo calcula a `area_relativa` ao seu intervalo.

6. Quando todos os processos finalizam o cáculo da integral de seus respectivos intervalos, o valor de cada integral parcial é somado a `area_total`, então,  é fechada a comunicação MPI.

A implementação do Paralelismo é apresentada abaixo:
```bash
int main(int argc, char **argv)
{
    struct timeval start, stop; # Intervalo de tempo calculado ao fim
    gettimeofday(&start, 0);

    int my_rank = 0;           # Rank do meu processo
    int p = 0;                 # Numero de processos
    const double xa = 0.;      # X Início da figura
    const double xb = 30.;     # X Fim da figura
    double n = 0.;             # Numero de mini trapezios
    double inc = 0.;           # Incremento (Base do Trapezio)
    double local_a = 0.;       # X Início da figura LOCAL
    double local_b = 0.;       # X Fim da figura LOCAL
    long long int local_n = 0; # Numero de mini trapezios LOCAL

    double area_relativa = 0.; # Area relativa ao intervalo
    double area_total = 0.;    # Area total

    MPI_Init(&argc, &argv);

    # Rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    # Quantos processos então sendo usados
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    # Destribui o valor de n para todos os processos
    setSize(argc, argv, my_rank, &n);

    # O incremento e local_n serão os mesmo para todos os processos
    inc = (xb - xa) / n;
    local_n = n / p;

    # O tamanho de cada intervalo de processo será (local_n * inc)
    local_a = xa + my_rank * (local_n * inc);
    local_b = local_a + (local_n * inc);

    # Bloqueia o processo até todos chegarem nesse ponto
    MPI_Barrier(MPI_COMM_WORLD);

    area_relativa = trapezioIntegral(local_a, local_b, local_n);

    # Soma as integrais calculadas por cada processo
    MPI_Reduce(&area_relativa, &area_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        gettimeofday(&stop, 0);

        FILE *fp;
        char outputFilename[] = "./trapezio/tempo_mpi_trapezio.txt";

        fp = fopen(outputFilename, "a");
        if (fp == NULL)
        {
            fprintf(stderr, "Nao foi possivel abrir o arquivo %s!\n", outputFilename);
            exit(1);
        }

        fprintf(fp, "\tTempo: %1.2e \tResultado: %f\n", 
        ((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec)),
        area_total);

        fclose(fp);
    }
    else
    { /* Nothing */ }

    MPI_Finalize();
}
```

## Desenvolvimento

Para esta análise, serão realizadas **5 execuções** com tamanhos de problema 374.500.000, 550.000.000, 900.000.000 e 1.500.000.000 - definidos empiricamente de modo a atingir os limites mínimos determinados pela [referência](https://github.com/ozielalves/prog-paralela/tree/master/referencia) - em **3 quantidades de cores** (2, 4 e 8). Se espera que o comportamento de ambos os algoritmos quanto a aproximação do Pi seja parecido para um mesmo tamanho de problema quando se altera apenas o número de cores, sendo o tempo de execução o único fator variável. Uma descrição completa da máquina de testes pode ser encontrada no tópico [Condições de Testes](#condições-de-testes).

### Corretude

Para validar a corretude dos algoritmos implementados foi realizado um teste utilizando **4.550.000** como tamanho de problema para os dois códigos:

![Alt Corretude - Pi Paralelo e Pi Serial](./data/pi_graphs/pi_terminal_print.PNG)

Como é possível perceber, ambos os códigos conseguem aproximar de maneira correta o valor de pi, dado o número de pontos solicitados.<br><br>
**Obs.:** Vale salientar que para este modelo de amostragem quanto maior o número de pontos a serem definidos mais preciso será o valor de pi retornado.


### Gráficos

#### Serial e Paralelo - Tempo x Tamanho do Problema

![Alt Serial e Paralelo - Tempo x Tamanho do Problema](./data/pi_graphs/serial_paralelo_tempo_por_tamanho_do_problema.PNG)

Através do gráfico comparativo, é possível observar que o código paralelo é mais eficiente que o código serial pois a reta relativa a este último apresenta um coefiente angular maior do que as relativas ao primeiro, o que indica que ao se aumentar o temanho de problema no código serial o aumento em tempo de execução é proporcionalmente maior que o que seria observado no código paralelo. Vale salientar que as curvas referentes a 4 e 8 cores são praticamente idênticas, isso ocorre devido aos limites da máquina de teste, fenômeno que será mais bem explicado no item [Considerações Finais](#considerções-finais).

#### Tamanho do Problema - Tempo x Cores

![Alt Paralelo - Tempo x Cores](./data/pi_graphs/paralelo_tempo_por_cores.PNG)

A partir do gráfico apresentado, é clara a influência do número de cores no tempo de execução. Note que, por exemplo, o tempo de execução para o problema de maior tamanho cai cerca de 45% ao se passar do código serial para o código paralelo ultilizando 2 cores. Novamente, verifica-se que o desempenho para 4 e 8 cores é idêntico.


### Análise de Speedup
É possível definir o speedup, quando da utilização de n cores, como sendo o tempo de execução no código serial dividido pelo tempo médio de execução para n cores em um dado tamanho de problema. Dessa forma, o speedup representa um aumento médio de velocidade na resolução dos problemas. Sabendo que o limite de cores/threads da máquina de testes é 4, é esperado que o speedup da execução dos problemas para 4 e 8 cores seja aproximadamente idêntico.

#### Speedup x Número de Cores Utilizados

![Alt Speedup x Cores](./data/pi_graphs/Speedup_pi.jpg)

Como esperado, o gráfico nos mostra um desempenho bastante similiar para 4 e 8 cores, no entanto, para cores virtuais, a execução dos problemas em 4 cores obteve um speedup relativamente bom se comparado ao speedup para a execução nos 2 cores físicos.
 
A tabela abaixo apresenta o speedup médio por número de cores após 5 tentativas de execução dos 4 problemas descritos neste item.

| Número de Cores | 2 | 4 | 8 |
| --- | --- | ---| --- |
|**Speedup Médio**|1.80|2.47|2.44| 

### Análise de Eficiência
Através do cáculo do speedup, é possível obter a eficiência do algoritmo quando submetido a execução com as diferentes quantidades de cores. Este cálculo pode ser realizado através da divisão do speedup do algoritmo utilizando n cores pelos n cores utilizados. Como a máquina de testes possui apenas 2 cores físicos e implementa um hyper-threading para executar programas em 4 cores, para efeitos de análise comparativa iremos relacionar apenas estas duas quantidades. Porém, note que a eficiência de cores virtuais equivale a cerca de 30% da eficiência de cores físicos.

#### Eficiêcia x Tamanhos do Problema

![Alt Eficiêcia x Tamanhos do Problema](./data/pi_graphs/Eficiencia_pi.jpg)

Se olharmos atentamente para a linha representa a eficiência para 2 cores é possível identificar uma mínima queda quando aumentamos pela primeria vez o tamanho do problema, no entanto, quando aumentamos mais uma vez o tamanho de problema, a linha volta a subir de maneira a permanecer praticamente estável para o próximo tamanho de problema. Já a linha que representa a eficiência para 4 cores se aproxima visivelmente de uma reta, o que índica uma constância na eficiência se aumentando o tamanho do problema na mesma proporção em que o número de cores. Desse modo, é possível definir o algoritmo analisado como **fracamente escalável**.<br>

Apesar dos limites da máquina de testes, a eficiência média reduz de maneira considerável se compararmos o passo no uso de 2 para 4 cores, isso acontece porque aumentar a quantidade de cores utilizados gera mais comunicação, o que implica em uma maior distância em relaçao a eficiência linear. A tabela abaixo apresenta a eficiência média calculada através dos valores de speedup médio anteriormente mencionados.

| Número de Cores | 2 | 4 | 8 |
| --- | --- | ---| --- |
|**Eficiência Média**|0.90|0.61|0.30| 

## Conclusão

### Considerações Finais

Devido aos limites da máquina de testes, o número de cores passíveis de utilização é restrito. Das análises apresentadas, fica explicito que 4 cores é o limite do dispositivo junto a implementação do Hyper-threading, de  maneira a obter ainda um speedup relevante, não sendo possível estender o número de cores utilizados para 8. Apesar disto, através desta análise foi possível perceber que a paralelização de códigos seriais, ainda que simples, traz resultados bastante promissores no que diz respeito a eficiência e velocidade. Além disto, também foi permitido constatar que o speedup é ainda mais pronunciado para tamanhos maiores de problema. No entando, isto não quer dizer necessáriamente que o algoritmo tenha uma boa escalabiliade.

### Softwares utilizados
```bash
~$: g++ --version
g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

```bash
~$: python3 --version
Python 3.6.4
```

```bash
~$: grip --version
Grip 4.2.0 
```
