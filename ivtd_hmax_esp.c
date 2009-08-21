#include "glpkwrapper.h"
#include "matsol.h"
#include "mattraf.h"
#include "milp_hmax_agg.c"
#include "ivtd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>
#include <tgmath.h>
#include <glpk.h>

void atribuiCompTraf(LPGW lp, LPGW lpEsp, int tamRede)
{
	int i, k, posH1111, len, pos_restricao, pos_topologia, *ind;
	double *coef;
	//Encontrar a posicao da variavel Hijsd[1,1,1,1] em lp
	posH1111 = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");
	// Encontra a posicao da restricao compval[i,j,s,d] que define os componentes de tráfego válidos HVijsd
	pos_restricao = lpx_find_row(lpEsp, "compval[1,1,1,1]");
	// Descobre o numero de coeficientes nao-nulos da restricao compval[i,j,s,d] 
	len = lpx_get_mat_row(lpEsp, pos_restricao, NULL, NULL);
	//printf("len=%d\n", len);
	// Aloca memoria suficiente para armazenar indices das variaveis e valores dos coeficientes da restricao compval[i,j,s,d]
	ind = (int*)    malloc((len+1)*sizeof(int));
	coef = (double*) malloc((len+1)*sizeof(double));
	//Acha a posicao da variavel Bijsd em lpEsp, cujo coeficiente sera alterado
	pos_topologia = lpx_find_col(lpEsp, "Bijsd[1,1,1,1]");
	// Toma valores das variáveis Hijsd de lp e atualiza valor dos coeficientes Hijsd das variáveis Bijsd em lpEsp
	for (i=0; i<tamRede*tamRede*tamRede*tamRede; i++)
	{
		// Busca primeira linha da restricao compval[i,j,s,d] armazenando indices das variaveis e valores dos coeficientes
		len = lpx_get_mat_row(lpEsp, pos_restricao+i, ind, coef);
		// Busca posicao (k) do coeficiente Hijsd da variavel Bijsd
		k = 1;
		while (pos_topologia+i != ind[k])
			k++;

/*		if (gwValorVariavel(lp,posH1111+i))
		{
		printf("variavel selecionada em lpEsp cujo coeficiente sera alterado: %s\n", glp_get_col_name(lpEsp,ind[k]));
		printf("valor original do coeficiente em lpEsp: %lf\n", coef[k]);
		printf("valor do coeficiente retirando de lp: %lf\n", gwValorVariavel(lp, posH1111+i));
		}
*/
		coef[k] = -1*gwValorVariavel(lp, posH1111+i);
		lpx_set_mat_row (lpEsp, pos_restricao+i, len, ind, coef);

/*		if (gwValorVariavel(lp,posH1111+i))
		{
		lpx_get_mat_row (lpEsp, pos_restricao+i, ind, coef);
		k = 1;
		while (pos_topologia+i != ind[k])
			k++;
		printf(".................ATUALIZOU coeficiente em lpEsp\n");
		printf("variavel selecionada em lpEsp cujo coeficiente FOI alterado: %s\n", glp_get_col_name(lpEsp,ind[k]));
		printf("coeficiente da variável selecionada em lpEsp: %lf = %lf\n", coef[k], -1*gwValorVariavel(lp,posH1111+i));
		}
*/
	}
}

/* Retorn 1 se localizar o enlace menos carregado e 
atribui a linha e a coluna aos parametros linHmin e colHmin, respectivamente */
//int localizaEnlaceMenosCarregado(int tamRede, Hij *vetHij_hmax,	int *linHmin, int *colHmin)
int localizaEnlaceMenosCarregado(int tamRede, Hij vetHij_hmax[], Hij hmin)
{
	int k, achou, indice_menor_vetHij_hmax;
	double menor_vetHij_hmax;
	indice_menor_vetHij_hmax = -1;
	menor_vetHij_hmax  = FLT_MAX;
	for(k=0; k<(tamRede*tamRede); k++)
	{
		if (vetHij_hmax[k]->val > 0 && vetHij_hmax[k]->val < menor_vetHij_hmax && !vetHij_hmax[k]->blacklist)
		{
			indice_menor_vetHij_hmax = k;
			menor_vetHij_hmax = vetHij_hmax[k]->val;
		}
	}
	achou = (indice_menor_vetHij_hmax != -1);
	if(achou)
	{
		k = indice_menor_vetHij_hmax;
		hmin->lin = vetHij_hmax[k]->lin;
		hmin->col = vetHij_hmax[k]->col;
		hmin->blacklist = vetHij_hmax[k]->blacklist = 1;
		hmin->val = vetHij_hmax[k]->val;
		//printf("\nEnlace menos carregado: H[%d,%d]=%lf\n", vetHij_hmax[k]->lin, vetHij_hmax[k]->col, vetHij_hmax[k]->val);
	}
	return achou;
}

/* Retorna 1 se remover o enlace menos carregado da topologia */
int removeEnlaceMenosCarregado(MatSol topologia, Hij vetHij[], Hij hmin)
{
	int removeu, achou;
	removeu = 0;
	achou = localizaEnlaceMenosCarregado(topologia->ordem, vetHij, hmin);
	if(achou)
	{
		msDecrementaMatSol(topologia, hmin->lin, hmin->col);
		removeu = 1;
		printf("Removeu enlace hmin[%d,%d]=%lf\n", hmin->lin, hmin->col, hmin->val);
	}
	return removeu;
}

/* Iterative Virtual Topology Design para Congestionamento */
int ivtd_hmax(char *modelo, char *modeloEsp, char *dados)
{
	int tamRede, posB11, posH11, posB11_esp, posH11_esp, sair, iteracao, seccionou, posH1111, i;
	double valor, lowerBound, grauLogicoMedio, valorFinal;
	LPGW lp;
	LPGW lpEsp;
	MatSol topologia;
	MatTraf matTraf;
	Hij *vetHij;
	Hij hmin;
	
	lowerBound = 0.0f;
	grauLogicoMedio = 0;
	seccionou = 0;
	
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);

	lpEsp = gwCarregaModeloLP(modeloEsp, dados, "fooo.txt");
	gwAtribuiParametrosLP(lpEsp);
	gwCriaIndiceLP(lpEsp);
	
	posB11 = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	posH11 = gwPosicaoVariavelLP(lp, "Hij[1,1]");
	
	posB11_esp = gwPosicaoVariavelLP(lpEsp, "Bij[1,1]");
	posH11_esp = gwPosicaoVariavelLP(lpEsp, "Hij[1,1]");
	
	matTraf = mtioCarregaMatTraf(dados);
	if(matTraf != NULL)
	{
		tamRede = matTraf->ordem;
	}
	else
	{	
		tamRede = mtioLocalizaParametroInt(dados, "N");
	}
	topologia = msNewMatSol(tamRede);
	msTotalmenteConexa(topologia);
	
	sair = 0;
	iteracao = 0;
	
	vetHij = ivtdAlocaVetorHij(tamRede*tamRede);
	hmin = ivtdAlocaHij();
	
	while(!sair)
	{
		msImprimeMatSol(topologia);
		hmaxAtribuiTopologiaBijLP(lp, topologia);
		hmaxAtribuiTopologiaBijLP(lpEsp, topologia);

		valor = gwValorLP(lp);

/*    posH1111 = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");
	for(i=0; i<tamRede*tamRede*tamRede*tamRede; i++)
	{
	if (gwValorVariavel(lp,posH1111+i))
		printf("%s=%lf ",glp_get_col_name(lp,posH1111+i),gwValorVariavel(lp,posH1111+i));
	}
*/
		atribuiCompTraf(lp, lpEsp, tamRede);
		gwValorLP(lpEsp);
		
		if(valor < 0)
		{
			//readiciona enlace
			msAdicionaEnlace(topologia, hmin->lin, hmin->col);
			printf("Seccionou a rede e readicionou o enlace b[%d,%d]\n", hmin->lin, hmin->col);
			if(!seccionou)
			{
				printf("Solucao do IVTD original:\nCongestionamento: %lf\nTopologia:\n", valorFinal);
				msImprimeMatSol(topologia);
				seccionou = 1;
			}
		}
		else
		{
			valorFinal = valor;
			printf("Iteracao: %d - Congestionamento: %lf - num.enlaces: %d", iteracao, valor, tamRede*(tamRede-1)-iteracao);
			/* Para imprimir o lower bound e o grau logico medio comente esta linha
			if(matTraf != NULL)
			{
				grauLogicoMedio = (tamRede-1) - (((double)iteracao)/tamRede);
				lowerBound = hmaxLBD(matTraf, rint(grauLogicoMedio));
				printf(" - LowerBound: %lf - grauLogicoMedio: %f", lowerBound, grauLogicoMedio);
			}
			//*/
			printf("\n");
			iteracao++;
			
			ivtdAtualizaVetHij(lpEsp, posH11_esp, vetHij, tamRede);
			//seccionou = 0;
		}
		
		sair = !removeEnlaceMenosCarregado(topologia, vetHij, hmin);
	}
	printf("Congestionamento: %lf\nTopologia final:\n", valorFinal);
	msImprimeMatSol(topologia);
	
	ivtdLiberaVetorHij(tamRede*tamRede, vetHij);
	ivtdLiberaHij(hmin);
	if(matTraf != NULL)
	{
		mtDelMatTraf(matTraf);
	}
	msDelMatSol(topologia);
	gwFinalizaLP(lp);
	gwFinalizaLP(lpEsp);
	return 0;
}

/* Argumento de entrada obrigatorio: nome do arquivo de dados (matriz de trafego) */
int main(int argc, char **argv)
{
	char nomeArqModelo[256], nomeArqmodeloEsp[256], nomeArqDados[256];
	int result;
	
	struct timeval t1, t2;
	float tempo;
	gettimeofday(&t1, NULL);
	
	if (argc == 4)
	{
		strcpy(nomeArqModelo, argv[1]);
		strcpy(nomeArqmodeloEsp, argv[2]);
		strcpy(nomeArqDados, argv[3]);
	}
	else
	{
		printf("Uso: %s <nomeArqModelo> <nomeArqmodeloEsp> <nomeArqDados>\n", argv[0]);
		return -1;
	}
	result = ivtd_hmax(nomeArqModelo, nomeArqmodeloEsp, nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

