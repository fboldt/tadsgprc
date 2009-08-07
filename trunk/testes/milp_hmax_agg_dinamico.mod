/*
Formula��o do Modelo Agregado MILP
- Minimizacao de congestionamento Hmax [Ramaswami, 96]
- Descri��o do modelo em MathProg por Renato Tannure R. de Almeida
*/

#############################################################
########################### PAR�METROS ######################
#############################################################

# 1 - Numero de n�s da rede 
param N, integer, > 0;

# 2 - Conjunto de indices
set I := 1..N;

# 3 - Matriz de Trafego
param MatTraf{s in I, d in I};

# 4 - Somas do tr�fego partindo de cada n�
param Hs{s in I}  :=  sum{d in I: d!=s} MatTraf[s,d];

#############################################################
########################### VARI�VEIS #######################
#############################################################

# 2 - Matriz da topologia virtual
var  Bij{i in I, j in I}, binary;

# 3 - Parcela de tr�fego vindo de "s" passando pelo enlace (i,j)
var Hijs{i in I, j in I, s in I} >= 0;

# 3 - trafego transportado atraves do caminho optico que interliga os n�s (i,j)
var Hij{i in I, j in I } >= 0;

# 4 - Congestionamento
var Hmax >=0;

#############################################################
########################### RESTRI��ES ######################
#############################################################

# 1 - Restri�ao de limita�ao de fluxo
s.t.  limit{i in I, j in I}: Hmax >= Hij[i,j];

# 2 - Restri�ao de limita�ao de fluxo tipo 2
s.t.  limit2{i in I, j in I, s in I}: Hijs[i,j,s] <= (Bij[i,j] * Hs[s]);

# 3 - Restri�ao de conserva�ao de trafego tipo 1
s.t.  conserv1{i in I, j in I}: Hij[i,j] = sum{s in I} Hijs[i,j,s];

# 4 - Restri�ao de conserva�ao de trafego tipo 2
s.t.  conserv2{i in I, s in I}: 
      sum{j in I} Hijs[i,j,s] - sum{j in I} Hijs[j,i,s] =
	if (s!=i) then -MatTraf[s,i];
	
# 5 - Restri�ao de conserva�ao de trafego tipo 3
s.t.  conserv3{i in I, s in I}: 
      sum{j in I} Hijs[i,j,s] - sum{j in I} Hijs[j,i,s] =
	if (s=i) then Hs[s];


#############################################################
######################## FUN��O OBJETIVO ####################
#############################################################

minimize congestionamento: Hmax;

end;

