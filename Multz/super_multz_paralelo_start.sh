#!/bin/bash

#SBATCH --partition=test
#SBATCH --job-name=multz_paralelo
#SBATCH --output=multz_paralelo_output.out
#SBATCH --error=multz_paralelo_error.err
#SBATCH --time=0-02:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32

#No Supercomputador, 1 n� = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configura��o acima reserva 1 n� inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo ter� acesso a 32 cores

#Loop principal de execu��es. S�o 10 tentativas

	rm ./paralelo/pth_multz
	rm ./paralelo/tempo_pth_multz.txt

	g++ -pthread -w -o ./paralelo/pth_multz ./paralelo/pth_multz.cpp

	tentativas=10 

		for cores in 4 8 16 32 # Quantidade de cores utilziados
		do
				for size in 2000000000 4000000000 6000000000 8000000000 # Tamanhos do problema
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

