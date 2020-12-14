#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=filters_paralelo
#SBATCH --output=filters_paralelo_output.out
#SBATCH --error=filters_paralelo_error.err
#SBATCH --time=0-10:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=4
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configuração acima reserva 1 nó inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo terá acesso a 32 cores

#Loop principal de execuções. São 10 tentativas
	module load compilers/gnu/8.3

	rm ./paralelo/omp_filters
	rm ./paralelo/tempo_omp_filters.txt

	g++ -fopenmp -w -o ./paralelo/omp_filters ./paralelo/omp_filters.cpp

	tentativas=10 

		for cores in 4 8 16 32 # Quantidade de threads utilziados
		do
				for image in 1 2 3 4 # Tamanhos do problema
				do   	
					echo -e "\nThreads Qtd: $threads\tImagem: $image\n" >> "./paralelo/tempo_omp_filters.txt" 
					for tentativa in $(seq $tentativas) 
					do
						./paralelo/omp_filters $threads $image
					done
				done

		done
		
		exit 

	exit

