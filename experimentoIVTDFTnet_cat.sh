 #!/bin/bash

for direxp in $1 ;
do
	echo $direxp ;
	for qnos in $direxp/* ;
	do
		echo $qnos ;
		rm $qnos/*.cat ;
		for tipodist in $qnos/* ;
		do
			echo $tipodist ; 
			echo sh catResult.sh $tipodist original ;
			sh catResult.sh $tipodist original ;
			echo sh catResult.sh $tipodist readiciona ;
			sh catResult.sh $tipodist readiciona ;
			echo sh catResult.sh $tipodist hmax ;
			sh catResult.sh $tipodist hmax ;
			echo sh catResult.sh $tipodist espurio ;
			sh catResult.sh $tipodist espurio ;
			echo sh catResult.sh $tipodist espreadiciona ;
			sh catResult.sh $tipodist espreadiciona ;
			cat $tipodist/original.cat > $tipodist.cat
			cat $tipodist/readiciona.cat >> $tipodist.cat
			cat $tipodist/hmax.cat >> $tipodist.cat
			cat $tipodist/espurio.cat >> $tipodist.cat
			cat $tipodist/espreadiciona.cat >> $tipodist.cat
		done
	done
done

