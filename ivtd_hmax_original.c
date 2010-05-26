#include "toolkit.h"
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

/* Retorna 1 se localizar o enlace menos carregado e 
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
		//printf("Removeu enlace hmin[%d,%d]=%lf\n", hmin->lin, hmin->col, hmin->val);
	}
	return removeu;
}

/* Iterative Virtual Topology Design para Congestionamento */
int ivtd_hmax_original(char *modelo, char *dados)
{
	int tamRede, posB11, posH11, sair, iteracao, seccionou1avez;
	int posH1111;
	double valor, lowerBound, grauLogicoMedio, valorFinal, *vetHijsd;
	LPGW lp;
	MatSol topologia;
	MatTraf matTraf;
	Hij *vetHij;
	Hij hmin;
	FILE *foriginal, *freadiciona;
	char nomeforiginal[256], nomefreadiciona[256];
	
	sprintf(nomeforiginal, "%s.original", dados);
	foriginal = fopen(nomeforiginal, "w+");
	fprintf(foriginal, "hmax "); 
	sprintf(nomefreadiciona, "%s.readiciona", dados);
	freadiciona = fopen(nomefreadiciona, "w+");
	fprintf(freadiciona, "hmax "); 
	
	lowerBound = 0.0;
	grauLogicoMedio = 0;
	seccionou1avez = 1;
	valorFinal = 0;

	//Le modelo LP para minimizacao do congestionamento MinCong
	
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);

	// Identifica posicao inicial da sequencia de variaveis Bij e Hij dos problemas MinCong e Esp
	
	posB11 = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	posH11 = gwPosicaoVariavelLP(lp, "Hij[1,1]");
	posH1111 = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");

	// Funcoes de interpretacao de arquivos de dados associados ao modelo de programacao linear
	// Finalidade: carregar o tamanho da rede (N) na variavel tamRede
	matTraf = mtioCarregaMatTraf(dados);
	if(matTraf != NULL)
	{
		tamRede = matTraf->ordem;
	}
	else
	{	
		tamRede = mtioLocalizaParametroInt(dados, "N");
	}
	
	// Inicializa topologia totalmente conexa (iteracao inicial)
	topologia = msNewMatSol(tamRede);
	msTotalmenteConexa(topologia);
	
	sair = 0;
	iteracao = 0;
	
	vetHij = ivtdAlocaVetorHij(tamRede*tamRede); // aloca o vetor que guarda os valores de carregamento nos caminhos opticos
	
	vetHijsd = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));
	
	hmin = ivtdAlocaHij(); // aloca variavel que armazena o carregamento do enlace menos carregado (criterio de retirada de enlaces do ivtd)
	
	while(!sair)
	{
		/*
		printf("Iteracao: %d\n", iteracao);
		msImprimeMatSol(topologia);
		//*/
		//printf("atribui Bij_lp\n");
		hmaxAtribuiTopologiaBijLP(lp, topologia);
	
		//printf("resolve lp\n");
		valor = gwResolveLP(lp); // variavel valor armazena funcao objetivo otimizada
		//printf("Congestionamento da Topologia = %lf\n", valor);
		
		//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lp (COM componentes espurios)
		//ivtdAtualizaVetHij(lp, posH11, vetHij_Invalido, tamRede);
		ivtdAtualizaVetHij(lp, posH11, vetHij, tamRede);
	
		if(valor < 0) //indica que a topologia foi seccionada pois nao e possivel resolver o LP
		{
			//readiciona enlace
			msAdicionaEnlace(topologia, hmin->lin, hmin->col);
			//printf("Seccionou a rede e readicionou o enlace b[%d,%d]\n", hmin->lin, hmin->col);
			
			//indica a primeira vez que a topologia foi seccionada, que e o resultado de ivtd_hmax segundo Karcius D. R. Assis
			if(seccionou1avez)
			{
				//printf("Solucao do IVTD original:\nCongestionamento: %lf\nTopologia:\n", valorFinal);
				//msImprimeMatSol(topologia);
				seccionou1avez = 0;
			}
		}
		else 
		{
			// armazena ultimo valor valido para FO do modelo LP		
			valorFinal = valor;
			
			//printf("Iteracao: %d - Congestionamento: %lf - num.enlaces: %d", iteracao, valor, tamRede*(tamRede-1)-iteracao);
			//printf("%lf, ", valor);
			fprintf(freadiciona, "%lf ", valor);
			if(seccionou1avez)
			{
				fprintf(foriginal, "%lf ", valor);
			}
			/* Para imprimir o lower bound e o grau logico medio comente esta linha
			if(matTraf != NULL)
			{
				grauLogicoMedio = (tamRede-1) - (((double)iteracao)/tamRede);
				lowerBound = hmaxLBD(matTraf, rint(grauLogicoMedio));
				printf(" - LowerBound: %lf - grauLogicoMedio: %f", lowerBound, grauLogicoMedio);
			}
			//*/
			//printf("\n");
			
			iteracao++;
		}
		sair = !removeEnlaceMenosCarregado(topologia, vetHij, hmin);
	}
	
	//printf("Congestionamento: %lf\nTopologia final:\n", valorFinal);
	//msImprimeMatSol(topologia);
	fprintf(freadiciona, "\n");
	fprintf(foriginal, "\n");
	fclose(foriginal);
	fclose(freadiciona);
	
	ivtdLiberaVetorHij(tamRede*tamRede, vetHij);
	ivtdLiberaHij(hmin);
	if(matTraf != NULL)
	{
		mtDelMatTraf(matTraf);
	}
	msDelMatSol(topologia);
	gwFinalizaLP(lp);
	
	tkLiberaVetor((void*) vetHijsd);
		
	return 0;
}

/* Argumento de entrada obrigatorio: nome do arquivo de dados (matriz de trafego) */
int main(int argc, char **argv)
{
	char nomeArqModelo[256], nomeArqDados[256];
	int result;
	
	struct timeval t1, t2;
	float tempo;
	gettimeofday(&t1, NULL);
	
	if (argc == 3)
	{
		strcpy(nomeArqModelo, argv[1]);
		strcpy(nomeArqDados, argv[2]);
	}
	else
	{
		printf("Uso: %s <nomeArqModelo> <nomeArqDados>\n", argv[0]);
		return -1;
	}
	result = ivtd_hmax_original(nomeArqModelo, nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

