# sh similaridade.sh tamanhoRede quantTestes
count=0	
while [ $count -lt 13 ]
do					
    count=`expr $count + 1`
    echo "./testeSimilaridade.bin $1 $2 $count > segatto/similaridade.$2.gl$count.tam$1.txt"		
    ./testeSimilaridade.bin $1 $2 $count > segatto/similaridade.$2.gl$count.tam$1.txt
done					

