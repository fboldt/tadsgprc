/*
Formulação do Modelo Agregado MILP
- Minimizacao de congestionamento Hmax [Ramaswami, 96]
- Descrição do modelo em MathProg por Renato Tannure R. de Almeida
*/

#############################################################
########################### PARÂMETROS ######################
#############################################################

# 1 - Numero de nós da rede 
param N, integer, > 0;

# 2 - Conjunto de indices
set I := 1..N;

# 3 - Matriz de Trafego
param MatTraf{s in I, d in I};

# 4 - Somas do tráfego partindo de cada nó
param Hs{s in I}  :=  sum{d in I: d!=s} MatTraf[s,d];

#############################################################
########################### VARIÁVEIS #######################
#############################################################

# 2 - Matriz da topologia virtual
var  Bij{i in I, j in I}, binary;

# 3 - Parcela de tráfego vindo de "s" passando pelo enlace (i,j)
var Hijs{i in I, j in I, s in I} >= 0;

# 3 - trafego transportado atraves do caminho optico que interliga os nós (i,j)
var Hij{i in I, j in I } >= 0;

# 4 - Congestionamento
var Hmax >=0;

#############################################################
########################### RESTRIÇÕES ######################
#############################################################

# 1 - Restriçao de limitaçao de fluxo
s.t.  limit{i in I, j in I}: Hmax >= Hij[i,j];

# 2 - Restriçao de limitaçao de fluxo tipo 2
s.t.  limit2{i in I, j in I, s in I}: Hijs[i,j,s] <= (Bij[i,j] * Hs[s]);

# 3 - Restriçao de conservaçao de trafego tipo 1
s.t.  conserv1{i in I, j in I}: Hij[i,j] = sum{s in I} Hijs[i,j,s];

# 4 - Restriçao de conservaçao de trafego tipo 2
s.t.  conserv2{i in I, s in I}: 
      sum{j in I} Hijs[i,j,s] - sum{j in I} Hijs[j,i,s] =
	if (s!=i) then -MatTraf[s,i];
	
# 5 - Restriçao de conservaçao de trafego tipo 3
s.t.  conserv3{i in I, s in I}: 
      sum{j in I} Hijs[i,j,s] - sum{j in I} Hijs[j,i,s] =
	if (s=i) then Hs[s];


#############################################################
######################## FUNÇÃO OBJETIVO ####################
#############################################################

minimize congestionamento: Hmax;

end;

