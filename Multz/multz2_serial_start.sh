#!/bin/bash

#SBATCH --partition=test
#SBATCH --job-name=multz2_serial
#SBATCH --output=multz2_serial_output.out
#SBATCH --error=multz2_serial_error.err
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
rm ./serial/multz2
rm ./serial/tempo_multz2.txt

g++ -g -o ./serial/multz2 ./serial/multz2.cpp

tentativas=10 

	for size in 1200 1400 1800 2000  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_multz2.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/multz2 $size
		
		done
	done

	exit

exit
