#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh
# Permissão máxima ao este arquivo: chmod 777 shellscript_start.sh

#OPCIONAL: apagar arquivos temporários (gerados ou não pelo .c).
rm ./trapezio/trapezio
rm ./trapezio/tempo_trapezio.txt

g++ -g -o ./trapezio/trapezio ./trapezio/trapezio.cpp

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
	tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

		for size in 1200000000 2400000000 4800000000 9600000000 #tamanho do problema
		do   	
			echo -e "\nTamanho do Problema: $size\n" >> "./trapezio/tempo_trapezio.txt" 
			for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
			do
				./trapezio/trapezio $size
			done
		done

	exit 




exit
