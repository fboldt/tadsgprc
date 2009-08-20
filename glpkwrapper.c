#include "glpkwrapper.h"
#include <glpk.h>
#include <stdlib.h>

/* Fixa o valor a uma variavel inteira atribuindo esse valor ao upper bond e ao lower bound */
void gwAtribuiValorPosicaoLP(LPGW lp, int pos, int valor)
{
	// na funcao lpx_set_col_bnds, type eh o tipo de limitacao das variaveis.
	// LPX_FX significa LB = x = UB (lower bound = upper bound)
	// O GLPK faz a indexacao de modo linear e a partir de 1.
	lpx_set_col_bnds (lp, pos, LPX_FX, valor, valor);
}

// Faz a leitura do problema e gera o modelo linear
// Inicializa estruturas de dados do LP, que eh um MILP,
// pois os valores (bounds) das variaveis inteiras ainda nao foram setados
LPGW gwCarregaModeloLP(char *modelo, char *dados, char *saida)
{
	return (lpx_read_model(modelo, dados, saida));
}

/* Atribui parametros default em modelo lp */
void gwConfiguraGLPK(LPGW lp)
{
	// Message Level: 0 - nothing; 1 - errors; 2 - normal; 3 - verbose.
	lpx_set_int_parm(lp, LPX_K_MSGLEV, 0);
	// Boolean: Habilita Pressolver
	lpx_set_int_parm(lp, LPX_K_PRESOL, 1);
}

// Finaliza (desaloca) estruturas de dados do LP
void gwFinalizaLP(LPGW lp)
{
	lpx_delete_prob(lp);
}

// Cria o indice de nomes no modelo LP
void gwCriaIndiceLP(LPGW lp)
{
	lpx_create_index(lp);
}

// Descobre a posicao de dada variavel no modelo LP
int gwPosicaoVariavelLP(LPGW lp, char *NomeVariavel)
{
	int pos_variavel;
	pos_variavel = lpx_find_col(lp, NomeVariavel);
	return pos_variavel;
}

/* Retorna o valor da funcao objetivo de um lp */
double gwValorLP(LPGW lp)
{
	// Roda o simplex e testa erro
	if (lpx_simplex(lp) != LPX_E_OK)
	{           
		return -1;
	}
	return lpx_get_obj_val(lp);
}

/* Retorna o valor de uma variavel em um lp em determinada posicao */
double gwValorVariavel(LPGW lp, int posicao)
{
	return lpx_get_col_prim(lp,posicao);
}

