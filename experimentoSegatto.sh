count=0	
while [ $count -lt 13 ]
do					
    count=`expr $count + 1`
    echo "./segatto.bin $1 $2 $count > segatto/$2.gl$count.txt"		
    ./segatto.bin $1 $2 $count > segatto/$2.gl$count.txt
done					

