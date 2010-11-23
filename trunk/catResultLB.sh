#!/bin/bash

for i in $1/*
do
	rm $i/lbi.cat
	for arquivo in $i/*.reslbi
	do 
		echo 0 >> $arquivo
		./printiffloat < $arquivo >> $i/lbi.cat
	done

	sed "s/\./,/g" $i/lbi.cat > tmp
	cat tmp > $i/lbi.cat
done

