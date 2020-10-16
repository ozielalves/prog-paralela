#!/bin/bash

rm ./serial/trapezio
rm ./serial/tempo_trapezio.txt

g++ -g -o ./trapezio/trapezio ./trapezio/trapezio.cpp

tentativas=5 

		for size in 1200000000 2400000000 4800000000 9600000000 # Tamanhos do problema
		do   	
			echo -e "\nTamanho do Problema: $size\n" >> "./serial/tempo_trapezio.txt" 
			for tentativa in $(seq $tentativas) 
			do
				./trapezio/trapezio $size
			done
		done

	exit 




exit
