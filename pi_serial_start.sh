#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh
# Permissão máxima ao este arquivo: chmod 777 shellscript_start.sh

rm ./pi/pi
rm ./pi/tempo_pi.txt

g++ -g -o ./pi/pi ./pi/pi.cpp

#OBRIGATÓRIO: Laço de iteração para resgate dos tempos de acordo com "cores" e "size"

#Loop principal de execuções. São 10 tentativas
tentativas=5 #Quantas vezes o código será executado dado um par (cores,size)

	for size in 374500000 550000000 900000000 1500000000 ##tamanho do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./pi/tempo_pi.txt" 
		for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"
		do
			./pi/pi $size
		done
	done
	
	exit 




exit
