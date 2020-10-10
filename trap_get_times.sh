#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh
# Permissão máxima ao este arquivo: chmod 777 shellscript_start.sh

#OPCIONAL: apagar arquivos temporários (gerados ou não pelo .c).
rm ./trapezio/mpi_trapezio
rm ./trapezio/trapezio

#Compilação de Código. Modifique para o que mais se adequa a você.
mpicc -g -o ./trapezio/mpi_trapezio ./trapezio/mpi_trapezio.cpp -lstdc++ -lm
g++ -g -o ./trapezio/trapezio ./trapezio/trapezio.cpp

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for cores in 2 4 8 #números de cores utilizados
	do
			for size in 10000 100000 #10000 100000 #tamanho do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./trapezio/tempo_mpi_trapezio.txt" 
				echo -e "\nTamanho do Problema: $size\n" >> "./trapezio/tempo_trapezio.txt" 
				for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
				do
					#Executar o código. Modifique para o que mais se adequa a você.
					mpirun -np  $cores ./trapezio/mpi_trapezio $size 
					./trapezio/trapezio $size
				done
			done

	done
	
	exit 




exit
