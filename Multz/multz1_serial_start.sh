#!/bin/bash

#SBATCH --partition=test
#SBATCH --job-name=multz1_serial
#SBATCH --output=multz1_serial_output.out
#SBATCH --error=multz1_serial_error.err
#SBATCH --time=0-02:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#A configuração acima reserva 1 nó inteiro (32 cpus no mesmo processador), vai rodar apenas 1 processo por vez,
# mas este processo terá acesso a 32 cores

#Loop principal de execuções. São 10 tentativas
rm ./serial/multz1
rm ./serial/tempo_multz1.txt

g++ -g -o ./serial/multz1 ./serial/multz1.cpp

tentativas=10 

	for size in 1408 1664 1920 2176  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_multz1.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/multz1 $size
		
		done
	done

	exit

exit
