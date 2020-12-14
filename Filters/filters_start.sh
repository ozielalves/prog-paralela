#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=filters_serial
#SBATCH --output=filters_output.out
#SBATCH --error=filters_error.err
#SBATCH --time=0-06:0
#SBATCH --hint=compute_bound 

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=32000

#No Supercomputador, 1 nó = 32 Cores (ou CPUs)
#Lembrar que: TASK = PROCESSO

#Loop principal de execuções. São 10 tentativas
rm ./serial/filters
rm ./serial/tempo_filters.txt

g++ -w -g -o ./serial/filters ./serial/filters.cpp

tentativas=10 

	for image in 1 2 3 4 # Tamanhos do problema
	do   	
		echo -e "\nImagem: $image\n" >> "./serial/tempo_filters.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/filters $image
		
		done
	done

	exit

exit
