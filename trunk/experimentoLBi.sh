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
				cat $direxp/hmaxl $i > $i.lbi;
				echo  ./lbi_hmax_glass testes/lbi_MinHmax_GlAss.mod $i.lbi  > $i.reslbi;
				 ./lbi_hmax_glass testes/lbi_MinHmax_GlAss.mod $i.lbi > $i.reslbi;
			done
		done
	done
done

