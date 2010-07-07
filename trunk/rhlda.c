#include "toolkit.h"
#include "glpkwrapper.h"
#include "matsol.h"
#include "mattraf.h"
#include "milp.c"
#include "ivtd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <sys/time.h>
#include <tgmath.h>
#include <glpk.h>

double valorEnlaceMaisCarregado(int tamRede, Hij vetHij_hmax[])
{
	int k, indice_maior;
	double maior;
	indice_maior = -1;
	maior = -FLT_MAX;
	for(k=0; k<(tamRede*tamRede); k++)
	{
		if (vetHij_hmax[k]->val > maior)
		{
			indice_maior = k;
			maior = vetHij_hmax[k]->val;
		}
	}
	return maior;
}

int localizaMenorDemanda(MatSol topologia, MatSol retirados, MatTraf matTraf, Hij hmin)
{
	int i, j;
	double menor;
	menor = FLT_MAX;
	for(i = 0; i < matTraf->ordem; i++)
	{
		for(j = 0; j < matTraf->ordem; j++)
		{
			if(matTraf->h[i][j] < menor && retirados->b[i][j] > 0 && topologia->qLin[i] > 1 && topologia->qCol[j] > 1)
			{
				menor = matTraf->h[i][j];
				hmin->lin = i;
				hmin->col = j;
			}
		}
	}	
	return menor != FLT_MAX;
}

int removeEnlaceMenorDemanda(MatSol topologia, MatSol retirados, MatTraf matTraf, Hij hmin)
{
	int removeu, achou;
	removeu = 0;
	achou = localizaMenorDemanda(topologia, retirados, matTraf, hmin);
	if(achou)
	{
		msDecrementaMatSol(topologia, hmin->lin, hmin->col);
		msDecrementaMatSol(retirados, hmin->lin, hmin->col);
		removeu = 1;
		printf("Removeu enlace h[%d,%d]\n", hmin->lin, hmin->col);
	}
	return removeu;
}

int rhlda(char *modelo, char *dados)
{
	int tamRede, posB11, posH11, sair, iteracao, seccionou1avez;
	int posH1111;
	double valor, lowerBound, grauLogicoMedio, valorFinal, *vetHijsd;
	LPGW lp;
	MatSol topologia, retirados;
	MatTraf matTraf;
	Hij *vetHij;
	Hij hmin;
	FILE *foriginal, *freadiciona, *fhmax;
	char nomeforiginal[256], nomefreadiciona[256], nomefhmax[256];
	
	sprintf(nomeforiginal, "%s.original", dados);
	foriginal = fopen(nomeforiginal, "w+");
	fprintf(foriginal, "%s ", modelo); 
	
	sprintf(nomefreadiciona, "%s.readiciona", dados);
	freadiciona = fopen(nomefreadiciona, "w+");
	fprintf(freadiciona, "%s ", modelo); 
	
	sprintf(nomefhmax, "%s.hmax", dados);
	fhmax = fopen(nomefhmax, "w+");
	fprintf(fhmax, "hmax "); 
	
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

	retirados = msNewMatSol(tamRede);
	msTotalmenteConexa(retirados);	

	sair = 0;
	iteracao = 0;
	
	vetHij = ivtdAlocaVetorHij(tamRede*tamRede); // aloca o vetor que guarda os valores de carregamento nos caminhos opticos
	
	vetHijsd = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));
	
	hmin = ivtdAlocaHij(); // aloca variavel que armazena o carregamento do enlace menos carregado (criterio de retirada de enlaces do ivtd)
	
	while(!sair)
	{
		/*
		printf("topologia: %d\n", iteracao);
		msImprimeMatSol(topologia);
		printf("retirados: %d\n", iteracao);
		msImprimeMatSol(retirados);
		
		//*/
		//printf("atribui Bij_lp\n");
		milpAtribuiTopologiaBijLP(lp, topologia);
	
		//printf("resolve lp\n");
		valor = gwResolveLP(lp); // variavel valor armazena funcao objetivo otimizada
		//printf("Processamento = %.60lf\n", valor);
		
		//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lp (COM componentes espurios)
		ivtdAtualizaVetHij(lp, posH11, vetHij, tamRede);
	
		if(valor == -1) //indica que a topologia foi seccionada pois nao e possivel resolver o LP
		{
			//readiciona enlace
			msAdicionaEnlace(topologia, hmin->lin, hmin->col);
			//printf("Seccionou a rede e readicionou o enlace b[%d,%d]\n", hmin->lin, hmin->col);
			
			//indica a primeira vez que a topologia foi seccionada, que e o resultado de ivtd_hmax segundo Karcius D. R. Assis
			if(seccionou1avez)
			{
				//printf("Solucao do IVTD original:\nProcessamento: %lf\nTopologia:\n", valorFinal);
				//msImprimeMatSol(topologia);
				seccionou1avez = 0;
			}
		}
		else 
		{
			// armazena ultimo valor valido para FO do modelo LP		
			valorFinal = valor;
			fprintf(fhmax, "%lf ", valorEnlaceMaisCarregado(tamRede, vetHij));
			
			printf("Iteracao: %d - Processamento: %lf - num.enlaces: %d\n", iteracao, valor, tamRede*(tamRede-1)-iteracao);
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
		sair = !removeEnlaceMenorDemanda(topologia, retirados, matTraf, hmin);
	}
	
	//printf("Processamento: %lf\nTopologia final:\n", valorFinal);
	//msImprimeMatSol(topologia);
	fprintf(freadiciona, "\n");
	fclose(freadiciona);
	fprintf(foriginal, "\n");
	fclose(foriginal);
	fprintf(fhmax, "\n");
	fclose(fhmax);
	
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
	result = rhlda(nomeArqModelo, nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

