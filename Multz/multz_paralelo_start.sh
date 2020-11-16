#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=multz_paralelo
#SBATCH --output=multz_paralelo_output.out
#SBATCH --error=multz_paralelo_error.err
#SBATCH --time=0-05:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configuração acima reserva 1 nó inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo terá acesso a 32 cores

#Loop principal de execuções. São 10 tentativas

	rm ./paralelo/pth_multz
	rm ./paralelo/tempo_pth_multz.txt

	g++ -pthread -w -o ./paralelo/pth_multz ./paralelo/pth_multz.cpp

	tentativas=10 

		for cores in 4 8 16 32 # Quantidade de cores utilziados
		do
				for size in 1408 1664 1920 2176 # Tamanhos do problema
				do   	
					echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./paralelo/tempo_pth_multz.txt" 
					for tentativa in $(seq $tentativas) 
					do
						./paralelo/pth_multz $cores $size
					done
				done

		done
		
		exit 

	exit

