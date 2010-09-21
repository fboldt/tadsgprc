/* Formulação do Modelo para LBi com Grau Lógico Assimétrico e Limitação do Número de Transceptores

   Descrição do modelo em MathProg por Renato T. R. Almeida (renatotralmeida@gmail.com).
*/

#############################################################
########################### PARÂMETROS ######################
#############################################################

# 1 - Numero de nós da rede 
param N, integer, > 0;

# 2 - Conjunto de indices
set I := 1..N;

# 3 - Matriz de Trafego
param mattraf{s in I, d in I};

# 4 - Somas do tráfego partindo de cada nó
param hs{s in I}  :=  sum{d in I: d!=s} mattraf[s,d];

# 5 - Plano de Corte inicial (MFT bound, por exemplo)
param HmaxL;


#############################################################
########################### VARIÁVEIS #######################
#############################################################

# 1 - Matriz da topologia virtual
var  B 'topologia' {i in I, j in I: i!=j}, >=0, <=1;

# 3 - Parcela de tráfego vindo de "s" passando pelo enlace (i,j)
var h{ i in I, j in I, s in I: i!=j and j!=s} >= 0;

# 4 - trafego transportado atraves do caminho optico que interliga os nós (i,j)
var H{i in I, j in I: i!=j} >= 0;

# 5 - Congestionamento
var Hmax >=0;

# 6 - Numero de transceptores
var ntransc >=0;

#############################################################
########################### RESTRIÇÕES ######################
#############################################################

# 1 - Restriçao de limitaçao de fluxo com plano de corte
/* (1.0)*Hmax -(1.0)*H[i,j] + (HmaxL)*B[i,j] >= HmaxL */
s.t.  limit_cutpl{i in I, j in I: i!=j}: Hmax >= H[i,j] + HmaxL*(1 - B[i,j]);

# 2 - Restriçao ao numero de transceptores
s.t.  limit_ntransc: sum{i in I, j in I: i!=j} B[i,j] <= ntransc;

# 3 - Restriçao de limitaçao de fluxo tipo 2
s.t.  limit2{i in I, j in I, s in I: i!=j and j!=s}: h[i,j,s] <= (B[i,j] * hs[s]);

# 4 - Restriçao de conservaçao de trafego tipo 1
s.t.  conserv1{i in I, j in I: i!=j}: H[i,j] = sum{s in I: j!=s} h[i,j,s];

# 5 - Restriçao de conservaçao de trafego tipo 2
s.t.  conserv2{i in I, s in I}: 
      sum{j in I: i!=j and j!=s} h[i,j,s] - sum{j in I: i!=j and i!=s} h[j,i,s] =
	if (s!=i) then -mattraf[s,i] else hs[s];


#############################################################
######################## FUNÇÃO OBJETIVO ####################
#############################################################

minimize congestionamento: Hmax;

end;
