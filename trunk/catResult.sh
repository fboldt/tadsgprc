#!/bin/bash

echo $2 > $1/$2.cat
for arquivo in $1/*.$2
do 
	cat $arquivo >> $1/$2.cat
done

sed "s/\./,/g" $1/$2.cat > tmp
cat tmp > $1/$2.cat

echo mean >> $1/$2.cat
echo stdev >> $1/$2.cat
echo stdevp >> $1/$2.cat

