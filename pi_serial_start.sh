#!/bin/bash

rm ./pi/pi
rm ./pi/tempo_pi.txt

g++ -g -o ./pi/pi ./pi/pi.cpp

tentativas=5 

	for size in 374500000 550000000 900000000 1500000000 # Tamanhos do problema
	do   	
		echo -e "\nTamanho do Problema: $size\n" >> "./pi/tempo_pi.txt" 
		for tentativa in $(seq $tentativas)
		do
			./pi/pi $size
		done
	done
	
	exit 
	
exit
