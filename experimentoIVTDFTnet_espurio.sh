 #!/bin/bash

for direxp in $1 ;
do
	echo $direxp ;
	for qnos in $direxp/* ;
	do
		echo $qnos ;
		for tipodist in $qnos/* ;
		do
			echo $tipodist ; 
			for i in $tipodist/*.mattraf ; 
			do 
				echo  ./ivtd_ftnet_esp.bin testes/milp_ftnet.mod testes/milp_esp.mod $i ;
				 ./ivtd_ftnet_esp.bin testes/milp_ftnet.mod testes/milp_esp.mod $i ;
			done
		done
	done
done

#sh experimentoIVTDFTnet_cat.sh $1

