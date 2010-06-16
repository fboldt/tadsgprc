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
				echo  ./ivtd_ftnet_original.bin testes/milp_ftnet.mod $i ;
				 ./ivtd_ftnet_original.bin testes/milp_ftnet.mod $i ;
			done
		done
	done
done

