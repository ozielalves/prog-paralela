#!/bin/bash

rm ./serial/multz1
rm ./serial/tempo_multz1.txt

g++ -g -o ./serial/multz1 ./serial/multz1.cpp

tentativas=5 

	for size in 92000 108000 124000 140000  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_multz1.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/multz1 $size
		
		done
	done

	exit

exit
