#ifndef __GLPKWRAPPER_H_
#define __GLPKWRAPPER_H_
#include <glpk.h>

/* Codigo legado */
#define TK_LeModeloLP gwCarregaModeloLP
#define TK_SetaParametrosLP gwAtribuiParametrosLP
#define TK_FinalizaLP gwFinalizaLP
#define TK_CriaIndiceLP gwCriaIndiceLP
#define TK_PosicaoVariavelLP gwPosicaoVariavelLP
#define gwAtribuiParametrosLP gwConfiguraGLPK
#define gwValorLP gwResolveLP

#define LPGW LPX*

/* Fixa o valor a uma variavel inteira atribuindo esse valor ao upper bond e ao lower bound */
void gwAtribuiValorPosicaoLP(LPGW lp, int pos, int valor)
;

// Faz a leitura do problema e gera o modelo linear
// Inicializa estruturas de dados do LP, que eh um MILP,
// pois os valores (bounds) das variaveis inteiras ainda nao foram setados
LPGW gwCarregaModeloLP(char *modelo, char *dados, char *saida)
;

/* Atribui parametros default em modelo lp */
void gwConfiguraGLPK(LPGW lp)
;

// Finaliza (desaloca) estruturas de dados do LP
void gwFinalizaLP(LPGW lp)
;

// Cria o indice de nomes no modelo LP
void gwCriaIndiceLP(LPGW lp)
;

// Descobre a posicao de dada variavel no modelo LP
int gwPosicaoVariavelLP(LPGW lp, char *NomeVariavel)
;

/* Retorna o valor da funcao objetivo de um lp */
double gwResolveLP(LPGW lp)
;

/* Retorna o valor de uma variavel em um lp em determinada posicao */
double gwValorVariavel(LPGW lp, int posicao)
;

/* Retorna o nome de uma variavel em um lp em determinada posicao */
const char *gwNomeVariavel(LPGW lp, int posicao)
;

#endif


