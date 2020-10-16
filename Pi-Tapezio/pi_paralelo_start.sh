#!/bin/bash

rm ./pi/mpi_pi
rm ./pi/tempo_mpi_pi.txt

mpicc -g -o ./pi/mpi_pi ./pi/mpi_pi.cpp -lstdc++ -lm

tentativas=5 

	for cores in 2 4 8 # Cores
	do
			for size in 374500000 550000000 900000000 1500000000 # Tamanhos do Problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./pi/tempo_mpi_pi.txt" 
				for tentativa in $(seq $tentativas)
				do
					mpirun -np  $cores ./pi/mpi_pi $size 
				done
			done
	done
	
	exit 

exit
