#!/bin/bash

#INFORMAÇÕES- Para rodar no computador pessoal.
# EXECUTAR NO TERMINAL: ./shellscript_start.sh
	#Caso não funcione, conceda permissão máxima ao este arquivo: chmod 777 shellscript_start.sh

#OPCIONAL: apagar arquivos temporários (gerados ou não pelo .c).
rm tempo_mpi_trapezio.txt
rm mpi_trapezio

#Compilação de Código. Modifique para o que mais se adequa a você.
mpicc -g -o mpi_trapezio mpi_trapezio.cpp -lstdc++ -lm

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for cores in 2 4 8 #números de cores utilizados
	do
			for size in 10 20 #tamanho do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\t\t\c" >> "tempo_mpi_trapezio.txt" 

				for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
				do
					#Executar o código. Modifique para o que mais se adequa a você.
					mpirun -np  $cores ./mpi_trapezio $size 
					#no meu PC, tive que utilizar "--oversubscrive" para rodar. No seu pode não ser necessário. 
				done
			done

	done
	
	exit 




exit
