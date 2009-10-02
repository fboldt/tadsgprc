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
	int i, j, k, l, m, posHijsd, len, pos_restricao, pos_topologia, *ind;
	double *coef;
	char str[64], strB[64], strH[64];
	
//Atualiza o valor da variavel Hijsd de lp no coeficiente Hijsd*Bijsd das restricoes compval de lpEsp
	for (i=0; i<tamRede; i++)
		for (j=0; j<tamRede; j++)
			for (k=0; k<tamRede; k++)
				for (l=0; l<tamRede; l++)
	{
	//Encontrar a posicao da variavel Hijsd em lp
	sprintf(strH, "Hijsd[%d,%d,%d,%d]", i+1, j+1, k+1, l+1);
	posHijsd = gwPosicaoVariavelLP(lp, strH);
//	if (gwValorVariavel(lp,posHijsd))
//		printf("Posicao da variavel %s em lp = %d\n", strH, posHijsd);

	// Encontra a posicao da restricao compval[i,j,s,d] que define os componentes de tráfego válidos HVijsd
	sprintf(str, "compval[%d,%d,%d,%d]", i+1, j+1, k+1, l+1);
	pos_restricao = lpx_find_row(lpEsp, str);
//	if (gwValorVariavel(lp,posHijsd))
//		printf("Posicao da restricao %s em lpEsp = %d\n", str, pos_restricao);

	// Descobre o numero máximo de coeficientes nao-nulos da restricao compval[i,j,s,d] 
	len = lpx_get_mat_row(lpEsp, pos_restricao, NULL, NULL);
//	if (gwValorVariavel(lp,posHijsd))
//		printf("Numero de coeficientes nao-nulos da restricao %s = %d\n", str, len);
	// Aloca memoria suficiente para armazenar indices das variaveis e valores dos coeficientes da restricao compval[i,j,s,d]
	ind = (int*)    malloc((len+1)*sizeof(int));
	coef = (double*) malloc((len+1)*sizeof(double));

	//Acha a posicao da variavel Bijsd[1,1,1,1] em lpEsp, cujo coeficiente sera alterado
	sprintf(strB, "Bijsd[%d,%d,%d,%d]", i+1, j+1, k+1, l+1);
	pos_topologia = lpx_find_col(lpEsp, strB);
//	if (gwValorVariavel(lp,posHijsd))
//		printf("Posicao da variavel %s em lpEsp = %d\n", strB, pos_topologia);

	//***Toma valores das variáveis Hijsd de lp e atualiza valor dos coeficientes Hijsd das variáveis Bijsd em lpEsp:***//

	// Busca linha da restricao compval[i,j,s,d] armazenando indices das variaveis e valores dos coeficientes
	len = lpx_get_mat_row(lpEsp, pos_restricao, ind, coef); //!!!!!!!!!!!!! ISTO ESTA DANDO PAUUUUUU!!!! PORRA!!!!!
//	if (gwValorVariavel(lp,posHijsd))
//		printf("%lf x %s %lf x %s = 0\n", coef[1], glp_get_col_name(lpEsp,ind[1]), coef[2], glp_get_col_name(lpEsp,ind[2]));
	// Busca posicao (m) do coeficiente Hijsd da variavel Bijsd
	m = 1;
	while (pos_topologia != ind[m])
	{
			m++;
	}

/*		if (gwValorVariavel(lp,posHijsd))
		{
		printf("variavel selecionada em lpEsp cujo coeficiente sera alterado: %s\n", glp_get_col_name(lpEsp,ind[m]));
		printf("valor original do coeficiente em lpEsp: %lf\n", coef[m]);
		printf("valor do coeficiente retirando de lp: %lf\n", gwValorVariavel(lp, posHijsd));
		}
*/
		coef[m] = -1*gwValorVariavel(lp, posHijsd);
		lpx_set_mat_row (lpEsp, pos_restricao, len, ind, coef);

/*		if (gwValorVariavel(lp,posHijsd))
		{
		lpx_get_mat_row (lpEsp, pos_restricao, ind, coef);
		printf(".................ATUALIZOU coeficiente em lpEsp\n");
		len = lpx_get_mat_row(lpEsp, pos_restricao, ind, coef);
		printf("%lf x %s %lf x %s = 0\n", coef[1], glp_get_col_name(lpEsp,ind[1]), coef[2], glp_get_col_name(lpEsp,ind[2]));
		}
	printf("*********************************************************\n");
*/
//	if(ind != NULL)
//		free(ind);
//	if(coef != NULL)
//		free(coef);
	}
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
	int tamRede, posB11, posH11, posB11_esp, posH11_esp, sair, iteracao, seccionou1avez, i;
	double valor, lowerBound, grauLogicoMedio, valorFinal;
	LPGW lp;
	LPGW lpEsp;
	MatSol topologia;
	MatTraf matTraf;
	Hij *vetHij;
	Hij *vetHij_Invalido;
	Hij hmin;
	
	lowerBound = 0.0f;
	grauLogicoMedio = 0;
	seccionou1avez = 0;

	//Le modelo LP para minimizacao do congestionamento MinCong
	
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);

	//Le modelo LP para eliminacao de componentes espurios (loops) Esp

	lpEsp = gwCarregaModeloLP(modeloEsp, dados, "fooo.txt");
	gwAtribuiParametrosLP(lpEsp);
	gwCriaIndiceLP(lpEsp);

	// Indetifica posicao inicial da sequencia de variaveis Bij e Hij dos problemas MinCong e Esp
	
	posB11 = gwPosicaoVariavelLP(lp, "Bij[1,1]");
	posH11 = gwPosicaoVariavelLP(lp, "Hij[1,1]");

	posB11_esp = gwPosicaoVariavelLP(lpEsp, "Bij[1,1]");
	posH11_esp = gwPosicaoVariavelLP(lpEsp, "Hij[1,1]");
	
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
	
	vetHij = ivtdAlocaVetorHij(tamRede*tamRede); // aloca o vetor que guarda os valores de caregamento nos caminhos opticos
	vetHij_Invalido = ivtdAlocaVetorHij(tamRede*tamRede); // aloca o vetor que guarda os valores de caregamento nos caminhos opticos
	hmin = ivtdAlocaHij(); // aloca variavel que armazena o carregamento do enlace menos carregado (criterio de retirada de enlaces do ivtd)
	
	while(!sair)
	{
		msImprimeMatSol(topologia);
		printf("atribui Bij_lp\n");
		hmaxAtribuiTopologiaBijLP(lp, topologia);
		printf("atribui Bij_lpEsp\n");
		hmaxAtribuiTopologiaBijLP(lpEsp, topologia);

		printf("resolve lp\n");
		valor = gwValorLP(lp); // variavel valor armazena funcao objetivo otimizada
		printf("Congestionamento da Topologia = %lf\n", valor);
		
		//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lp (COM componentes espurios)
			ivtdAtualizaVetHij(lp, posH11, vetHij_Invalido, tamRede);
				
		
		if(valor < 0) //indica que a topologia foi seccionada pois nao e possivel resolver o LP
		{
			//readiciona enlace
			msAdicionaEnlace(topologia, hmin->lin, hmin->col);
			printf("Seccionou a rede e readicionou o enlace b[%d,%d]\n", hmin->lin, hmin->col);
			
			//indica a primeira vez que a topologia foi seccionada, que e o resultado de ivtd_hmax segundo Karcius D. R. Assis
			if(!seccionou1avez)
			{
				printf("Solucao do IVTD original:\nCongestionamento: %lf\nTopologia:\n", valorFinal);
				msImprimeMatSol(topologia);
				seccionou1avez = 1;
			}
		}
		else  // indica que a solucao para o roteamento de trafego foi obtida passando-se para a eliminacao dos comp. espurios
		{
			// armazena ultimo valor valido para FO do modelo LP		
			valorFinal = valor;
		
			// elimina componentes espurios
			printf("Atribuindo componentes de trafego do model MinCong ao modelo Esp...\n");
			atribuiCompTraf(lp, lpEsp, tamRede);

			printf("Resolvendo lpEsp para retirar componentes espurios...\n");
			gwValorLP(lpEsp);

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
			
/*			printf("Vetor Hij sem atualizar:");
			for (i=0; i<tamRede*tamRede; i++)
				printf("%lf;", vetHij[i]->val);
			printf("\n*****************************\n");
*/			
			//Atualiza o valor de vetHij com os valores de Hij obtidos pela solucao de lpEsp (sem componentes espurios)
			ivtdAtualizaVetHij(lpEsp, posH11_esp, vetHij, tamRede);
			
			printf("Vetor Hij INVALIDO-VALIDO:");
			for (i=0; i<tamRede*tamRede; i++)
				printf("%lf\t", vetHij_Invalido[i]->val-vetHij[i]->val);
			printf("\n*****************************\n");

			
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
	result = ivtd_hmax_esp(nomeArqModelo, nomeArqmodeloEsp, nomeArqDados);
	
	gettimeofday(&t2, NULL);
	tempo = (float) (t2.tv_sec-t1.tv_sec) + 1E-6*(float)(t2.tv_usec-t1.tv_usec);
	printf("\nTempo de execução (s): %f\n\n", tempo);
	
	return result;
}

