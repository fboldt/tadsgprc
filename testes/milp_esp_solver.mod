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
param MatTraf{s in I, d in I}, >= 0;

# 4 - Parcela de tráfego vindo de "s" passando pelo enlace (i,j)
param Hijsd{i in I, j in I, s in I, d in I}, >= 0;

# 5 - Matriz da topologia virtual (versao solver)
param Bij{i in I, j in I}, binary;

#############################################################
########################### VARIÁVEIS #######################
#############################################################

# 1 - Matriz da topologia virtual
#var Bij{i in I, j in I}, binary;

# 2 - Variáveis de decisão da Matriz da topologia virtual 
var Bijsd{i in I, j in I, s in I, d in I}, binary;

# 4 - Componente de tráfego válidos (V) vindo de "s" passando pelo enlace (i,j)
var HVijsd{i in I, j in I, s in I, d in I} >= 0;

# 5 - Trafego transportado pelo caminho optico que interliga os nós (i,j)
var Hij{i in I, j in I } >= 0;

# 6 - Tráfego total sem componentes espúrios 
var Bnet >=0;

#############################################################
########################### RESTRIÇÕES ######################
#############################################################

# 1 - Relaciona variáveis de decisão de componentes de tráfego com as de topologia logica
 s.t.  relbijsdbij{i in I, j in I, s in I, d in I}: Bijsd[i,j,s,d] <= Bij[i,j];

# 2 - Totalizacao do trafego valido
s.t. defBnet: Bnet = sum {i in I, j in I, s in I, d in I} Bijsd[i,j,s,d];

# 3 - Restriçao de conservaçao de trafego tipo 1
s.t.  conserv1{i in I, j in I}: Hij[i,j] = sum{s in I, d in I} HVijsd[i,j,s,d];

# 4 - Restriçao de conservaçao de trafego tipo 2
s.t.  conserv2{i in I, s in I, d in I}: 
		sum {j in I} HVijsd[i,j,s,d] - sum {j in I} HVijsd[j,i,s,d] = 
		(if s = i then MatTraf[s,d] 
		else (if d = i then -MatTraf[s,d]
		else 0));

# 5 - Definição dos componentes de tráfego válidos HVijsd
s.t.  compval{i in I, j in I, s in I, d in I}: HVijsd[i,j,s,d] - Hijsd[i,j,s,d]*Bijsd[i,j,s,d] = 0;

#############################################################
######################## FUNÇÃO OBJETIVO ####################
#############################################################

minimize trafego_total: Bnet;

solve;

printf{i in I, j in I, s in I, d in I}: "HVijsd[%s,%s,%s,%s] - Hijsd[%s,%s,%s,%s] = %f\n", i, j, s, d, i, j, s, d, (HVijsd[i,j,s,d]-Hijsd[i,j,s,d]);

end;

