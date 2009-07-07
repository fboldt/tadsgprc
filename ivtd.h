#ifndef __IVTD_H_
#define __IVTD_H_
#include "glpkwrapper.h"

typedef struct hij
{
	int lin;
	int col;
	int blacklist;
	double val;
} *Hij;

/* ##################### */

/* Aloca um estrutura Hij */
Hij ivtdAlocaHij()
;

/* Aloca um vetor de estruturas Hij */
Hij *ivtdAlocaVetorHij(int n)
;

/* Libera uma estrutura Hij */
void ivtdLiberaHij(Hij hij)
;

/* Libera um vetor de estruturas Hij */
void ivtdLiberaVetorHij(int n, Hij *vetHij)
;

/* Atualiza os valores e coordenadas de H com os valores das variáveis H[i,j] retornados pelo GLPK */
void ivtdAtualizaVetHij(LPGW lp, int posH11, Hij *vetHij, int tamRede)
;

#endif

