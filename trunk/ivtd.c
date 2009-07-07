#include "ivtd.h"
#include "glpkwrapper.h"
#include <stdlib.h>

/* Aloca um estrutura Hij */
Hij ivtdAlocaHij()
{
	Hij hij;
	hij = (Hij) malloc(sizeof(struct hij));
	hij->blacklist = 0;
	hij->lin = 0;
	hij->col = 0;
	hij->val = 0;
	return (hij);
}

/* Aloca um vetor de estruturas Hij */
Hij *ivtdAlocaVetorHij(int n)
{
	int i;
	Hij *vetHij;
	vetHij = (Hij*) malloc(n*sizeof(Hij));
	for (i=0; i<n; i++)
	{
		vetHij[i] = ivtdAlocaHij();
	}
	return vetHij;
}

/* Libera uma estrutura Hij */
void ivtdLiberaHij(Hij hij)
{
	free(hij);
}

/* Libera um vetor de estruturas Hij */
void ivtdLiberaVetorHij(int n, Hij *vetHij)
{
	int i;
	for (i=0; i<n; i++)
	{
		ivtdLiberaHij(vetHij[i]);
	}
	free(vetHij);
}

/* Atualiza os valores e coordenadas de H com os valores das variáveis H[i,j] retornados pelo GLPK */
void ivtdAtualizaVetHij(LPGW lp, int posH11, Hij *vetHij, int tamRede)
{
	int k, pos;
	for(k=0; k<(tamRede*tamRede); k++)
	{
		pos = k + posH11;
		vetHij[k]->val = gwValorVariavel(lp, pos);
		vetHij[k]->lin = (k/tamRede);
		vetHij[k]->col = (k%tamRede);
	}
}

