#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=hist_paralelo
#SBATCH --output=hist_paralelo_output.out
#SBATCH --error=hist_paralelo_error.err
#SBATCH --time=0-10:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configuração acima reserva 1 nó inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo terá acesso a 32 cores

#Loop principal de execuções. São 10 tentativas

	rm ./paralelo/pth_hist
	rm ./paralelo/tempo_pth_hist.txt

	g++ -pthread -w -o ./paralelo/pth_hist ./paralelo/pth_hist.cpp

	tentativas=10 

		for cores in 4 8 16 32 # Quantidade de cores utilziados
		do
				for size in  1024000000 2048000000 3072000000 4096000000 # Tamanhos do problema
				do   	
					echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./paralelo/tempo_pth_hist.txt" 
					for tentativa in $(seq $tentativas) 
					do
						./paralelo/pth_hist $cores $size
					done
				done

		done
		
		exit 

	exit

