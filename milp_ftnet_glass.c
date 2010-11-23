/* LOWER BOUND ITERATIVO PARA GRAU LOGICO ASSIMETRICO                                  */
/* Autores: Sergio Nery, Renato Tannure e Francisco Boldt	                       */
/*                                                                                     */
/* Compilar:                                                                           */
/*    gcc lbi_hmax_glass.c mattraf.c toolkit.c -o lbi_hmax_glass -lglpk -Wall          */
/*                                                                                     */
/* Executar:                                                                           */
/*    ./lbi_hmax_glass <arq_mod> <arq_data>                                            */

#include <stdio.h>
#include <glpk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mattraf.h"


int main(int argc, char **argv)
{
	char *modelo, *data;
	int ntransc, N;
	float FTnet;
	LPX *milp;
	int pos_ntransc;

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
	
	glp_term_out(GLP_OFF); //inibe a exibicao de informacoes na tela
	milp = lpx_read_model(modelo, data, "foo.txt");
	
	// Parametros de otimizacao
	//lpx_set_int_parm(lp, LPX_K_MSGLEV, 0);
	lpx_set_int_parm(milp, LPX_K_PRESOL, 1);
	lpx_create_index(milp);
	
	pos_ntransc = lpx_find_col(milp, "ntransc");
	
	printf("%s ", data);
	for (ntransc=(N*(N-1)); ntransc>=(N*N/3.0f); ntransc--)
	{
		//Insere valor do numero de transceptores nas restricoes
		lpx_set_col_bnds(milp, pos_ntransc, LPX_FX, ntransc, ntransc);

		if (lpx_simplex(milp) != LPX_E_OK)
			exit(-1);
		FTnet = lpx_get_obj_val(milp);
		printf("%f ", FTnet);
	}
	printf("\n");
	lpx_delete_prob(milp);

	return 0;
}

