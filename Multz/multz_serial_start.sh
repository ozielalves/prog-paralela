#!/bin/bash

rm ./serial/multz
rm ./serial/tempo_multz.txt

g++ -g -o ./serial/multz ./serial/multz.cpp

tentativas=5 

	for size in 92000 108000 124000 140000  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_multz.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/multz $size
		
		done
	done

	exit

exit
