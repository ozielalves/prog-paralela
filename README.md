# Análise de Algoritmos Paralelos e Seriais
Universidade Federal do Rio Grande do Norte ([UFRN](http://http://www.ufrn.br)), 2020.

#### Análise por:
- [Oziel Alves](https://github.com/ozielalves/)

## Sumário

+ [Objetivos](#objetivos)
+ [Instruções de Uso](#instruções-de-uso)
  + [Dependências](#dependências)
    + [G++ Compiler](#g-compiler)
    + [MPI](#mpi---message-passing-interface)
  + [Compilação e Execução](#compilação-e-execução)
+ [Apresentação dos Algoritmos](#apresentação-dos-algoritmos)
  + [Cálculo do Pi](#cálculo-do-pi)
    + [Serial](#serial)
      + [Gráficos exclusivos](#gráficos-exclusivos-1)
      + [Tamanho x Iterações](#tamanho-x-iterações-1)
      + [Tamanho x Tempo médio](#tamanho-x-tempo-médio-1)
    + [Paralelo](#recursiva)
      + [Gráficos exclusivos](#gráficos-exclusivos-2)
      + [Tamanho x Iterações](#tamanho-x-iterações-2)
      + [Tamanho x Tempo médio](#tamanho-x-tempo-médio-2)
  + [Cálculo da Integral - Regrado do Trapézio](#cálculo-da-integral---regra-do-trapézio)
    + [Serial](#Serial)
      + [Gráficos exclusivos](#gráficos-exclusivos-3)
      + [Tamanho x Iterações](#tamanho-x-iterações-3)
      + [Tamanho x Tempo médio](#tamanho-x-tempo-médio-3)
    + [Paralelo](Paralelo)
      + [Gráficos exclusivos](#gráficos-exclusivos-4)
      + [Tamanho x Iterações](#tamanho-x-iterações-4)
      + [Tamanho x Tempo médio](#tamanho-x-tempo-médio-4)

+ [Condições de Testes](#condições-de-testes)
  + [Informações sobre a máquina utilizada](#informações-sobre-a-máquina-utilizada)
  + [Informações sobre os parametros utilizados](#informações-sobre-os-parametros-utilizados)
  + [Softwares utilizados](#softwares-utilizados)

## Objetivos
Analisar e avaliar o comportamento, eficienência e speedup dos algoritmos em relação ao seu tempo de execução, tamanho do problema e resultados obtidos. Os cenários irão simular a execução dos algoritmos para 2, 4 e 8 cores, no caso dos algorótimos paralelos, com alguns tamanhos de problema definidos empiricamente, sendo o menor tamanho estabelecido no objetivo até atingir o tempo mínimo de execução determinado pela referência da Análise (30 segundos).

## Instruções de uso
#### G++ Compiler
É necessário para a compilação do programa, visto que ele é feito em c++.
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
Instaladas as depenências, basta executar o shellcript determinado para a devida bateria de execuções na raiz do repositório:<br>
Note que serão realizados **5 execuções** com **4 tamanhos de problema** específicos para cada problema, em **3 quantidades de cores** (2, 4 e 8).
```bash
# Para o algorítimo que calcula o pi de forma serial
./pi_serial_start.sh
```
```bash
# Para o algorítimo que calcula o pi de forma paralela
./pi_paralelo_start.sh
```
```bash
# Para o algorítimo que calcula a integral de forma serial
./trap_serial_start.sh
```
```bash
# Para o algorítimo que calcula a integral de forma paralela
./trap_paralelo_start.sh
```
Obs.: Caso seja necessário conceder permissão máxima para os scripts, execute `chmod 777 [NOME DO SCRIPT].sh`
### Resultados
Após o termino das execuções do script é possível ter acesso aos arquivos `.txt` na pasta `pi` ou `trapezio`, de acordo com o script selecionado, os dados coletados foram utilizados para realização desta análise.

## Apresentação dos Algoritmos

### Cálculo do Pi
O algorítimo demonstra o método Monte Carlo para estimar o valor de **𝜋**. O método de Monte Carlo depende de amostragem independente e aleatória repetida. Esses métodos funcionam bem com sistemas paralelos e distribuídos, pois o trabalho pode ser dividido entre vários processos.

#### Serial
Dado um número `n` de pontos a serem definidos, a seguinte sub-rotina é implementada. 

1. É setado o valor `acertos` = 0.0

2. `n` pontos `x` e `y` serão definidos randomicamente com seed fixa = 42 dentro do intervalo de 0.0 a 1.0

3. Caso `( x * x + y * y )` seja menor que 1.0, `acertos` é acrescido em 1 unidade

4. Ao termino do laço, para conclusão do método de Monte Carlo,  é retornado `acertos` multiplicado por 4 e dividido por `n`

A implementação da função calcPi é apresentada abaixo:
```bash
double calcPi(int termos)
{
    // Gerador Mersene twist, SEED: 42
    mt19937 mt(42);
    // Numero real pseudo-aleatorio
    uniform_real_distribution<double> linear_r(0.f, 1.f);

    int acertos = 0;
    for (int i = 0; i < termos; i++)
    {

        double x = linear_r(mt);
        double y = linear_r(mt);
        
        if (x * x + y * y < 1.0)
        {
            acertos++;
        }
    }
    return (double)(4.0 * acertos / termos);
}
```
Vale salientar que para este modelo de amostragem quanto maior o número de pontos a serem definidos mais preciso será o valor de pi retornado.

##### Análise de Speedup
![Alt Serial - Tamanho x Velocidade](./data/pi)
##### Análise de Eficiência
![Alt Tamanho x Iterações](./pi/lonely/1-Sequential%20Search_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/1-Sequential%20Search_13.png)

### Cálculo da Integral usando a Regra do Trapézio
```bash
/*
 * Peter S. Pacheco, An Introduction to Parallel Programming,
 * Morgan Kaufmann Publishers, 2011
 * IPP:   Section 3.4.2 (pp. 104 and ff.)
 * Timing and command line argument added by Hannah Sonsalla, 
 * Macalester College, 2017
 *
 * mpi_trap.c
 *
 * ... Use MPI to implement a parallel version of the trapezoidal
 *     rule.  Uses collective communications to distribute the
 *     input data and compute the global sum.
 *
 * Input:    Number of trapezoids
 * Output:   Estimate of the integral from a to b of f(x)
 *           using the trapezoidal rule and n trapezoids.
 *
 * Usage:    mpirun -np <number of processes> ./mpi_trap < number of trapezoids>
 *
 * Algorithm:
 *    1.  Each process calculates "its" interval of
 *        integration.
 *    2.  Each process estimates the integral of f(x)
 *        over its interval using the trapezoidal rule.
 *    3a. Each process != 0 sends its integral to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Note:  f(x) is all hardwired to x*x.
 *
 */
```
Também conhecida como busca logaritmica, é um algoritmo de busca baseado na técnica *divide and conquer* e dependente de uma ordenação, que melhora notávelmente sua performance, visto que a cada iteração, é descartado metade do conjunto.
#### Iterativa
Dado um conjunto $A$ de $n$ elementos ordenados e um alvo $T$, é seguida a sub-rotina para encontrar o elemento:

1. É setado $L = 0$ e $R = n - 1$.
2. Se $L < R$ , a busca termina sem sucesso.
3. É setado a posição do meio ($m$) do elemento para  $\left \lfloor \frac{L + R}{2}\right \rfloor$.
4. Se $A_m < T$, será setado $L = m + 1$ e retorna ao passo 2.
5. Se $A_m > T$, será setado $R = m - 1$ e retorna ao passo 2.
6. Se $A_m = T$, a busca é finalizada retornando $m$.



Pelo fato da busca binária utilizar-se do método *divide and conquer*, acaba se tornando uma função logaritma que, em seu melhor caso, o elemento procurado estará exatamente em $\frac{n}{2}$ , já em seu pior caso, irá ter $\mathcal{O} \log_2 n$ comparações.

##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/2-Iterative%20Binary_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/2-Iterative%20Binary_13.png)

#### Recursiva

Apesar de possuir a mesma complexidade de tempo da Binária Iterativa, sua versão recursiva pode variar drasticamente em termos de memória consumida durante a execução (irá depender tanto do compilador quanto das otimizações feitas pelo programador).

##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/3-Recursive%20Binary_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/3-Recursive%20Binary_13.png)

É de fato um método de busca imensamente mais eficiente que o algoritmo de busca linear. Porém uma de suas desvantagens aparece quando o vetor não é ordenado (seja por impossibilidade, fluxo de entrada constante, números repetidos ...) ou quando o vetor implementado não suporta *random acess*, como por exemplo, listas encadeadas.

### Busca Ternária
Busca Ternária é uma técnica em ciência da computação para encontrar o mínimo ou o máximo de uma função unimodal. Uma busca ternária determina se o mínimo ou o máximo podem ou não estar no primeiro terço do domínio ou se ele pode ou não estar no último terço do domínio e, em seguida, repete o passo para o terceiro restante.
#### Iterativa
Seja $f(x)$ uma função unimodal num intervalo $[ l ; r ]$. Pega-se dois pontos $m1$ e $m2$ no segmento: $l < m1 < m2 < r$ 

Depois, segue-se três passos:

1. Se $f(m1) < f(m2)$: Então o elemento buscado não pode ser localizado no lado esquerdo $[ l ; m1 ]$. Então olha-se apenas no intervalo $[ m1 ; r ]$
2. Se $f(m1) > f(m2)$, que a situação é semelhante à anterior, até a simetria. Agora, o elemento buscado não pode estar no lado direito $[ m2 ; r ]$, então olha-se no segmento $[ l ; m2 ]$
3. Se $f(m1) = f(m2)$, a busca deve ser realizada em $[ m1 ; m2 ]$, mas este caso pode ser atribuído a qualquer um dos dois anteriores (para simplificar o código).

Recomendações para $m1$ e $m2$: 

​	$m1 = \frac{l + (r-l)}{3}$ 

​	$m2 = \frac{r - (r- l) }{3}$


Devido a divisão recurssiva do vetor por 3, a função, em seu pior caso, assume comportamento logarítimico, o que gera uma complexidade de $\mathcal{O} (log_3 n)$.

##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/5-Iterative%20Ternary_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/5-Iterative%20Ternary_13.png)
#### Recursiva
##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/6-Recursive%20Ternary_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/6-Recursive%20Ternary_13.png)
É uma função pouco usada, visto que a busca binária faz basicamente o mesmo trabalho só que com uma comparação a menos em cada iteração, causando uma complexidade de $2 \log_3(n)$, contra $\log_2(n)$ da busca binária.

### Jump Search
<!--Breve explicação-->
Como a Binary Search, Jump Search, ou Block Search é um algoritmo de pesquisa para arrays ordenados. A idéia básica é verificar menos elementos (do que a busca linear) saltando por etapas fixas ou ignorando alguns elementos no lugar de pesquisar todos os elementos.
<!--Pseudo-Código-->
Dada um vetor ordenado $L$, seu comprimento , queremos achar o elemento $S$.

Sendo $L_{min(a, b) - 1}$ o menor elemento entre $a$ e $b$:

1. Iremos setar $a = 0$ e $b = 0$

1. Enquanto que $L_{min((b, n) -1)}$ < $S$:  Iremos setar $a = b$ e $b = b + ⌊√n⌋$
2. Se $a ≥ n$, o programa termina e é retornado `null`.
3. Enquanto que $L_a$ < $S$: Iremos incrementar $a$ em 1 unidade.
4. Se $a = min (b, n)$, o programa termina e é retornado `null`.
5. Se $L_a$ = $S$, será retornado $a$.
6. Se não: é retornado `null`.

<!--Complexidade-->
Devido a comparação dos blocos formados,ambas as etapas do algoritmo observam, no máximo, √n itens, o que leva o algoritmo a apresentar em seu pior caso complexidade $\mathcal{O}\sqrt[]n$.

##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/4-Jump%20Search_14.png)

###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/4-Jump%20Search_13.png)
<!--Opnião-->

É de fato uma alternativa melhor que a busca sequencial, porém pior do que a busca binária. Pode ser um algoritmo útil caso empregado em casos específicos bem planejados, tais como um vetor que as chances do elemento buscado estar em uma das posições multiplas de $\sqrt{n}$ seja alta.

### Busca Fibonacci
<!--Breve explicação-->
Fibonacci Search é uma técnica baseada em comparação, em um array ordenado, que empregando a filosofia _divide and conquer_, usa números da sequ6encia Fibonacci para pesquisar um elemento em um determinado vetor.

<!--Pseudo-Código-->
1. Encontra o menor número de Fibonacci maior ou igual ao valor passado para a busca (`value`). 
2. Deixe que este número seja $fib_m$
3. Deixe que os dois números de Fibonacci que precedem sejam $fib_{m-1}$  e $fib_{m-2}$.
4. Enquanto o vetor possui elementos a serem inspecionados:
  1. Compara `value` com o último elemento da gama coberta por $fib_{m-2}$
  2. Se `value` corresponder, retorna endereço.
  3. Se $fib_m$ < `value`, reduz $fib_{m-2}$, $fib_{m-1}$ e $fib_m$ em dois números de fibonacci, indicando a eliminação de cerca de dois terços do vetor restante
  4. Se $fib_m$ for menor que value, reduz $fib{m-2}$, $fib_{m-1}$ e $fib_m$ em três números de fibonacci, redefina o deslocamento para o endereço. Juntos, estes passos indicam a eliminação de cerca de um terço do vetor restante.

Devido a eliminação de ranges, em seu pior caso a busca Fibonacci acaba se tornando uma função logarítimica de complexidade $\mathcal{O}\log n$.

##### Gráficos exclusivos
###### Tamanho x Iterações
![Alt Tamanho x Iterações](./pi/lonely/7-Fibonacci%20Search_14.png)
###### Tamanho x Tempo médio
![Alt Tamanho x Tempo médio](./pi/lonely/7-Fibonacci%20Search_13.png)
<!--Opnião-->
No geral, a busca fibonacci leva a aproximadamente 4% mais comparações que a busca binária. Porém, sua real vantagem está no fato que só necessita de adições e subtrações (se implementada corretamente), o que torna o trabalho da CPU muito menos danoso do que em divisões (como na binária).

Quando os elementos estão em uma memória não totalmente uniforme (i.e. quando o tempo de acesso a determinadas partes da memória pode variar), ela também pode levar uma pequena vantagem em relação a busca binária pois reduz pouca coisa a quantidade de acessos à memória. 

## Comparações Gerais
### Recursivos x Iterativos
#### Busca Binária

A busca binária em suas duas versões apresenta complexidade de tempo semelhante, visto o gráfico abaixo. A única diferença entre as duas é que uma irá (provavelmente) consumir mais memória devido a recursividade.

![Alt Iterativo x Recursivo (Por tempo)](./pi/versus/2-Iterative%20Binary_3-Recursive%20Binary_13.png)
#### Busca Ternária

Mesma aspecto observado na binária, única diferença é que a versão recursiva (provavelmente) consumirá mais memória devido a sua recursividade.

![Alt Iterativo x Recursivo (Por tempo)](./pi/versus/5-Iterative%20Ternary_6-Recursive%20Ternary_13.png)

### Recursivos x Busca Fibonacci
#### Tamanho x Iterações

[Fibonacci x Recursive Binary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/3-Recursive%20Binary_7-Fibonacci%20Search_14.png)

[Fibonacci x Recursive Ternary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/6-Recursive%20Ternary_7-Fibonacci%20Search_14.png)

#### Tamanho x Tempo

[Fibonacci x Recursive Binary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/3-Recursive%20Binary_7-Fibonacci%20Search_13.png)

[Fibonacci x Recursive Ternary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/6-Recursive%20Ternary_7-Fibonacci%20Search_13.png)

### Recursivos x Jump Search
#### Tamanho x Iterações
[Jump Search x Recursive Binary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/3-Recursive%20Binary_4-Jump%20Search_14.png)

[Jump Search x Recursive Ternary](./pi/versus/4-Jump%20Search_6-Recursive%20Ternary_14.png)

#### Tamanho x Tempo
[Jump Search x Recursive Binary](https://github.com/ozielalves/Analise_empirica/blob/master/pi/versus/3-Recursive%20Binary_4-Jump%20Search_13.png)

[Jump Search x Recursive Ternary](./pi/versus/4-Jump%20Search_6-Recursive%20Ternary_13.png)

## Condições de Testes
### Informações sobre a maquina utilizada
+ **Dell Inspiron 14' 7460** (13-inch, 2017)

+ **Processador** Intel Core i7 7500U (até 3.5 GHz) Cache 4M. (FSB)4 GT/s OPI

+ **Memória** 8 GB tipo DDR4 – 2133MHz

+ **Sistema** Ubuntu 20.04.1 LTS

### Informações sobre os parametros utilizados
Todos as informações interpretadas neste documento foram obtidas utilizando o seguinte comando::
```bash
# Irá compilar e executar o arquivo binário, em seguida o script gerador de gráficos (src/gen_plot.py)
make run
```
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
~$: pip3 --version
pip 9.0.1 from /usr/local/lib/python3.6/site-packages (python 3.6)
```
