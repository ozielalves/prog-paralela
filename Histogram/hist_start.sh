#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=hist_serial
#SBATCH --output=hist_output.out
#SBATCH --error=hist_error.err
#SBATCH --time=0-05:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configuração acima reserva 1 nó inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo terá acesso a 32 cores

#Loop principal de execuções. São 10 tentativas
rm ./serial/hist
rm ./serial/tempo_hist.txt

g++ -w -g -o ./serial/hist ./serial/hist.cpp

tentativas=10 

	for size in 1024000000 2048000000 3072000000 4096000000 # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_hist.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/hist $size
		
		done
	done

	exit

exit
