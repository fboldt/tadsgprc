#ifndef __GLPKWRAPPER_H_
#define __GLPKWRAPPER_H_
#include <glpk.h>

/* Codigo legado */
#define TK_LeModeloLP gwCarregaModeloLP
#define TK_SetaParametrosLP gwAtribuiParametrosLP
#define TK_FinalizaLP gwFinalizaLP
#define TK_CriaIndiceLP gwCriaIndiceLP
#define TK_PosicaoVariavelLP gwPosicaoVariavelLP

// Faz a leitura do problema e gera o modelo linear
// Inicializa estruturas de dados do LP, que eh um MILP,
// pois os valores (bounds) das variaveis inteiras ainda nao foram setados
LPX *gwCarregaModeloLP(char *modelo, char *dados, char *saida)
;

/* Atribui parametros default em modelo lp */
void gwAtribuiParametrosLP(LPX *lp)
;

// Finaliza (desaloca) estruturas de dados do LP
void gwFinalizaLP(LPX *lp)
;

// Cria o indice de nomes no modelo LP
void gwCriaIndiceLP(LPX *lp)
;

// Descobre a posicao de dada variavel no modelo LP
int gwPosicaoVariavelLP(char NomeVariavel[], LPX *lp)
;

/* Retorna o valor da funcao objetivo de um lp */
float gwValorLP(LPX *lp)
;


#endif


