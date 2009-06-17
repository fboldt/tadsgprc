#include "glpkwrapper.h"
#include <glpk.h>
#include <stdlib.h>

// Faz a leitura do problema e gera o modelo linear
// Inicializa estruturas de dados do LP, que eh um MILP,
// pois os valores (bounds) das variaveis inteiras ainda nao foram setados
LPX *gwCarregaModeloLP(char *modelo, char *dados, char *saida)
{
	return (lpx_read_model(modelo, dados, saida));
}

/* Atribui parametros default em modelo lp */
void gwAtribuiParametrosLP(LPX *lp)
{
	// Message Level: 0 - nothing; 1 - errors; 2 - normal; 3 - verbose.
	lpx_set_int_parm(lp, LPX_K_MSGLEV, 0);
	// Boolean: Habilita Pressolver
	lpx_set_int_parm(lp, LPX_K_PRESOL, 1);
}

// Finaliza (desaloca) estruturas de dados do LP
void gwFinalizaLP(LPX *lp)
{
	lpx_delete_prob(lp);
}

// Cria o indice de nomes no modelo LP
void gwCriaIndiceLP(LPX *lp)
{
	lpx_create_index(lp);
}

// Descobre a posicao de dada variavel no modelo LP
int gwPosicaoVariavelLP(char NomeVariavel[], LPX *lp)
{
	int pos_variavel;
	pos_variavel = lpx_find_col(lp, NomeVariavel);
	return pos_variavel;
}

/* Retorna o valor da funcao objetivo de um lp */
float gwValorLP(LPX *lp)
{
	// Roda o simplex e testa erro
	if (lpx_simplex(lp) != LPX_E_OK)
	{           
		exit(2);
	}
	return lpx_get_obj_val(lp);
}

