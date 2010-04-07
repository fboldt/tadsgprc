/*
Formula��o do Modelo MILP Desagregado
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

#############################################################
########################### VARI�VEIS #######################
#############################################################

# 1 - Matriz da topologia virtual
var  Bij{i in I, j in I}, binary;

# 2 - Parcela de tr�fego vindo de "s" indo para "d" passando pelo enlace (i,j)
var Hijsd{i in I, j in I, s in I, d in I} >= 0;

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
s.t.  limit2{i in I, j in I, s in I, d in I}: (Bij[i,j] * MatTraf[s,d]) - Hijsd[i,j,s,d] >= 0;

# 3 - Restri�ao de conserva�ao de trafego tipo 1
s.t.  conserv1{i in I, j in I}: Hij[i,j] - sum{s in I,d in I} Hijsd[i,j,s,d] = 0;

# 4 - Restri�ao de conserva�ao de trafego tipo 2
s.t.  conserv2{i in I, s in I, d in I}: 
		sum {j in I} Hijsd[i,j,s,d] - sum {j in I} Hijsd[j,i,s,d] = 
		(if s = i then MatTraf[s,d] 
		else (if d = i then -MatTraf[s,d]
		else 0));
		
# 5 - Restri��o de anula��o de componentes Hijsd com s=j e d=i.
s.t. anul{i in I, j in I, s in I, d in I: s = j, d = i}: Hijsd[i,j,d,s] = 0;

#############################################################
######################## FUN��O OBJETIVO ####################
#############################################################

minimize congestionamento: Hmax;

solve;

printf '\n# Variaveis Bij\n';
printf{i in I, j in I}: "param Bij[%s,%s]:= %f;\n", i, j, Bij[i,j];
printf '\n# Variaveis Hijsd\n';
printf{i in I, j in I, s in I, d in I}: "param Hijsd[%s,%s,%s,%s]:= %f;\n", i, j, s, d, Hijsd[i,j,s,d];

end;
