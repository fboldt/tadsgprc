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
				echo  ./ivtd_hmax_esp.bin testes/milp_hmax.mod testes/milp_esp.mod $i ;
				 ./ivtd_hmax_esp.bin testes/milp_hmax.mod testes/milp_esp.mod $i ;
				echo  ./ivtd_hmax_original.bin testes/milp_hmax.mod $i ;
				 ./ivtd_hmax_original.bin testes/milp_hmax.mod $i ;
			done
		done
	done
done

for direxp in $1 ;
do
	echo $direxp ;
	for qnos in $direxp/* ;
	do
		echo $qnos ;
		for tipodist in $qnos/* ;
		do
			echo $tipodist ; 
			echo sh catResult.sh $tipodist original ;
			sh catResult.sh $tipodist original ;
			echo sh catResult.sh $tipodist readiciona ;
			sh catResult.sh $tipodist readiciona ;
			echo sh catResult.sh $tipodist espurio ;
			sh catResult.sh $tipodist espurio ;
			echo sh catResult.sh $tipodist espreadiciona ;
			sh catResult.sh $tipodist espreadiciona ;
			cat $tipodist/original.cat > $tipodist.cat
			cat $tipodist/readiciona.cat >> $tipodist.cat
			cat $tipodist/espurio.cat >> $tipodist.cat
			cat $tipodist/espreadiciona.cat >> $tipodist.cat
		done
	done
done

