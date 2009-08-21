#ifndef __MILP_HMAX_AGG_C_
#define __MILP_HMAX_AGG_C_

#include "glpkwrapper.h"
#include "matsol.h"
#include "mattraf.h"

/* Atribui a topologia de ms ao lp */
void hmaxAtribuiTopologiaBijLP(LPGW lp, MatSol ms)
{
	int posBij, tam, k, lin, col;
	posBij = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	tam = ms->ordem*ms->ordem;
	for(k=0; k<tam; k++)
	{
		lin = k/ms->ordem;
		col = k%ms->ordem;
		gwAtribuiValorPosicaoLP(lp, (k+posBij), ms->b[lin][col]);
	}
}

/* Lower Bound Deterministico Hmax Ramaswami */
float hmaxLBD(MatTraf mt, int grauLogico)
{
	int i, j;
	float result, maxLin, maxCol;
	maxLin = maxCol = result = 0.0f;
	
	for(i=0; i<mt->ordem; i++)
	{
		for(j=0; j<mt->ordem; j++)
		{
			maxLin += mt->h[i][j];
			maxCol += mt->h[j][i];
		}
		result = result<maxLin?maxLin:result;
		result = result<maxCol?maxCol:result;
		maxLin = maxCol = 0;
	}
	
	return result/grauLogico;
}

#endif

