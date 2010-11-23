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
			echo $tipodist '>' $tipodist.resmilpftnet
			$tipodist > $tipodist.resmilpftnet
			for i in $tipodist/*.mattraf ; 
			do 
				echo  ./milp_ftnet_glass testes/milp_minftnet_glass.mod $i '>>' $tipodist.resmilpftnet;
				./milp_ftnet_glass testes/milp_minftnet_glass.mod $i >> $tipodist.resmilpftnet;
			done
		done
	done
done

