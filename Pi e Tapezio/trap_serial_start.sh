#!/bin/bash

rm ./trapezio/trapezio
rm ./trapezio/tempo_trapezio.txt

g++ -g -o ./trapezio/trapezio ./trapezio/trapezio.cpp

tentativas=5 

		for size in 1200000000 2400000000 4800000000 9600000000 # Tamanhos do problema
		do   	
			echo -e "\nTamanho do Problema: $size\n" >> "./trapezio/tempo_trapezio.txt" 
			for tentativa in $(seq $tentativas) 
			do
				./trapezio/trapezio $size
			done
		done

	exit 




exit
