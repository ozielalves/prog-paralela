#!/bin/bash

rm ./serial/multz2
rm ./serial/tempo_multz2.txt

g++ -g -o ./serial/multz2 ./serial/multz2.cpp

tentativas=5 

	for size in 92000 108000 124000 140000  # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_multz2.txt" 
		
		for tentativa in $(seq $tentativas) 
		do
			./serial/multz2 $size
		
		done
	done

	exit

exit
