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

//Atualiza o valor da variavel Hijsd de lp no coeficiente Hijsd*Bijsd das restricoes compval de lpEsp
void atribuiCompTraf(LPGW lp, LPGW lpEsp, int tamRede, double* vetHijsd_esp)
{
	int i, j, m, posHijsd, len, pos_restricao, pos_topologia, *ind;
	double *coef;
	
	//Encontrar a posicao da variavel Hijsd[1,1,1,1] em lp
	posHijsd = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");
	// Encontra a posicao da restricao compval[1,1,1,1] que define os componentes de tráfego válidos HVijsd
	pos_restricao = lpx_find_row(lpEsp, "compval[1,1,1,1]");
	
	// Aloca vetores para lpEsp
	ind = (int*) malloc((3)*sizeof(int));
	coef = (double*) malloc((3)*sizeof(double));
	
	//Acha a posicao da variavel Bijsd[1,1,1,1] em lpEsp, cujo coeficiente sera alterado
	pos_topologia = gwPosicaoVariavelLP(lpEsp, "Bijsd[1,1,1,1]");
	
	//***Toma valores das variáveis Hijsd de lp e atualiza valor dos coeficientes Hijsd das variáveis Bijsd em lpEsp:***//
	for(i=0; i<tamRede*tamRede*tamRede*tamRede; i++)
	{
		for(j=0; j<3; j++)
		{
			ind[j] = 0;
			coef[j] = 0.0;
		}
		
		// Busca linha da restricao compval[i,j,s,d] armazenando indices das variaveis e valores dos coeficientes
		len = lpx_get_mat_row(lpEsp, pos_restricao+i, ind, coef);
		
		// Busca posicao (m) do coeficiente Hijsd da variavel Bijsd em lpEsp
		m = 1;
		while (m<len && pos_topologia+i != ind[m])
		{
			m++;
		}
		
		coef[m] = gwValorVariavel(lp, posHijsd+i);
		vetHijsd_esp[i] = coef[m];
		coef[m] *= -1;
		lpx_set_mat_row(lpEsp, pos_restricao+i, len, ind, coef);
	}
		
	// Libera vetores ind e coef
	if(ind != NULL)
		free(ind);
	if(coef != NULL)
		free(coef);	
}


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
		printf("Removeu enlace hmin[%d,%d]=%lf\n", hmin->lin, hmin->col, hmin->val);
	}
	return removeu;
}

/* Iterative Virtual Topology Design para Congestionamento */
int ivtd_hmax_esp(char *modelo, char *modeloEsp, char *dados)
{
	int tamRede, posB11, posH11, posB11_esp, posH11_esp, sair, iteracao, seccionou1avez;
	int i, j, k, l, m, posB1111_esp, posHV1111_esp, posH1111;
	double valor, lowerBound, grauLogicoMedio, valorFinal, *vetHijsd, *vetHijsd_esp, *vetHVijsd, *vetBijsd;
	LPGW lp;
	LPGW lpEsp;
	MatSol topologia;
	MatTraf matTraf;
	Hij *vetHij;
	Hij *vetHij_Invalido;
	Hij hmin;
	
	lowerBound = 0.0;
	grauLogicoMedio = 0;
	seccionou1avez = 1;
	valorFinal = 0;

	//Le modelo LP para minimizacao do congestionamento MinCong
	
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);

	//Le modelo LP para eliminacao de componentes espurios (loops) Esp

	lpEsp = gwCarregaModeloLP(modeloEsp, dados, "fooo.txt");
	gwAtribuiParametrosLP(lpEsp);
	gwCriaIndiceLP(lpEsp);

	// Identifica posicao inicial da sequencia de variaveis Bij e Hij dos problemas MinCong e Esp
	
	posB11 = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	posH11 = gwPosicaoVariavelLP(lp, "Hij[1,1]");
	posH1111 = gwPosicaoVariavelLP(lp, "Hijsd[1,1,1,1]");

	posB11_esp = gwPosicaoVariavelLP(lpEsp, "Bij[1,1]");
	posH11_esp = gwPosicaoVariavelLP(lpEsp, "Hij[1,1]");
	posHV1111_esp = gwPosicaoVariavelLP(lpEsp, "HVijsd[1,1,1,1]");
	posB1111_esp = gwPosicaoVariavelLP(lpEsp, "Bijsd[1,1,1,1]");
	
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
	vetHij_Invalido = ivtdAlocaVetorHij(tamRede*tamRede); // aloca o vetor que guarda os valores de carregamento nos caminhos opticos
	
	vetHijsd = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));
	vetHijsd_esp = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));
	vetHVijsd = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));
	vetBijsd = (double*) tkAlocaVetor(tamRede*tamRede*tamRede*tamRede, sizeof(double));

	hmin = ivtdAlocaHij(); // aloca variavel que armazena o carregamento do enlace menos carregado (criterio de retirada de enlaces do ivtd)
	
	while(!sair)
	{
		printf("Iteracao: %d\n", iteracao);
		msImprimeMatSol(topologia);
		printf("atribui Bij_lp\n");
		hmaxAtribuiTopologiaBijLP(lp, topologia);
		printf("atribui Bij_lpEsp\n");
		hmaxAtribuiTopologiaBijLP(lpEsp, topologia);

		printf("resolve lp\n");
		valor = gwResolveLP(lp); // variavel valor armazena funcao objetivo otimizada
		printf("Congestionamento da Topologia = %lf\n", valor);
		
		//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lp (COM componentes espurios)
		ivtdAtualizaVetHij(lp, posH11, vetHij_Invalido, tamRede);
		//ivtdAtualizaVetHij(lp, posH11, vetHij, tamRede);
	
		if(valor < 0) //indica que a topologia foi seccionada pois nao e possivel resolver o LP
		{
			//readiciona enlace
			msAdicionaEnlace(topologia, hmin->lin, hmin->col);
			printf("Seccionou a rede e readicionou o enlace b[%d,%d]\n", hmin->lin, hmin->col);
			
			//indica a primeira vez que a topologia foi seccionada, que e o resultado de ivtd_hmax segundo Karcius D. R. Assis
			if(seccionou1avez)
			{
				printf("Solucao do IVTD original:\nCongestionamento: %lf\nTopologia:\n", valorFinal);
				msImprimeMatSol(topologia);
				seccionou1avez = 0;
			}
		}
		else  // indica que a solucao para o roteamento de trafego foi obtida passando-se para a eliminacao dos comp. espurios
		{
			// armazena ultimo valor valido para FO do modelo LP		
			valorFinal = valor;
			
			//* ************ para nao retirar espurios, descomentar esta linha
			
			// elimina componentes espurios
			printf("Atribuindo componentes de trafego do model MinCong ao modelo Esp...\n");
			atribuiCompTraf(lp, lpEsp, tamRede, vetHijsd_esp);
			
			/*
			char filelp[32];
			sprintf(filelp, "lps/lpesp%d.lp", iteracao);
			glp_write_lp(lpEsp, NULL, filelp);			
			//*/
			
			printf("Resolvendo lpEsp para retirar componentes espurios...\n");
			gwResolveLP(lpEsp);
			
			//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lpEsp (sem componentes espurios)
			ivtdAtualizaVetHij(lpEsp, posH11_esp, vetHij, tamRede);
			
			//*
			for (i=0; i<tamRede*tamRede*tamRede*tamRede; i++)
			{
				vetHijsd[i] = gwValorVariavel(lp, posH1111+i);
				vetHVijsd[i] = gwValorVariavel(lpEsp, posHV1111_esp+i);
				vetBijsd[i] = gwValorVariavel(lpEsp, posB1111_esp+i);
			}
			
			m=0;
			for (k=0; k<tamRede; k++)
					{
						for (l=0; l<tamRede; l++)
						{
			for (i=0; i<tamRede; i++)
			{
				for (j=0; j<tamRede; j++)
				{
							printf("[%d,%d,%d,%d]: Hijsd = %6.3lf \t Hijsd_esp = %6.3lf \t HVijsd = %6.3lf \t Bijsd = %3.1lf %c \n", 
								i+1, j+1, k+1, l+1, ceil(vetHijsd[m]*1000)/1000,ceil(vetHijsd_esp[m]*1000)/1000,ceil(vetHVijsd[m]*1000)/1000,vetBijsd[m], 
								ceil(vetHVijsd[m]*1000)<ceil(vetHijsd_esp[m]*1000)?'*':ceil(vetHVijsd[m]*1000)>ceil(vetHijsd_esp[m]*1000)?'X':' ');
							m++;
						}
					}
				}
			}

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
			
			printf("Vetor Hij INVALIDO:\n");
			for (i=0; i<tamRede*tamRede; i++)
			{
				printf("%7.2lf%c", vetHij_Invalido[i]->val, (i+1)%tamRede==0?'\n':' ');
			}
			printf("*****************************\n");
			
			printf("Vetor Hij VALIDO:\n");
			for (i=0; i<tamRede*tamRede; i++)
			{
				printf("%7.2lf%c", vetHij[i]->val, (i+1)%tamRede==0?'\n':' ');
			}
			printf("*****************************\n");

			printf("Vetor Hij INVALIDO-VALIDO:\n");
			for (i=0; i<tamRede*tamRede; i++)
			{
				printf("%7.2lf%c", vetHij_Invalido[i]->val-vetHij[i]->val, (i+1)%tamRede==0?'\n':' ');
			}
			printf("*****************************\n");
			
			//*/
			iteracao++;
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
	
	tkLiberaVetor((void*) vetHijsd_esp);
	tkLiberaVetor((void*) vetHijsd);
	tkLiberaVetor((void*) vetHVijsd);
	tkLiberaVetor((void*) vetBijsd);
		
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
	result = ivtd_hmax_esp(nomeArqModelo, nomeArqmodeloEsp, nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

