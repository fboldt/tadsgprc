/* LOWER BOUND ITERATIVO PARA GRAU LOGICO ASSIMETRICO                                  */
/* Autores: Sergio Nery, Renato Tannure e Francisco Boldt	                       */
/*                                                                                     */
/* Compilar:                                                                           */
/*    gcc lbi_hmax_glass.c mattraf.c toolkit.c -o lbi_hmax_glass -lglpk -Wall          */
/*                                                                                     */
/* Executar:                                                                           */
/*    ./lbi_hmax_glass <arq_mod> <arq_data> <ntransc>                                  */

#include <stdio.h>
#include <glpk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mattraf.h"

float  LBi(char *modelo, char *data, char *saida,
	float ntransc, int N)
{
	LPX *lp;
	int i, k;
	int iteracoes, len, sair;
	int pos_topologia, pos_restricao, pos_ntransc;
	float Hmax;
	float HmaxL = 0.000001f;
	int *ind;
	double *val;
	char str[64];
	char NomeRestricaoHmaxL[] = "limit_cutpl";
	char NomeVarTopologia[] = "B";

	glp_term_out(GLP_OFF); 
	lp = lpx_read_model(modelo, data, saida);

	// Parametros de otimizacao
	//lpx_set_int_parm(lp, LPX_K_MSGLEV, 0);
	lpx_set_int_parm(lp, LPX_K_PRESOL, 1);
	lpx_create_index(lp);

	//Insere valor do numero de transceptores nas restricoes
	pos_ntransc = lpx_find_col(lp, "ntransc");
	lpx_set_col_bnds(lp, pos_ntransc, LPX_FX, ntransc, ntransc);

	// Encontra a posicao da restricao
	// relativa ao plano de corte HmaxL
	sprintf(str, "%s[1,2]", NomeRestricaoHmaxL);
	pos_restricao = lpx_find_row(lp, str);
	//printf("pos_restricao=%d\n", pos_restricao);

	// Descobre o numero de coeficientes
	// nao-nulos da restricao do plano de corte 
	len = lpx_get_mat_row(lp, pos_restricao, NULL, NULL);
	//printf("len=%d\n", len);
	
	// Aloca memoria suficiente para armazenar
	// indices das variaveis e valores dos coeficientes
	// da restricao do plano de corte
	ind = (int*)    malloc((len+1)*sizeof(int));
	val = (double*) malloc((len+1)*sizeof(double));
	
	// Busca primeira linha da restricao do plano de corte
	// armazenando indices das variaveis e valores dos coeficientes
	len = lpx_get_mat_row(lp, pos_restricao, ind, val);
	
	// Busca posicao (k) do coeficiente (HmaxL)
	// da variavel de topologia virtual
	sprintf(str, "%s[1,2]", NomeVarTopologia);
	pos_topologia = lpx_find_col(lp, str);
	//printf("pos_topologia=%d\n", pos_topologia);

	k = 1;
	while (pos_topologia != ind[k])
		k++;
	// Atualiza valor do plano de corte HmaxL
	iteracoes = 0;
	do
	{
		iteracoes++;
		for (i=0; i<N*(N-1); i++)
		{
			len = lpx_get_mat_row (lp, pos_restricao+i, ind, val);
			val[k] = HmaxL;
			lpx_set_mat_row (lp, pos_restricao+i, len, ind, val);
			lpx_set_row_bnds(lp, pos_restricao+i, LPX_LO, HmaxL, 0.0);
		}

		if (lpx_simplex(lp) != LPX_E_OK)
			exit(-1);
		Hmax = lpx_get_obj_val(lp);
		//printf("iteracoes=%d\tHmax=%f\n", iteracoes, Hmax);
		sair = (fabs(Hmax-HmaxL) < 1E-4);
		HmaxL = Hmax;
	} while (!sair);
	lpx_delete_prob(lp);
	return Hmax;
}

int main(int argc, char **argv)
{
	char *modelo, *data;
	int ntransc, N;
	float LBi_Hmax;

	if (argc!=3)
	{
		printf("Numero incorreto de parametros.\n");
		printf("Uso:\n"
			"%s <arq_mod> <arq_data> \n",
			argv[0]);
		exit(-1);
	}
	modelo     = argv[1];
	data       = argv[2];
		
	N = mtLocalizaParametroInt(data, "N");
	printf("ntransc LBi_Hmax \n");
	for (ntransc=(N*(N-1)); ntransc>=N; ntransc--)
	{
		LBi_Hmax = LBi(modelo, data, NULL, ntransc, N);
		printf("%7d %f \n", ntransc, LBi_Hmax);
	}

	return 0;
}

