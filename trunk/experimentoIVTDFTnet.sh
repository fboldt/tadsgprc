 #!/bin/bash

sh experimentoIVTDFTnet_original.sh $1 & 

sh experimentoIVTDFTnet_espurio.sh $1 ;
sh experimentoIVTDFTnet_cat.sh $1 ;

