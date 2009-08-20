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

void hmaxAtribuiCompTraf(LPGW lp, LPGW lpEsp, int tamRede)
{
// refazer esta funcao cm base no procedimento de mudanca de coeficiente de variavel do lbi
//no lbi, o hmax e adicionado como coeficiente nas restricoes do plano de corte
/*	int posH1111, posH1111_esp, tam, k;
	posH1111 = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");
	posH1111_esp = gwPosicaoVariavelLP(lpEsp, "Hijsd[1,1,1,1]");
	tam = tamRede*tamRede*tamRede*tamRede;
	for(k=0; k<tam; k++)
	{
		gwAtribuiValorPosicaoLP(lpEsp, (k+posH1111_esp), gwValorVariavel(lp, (k+posH1111)));
	}
*/
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
	int tamRede, posB11, posH11, posB11_esp, posH11_esp, sair, iteracao, seccionou;
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
		//msImprimeMatSol(topologia);
		hmaxAtribuiTopologiaBijLP(lp, topologia);
		hmaxAtribuiTopologiaBijLP(lpEsp, topologia);

		valor = gwValorLP(lp);

		hmaxAtribuiCompTraf(lp, lpEsp, tamRede);
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

