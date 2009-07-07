#include "glpkwrapper.h"
#include "mattraf_io.h"
#include "matsol.h"
//#include "toolkit.h"
#include "milp_hmax_agg.c"
#include "ivtd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>


/* Retorn 1 se localizar o enlace menos carregado e 
atribui a linha e a coluna aos parametros linHmin e colHmin, respectivamente */
int localizaEnlaceMenosCarregado(int tamRede, Hij *vetHij_hmax,	int *linHmin, int *colHmin)
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
	if (achou)
	{
		k = indice_menor_vetHij_hmax;
		*linHmin = vetHij_hmax[k]->lin;
		*colHmin = vetHij_hmax[k]->col;
		vetHij_hmax[k]->blacklist = 1;
		//printf("\nEnlace menos carregado: H[%d,%d]=%lf\n", vetHij_hmax[k]->lin, vetHij_hmax[k]->col, vetHij_hmax[k]->val);
	}
	return achou;
}

/* Retorna 1 se remover o enlace menos carregado da topologia */
int removeEnlaceMenosCarregado(MatSol topologia, Hij *vetHij)
{
	int removeu, linHmin, colHmin, achou;
	removeu = 0;
	achou = localizaEnlaceMenosCarregado(topologia->ordem, vetHij, &linHmin, &colHmin);
	if(achou && topologia->qLin[linHmin] > 1 && topologia->qCol[colHmin] > 1)
	{
		msRemoveEnlace(topologia, linHmin, colHmin);
		removeu = 1;
	}
	//* Para habilitar o uso de black list, comente esta linha
	else
	{
		while(achou && (topologia->qLin[linHmin] <= 1 || topologia->qCol[colHmin] <= 1))
		{
			achou = localizaEnlaceMenosCarregado(topologia->ordem, vetHij, &linHmin, &colHmin);
		}
		if(achou)
		{
			msRemoveEnlace(topologia, linHmin, colHmin);
			removeu = 1;
		}
	}
	//*/
	return removeu;
}

/* Iterative Virtual Topology Design para Congestionamento */
int ivtd_hmax(char *modelo, char *dados)
{
	int tamRede, posB11, posH11, sair, iteracao;
	double valor;
	LPGW lp;
	MatSol topologia;
	Hij *vetHij;
	
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);
	
	posB11 = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	posH11 = gwPosicaoVariavelLP(lp, "Hij[1,1]");
	
	tamRede = mtioLocalizaParametroInt(dados, "N");
	topologia = msNewMatSol(tamRede);
	msTotalmenteConexa(topologia);
	
	sair = 0;
	iteracao = 1;
	
	vetHij = ivtdAlocaVetorHij(tamRede*tamRede);
	
	while(!sair)
	{
		//msImprimeMatSol(topologia);
		hmaxAtribuiTopologiaBijLP(lp, topologia);
		valor = gwValorLP(lp);
		printf("Iteracao: %d - Congestionamento: %lf\n", iteracao, valor);
		ivtdAtualizaVetHij(lp, posH11, vetHij, tamRede);
		
		sair = !removeEnlaceMenosCarregado(topologia, vetHij);
		iteracao++;
	}
	printf("Congestionamento: %lf\nTopologia final:\n", valor);
	msImprimeMatSol(topologia);
	
	ivtdLiberaVetorHij(tamRede*tamRede, vetHij);	
	msDelMatSol(topologia);
	gwFinalizaLP(lp);
	return 0;
}

/* Argumento de entrada obrigatorio: nome do arquivo de dados (matriz de trafego) */
int main(int argc, char **argv)
{
	char nomeArqDados[256];
	int result;
	
	struct timeval t1, t2;
	float tempo;
	gettimeofday(&t1, NULL);
	
	if (argc == 2)
	{
		strcpy(nomeArqDados, argv[1]);
	}
	else
	{
		printf("Uso: %s <nomeArqDados>\n", argv[0]);
		return -1;
	}
	result = ivtd_hmax("testes/milp_hmax_agg.mod", nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

