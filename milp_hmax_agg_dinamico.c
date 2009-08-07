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
		lin = k/tam;
		col = k%tam;
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

/* Atribui valores de demandas de trafego nao agregadas (MatTraf(s,d)) */
void hmaxAtribuiDemandasLP(LPGW lp, MatTraf mt)
{
// Determina posicao das restricoes do tipo "conserv2{i=1, s=1}"
// Atribui valor de MatTraf{s,d=i} a restricao "conserv2{i,s}"
}

/* Atribui valores de demandas de tráfego agregadas (Hs(s)) */
void hmaxAtribuiDemandasAgregadasLP(LPGW lp, MatTraf mt)
{
// Calcula demandas agregadas Hs(s) = sum(d){MatTraf(s,d)}

// Determina posicao das restricoes do tipo "limit2{i=1, j=1, s=1}"
// Recupera valores do vetor de coeficientes da restricao "limit2{i,j,s}"
// Localiza o coeficiente da variavel Bij
// Atribui valor de Hs(s) ao coeficiente da variavel Bij

// Determina posicao das restricoes do tipo "conserv3{i=1, s=1}"
// Atribui valor de Hs(s) a todas as restricoes "conserv3{i,s}" qualquer que seja i

}


#endif

