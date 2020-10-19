#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh

rm ./paralelo/mpi_oets
rm ./paralelo/tempo_mpi_oets.txt

mpicc -g -o ./paralelo/mpi_oets ./paralelo/mpi_oets.cpp -lstdc++ -lm

tentativas=5 

	for cores in 2 4 8 # Quantidade de cores utilziados
	do
			for size in 1200000000 2400000000 4800000000 9600000000 # Tamanhos do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./paralelo/tempo_mpi_oets.txt" 
				for tentativa in $(seq $tentativas) 
				do
					mpirun -np  $cores ./paralelo/mpi_oets $size 
				done
			done

	done
	
	exit 

exit
