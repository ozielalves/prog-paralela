#!/bin/bash
# EXECUTAR NO TERMINAL: ./shellscript_start.sh

rm ./paralelo/pth_multz
rm ./paralelo/tempo_pth_multz.txt

g++ -pthread -w -o ./paralelo/pth_multz ./paralelo/pth_multz.cpp

tentativas=5 

	for cores in 2 4 # Quantidade de cores utilziados
	do
			for size in 4 8 12 16 # Tamanhos do problema
			do   	
				echo -e "\nCores Qtd: $cores\tTamanho do Problema: $size\n" >> "./paralelo/tempo_pth_multz.txt" 
				for tentativa in $(seq $tentativas) 
				do
					./paralelo/pth_multz $cores $size
				done
			done

	done
	
	exit 

exit
