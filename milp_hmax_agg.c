#ifndef __MILP_HMAX_AGG_C_
#define __MILP_HMAX_AGG_C_

#include "glpkwrapper.h"
#include "matsol.h"

/* Atribui a topologia de ms ao lp */
void hmaxAtribuiTopologiaBijLP(LPGW lp, MatSol ms)
{
	int posBij, tam, k, lin, col;
	posBij = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	tam = ms->ordem*ms->ordem;
	for(k=0; k<tam; k++)
	{
		lin = k/tam;
		col = k%tam;
		gwAtribuiValorPosicaoLP(lp, (k+posBij), ms->b[lin][col]);
	}
}

#endif

