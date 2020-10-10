#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh
# Permissão máxima ao este arquivo: chmod 777 shellscript_start.sh

rm pi/mpi_pi
rm pi/tempo_mpi_pi.txt

mpicc -g -o ./pi/mpi_pi ./pi/mpi_pi.cpp -lstdc++ -lm

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for cores in 2 4 8 #números de cores utilizados
	do
			for size in 374500000 550000000 900000000 1500000000 ##tamanho do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./pi/tempo_mpi_pi.txt" 
				for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
				do
					mpirun -np  $cores ./pi/mpi_pi $size 
					./pi/pi $size
				done
			done
	done
	
	exit 




exit
