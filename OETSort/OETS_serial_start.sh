#!/bin/bash

rm ./serial/oets
rm ./serial/tempo_oets.txt

g++ -g -o ./oets/oets ./oets/oets.cpp

tentativas=5 

		for size in 1200000000 2400000000 4800000000 9600000000 # Tamanhos do problema
		do   	
			echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_oets.txt" 
			for tentativa in $(seq $tentativas) 
			do
				./oets/oets $size
			done
		done

	exit 
exit
