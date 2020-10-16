#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh

rm ./trapezio/mpi_trapezio
rm ./trapezio/tempo_mpi_trapezio.txt

mpicc -g -o ./trapezio/mpi_trapezio ./trapezio/mpi_trapezio.cpp -lstdc++ -lm

tentativas=5 

	for cores in 2 4 8 # Quantidade de cores utilziados
	do
			for size in 1200000000 2400000000 4800000000 9600000000 # Tamanhos do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./trapezio/tempo_mpi_trapezio.txt" 
				for tentativa in $(seq $tentativas) 
				do
					mpirun -np  $cores ./trapezio/mpi_trapezio $size 
				done
			done

	done
	
	exit 

exit
