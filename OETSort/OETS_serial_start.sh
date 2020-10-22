#!/bin/bash

rm ./serial/oets
rm ./serial/tempo_oets.txt

g++ -g -o ./serial/oets ./serial/oets.cpp

tentativas=5 

	for size in 92000 108000 124000 140000  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_oets.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/oets $size
		
		done
	done

	exit

exit
