#include "matsol.h"
#include "glpkwrapper.h"
#include <glpk.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Atribui a topologia de ms ao lp */
void atribuiTopologiaBij(LPX *lp, MatSol ms)
{
	int posBij, tam, k, lin, col;
	posBij = lpx_find_col(lp, "Bij[1,1]");
	tam = ms->ordem*ms->ordem;//-ms->ordem;
	for(k=0; k<tam; k++)
	{
		lin = k/tam;
		col = k%tam;
		// na funcao lpx_set_col_bnds, type eh o tipo de limitacao das variaveis.
		// LPX_FX significa LB = x = UB (lower bound = upper bound)
		// O GLPK faz a indexacao de modo linear e a partir de 1.
		lpx_set_col_bnds (lp, (k+posBij), LPX_FX,
			ms->b[lin][col],
			ms->b[lin][col]);
	}
}

/* Localiza parametro em arquivo de dados MathProg */
int LocalizaParametroInt(char *dados, char *parametro)
{
	FILE *arq;
	char s[1024];
	int valor, encontrado;
	encontrado = 0;
	valor = -1;
	arq = fopen(dados, "r");
	do
	{
		fscanf(arq, "%s", s);
		if(!strcmp(s, "param"))
		{
			fscanf(arq, "%s", s);
			if(!strcmp(s, parametro))
			{
				fscanf(arq, "%s", s);
				if(!strcmp(s, ":="))
				{
					fscanf(arq, "%d", &valor);
					encontrado = 1;
					printf("%d\n",valor);
				}
			}
		}
	}
	while(!feof(arq) && !encontrado);
	
	return valor;
}

/* Imprime o otimo de n testes para um grau logico */
void testeGL(LPX *lp, int tamRede, int gl, int n)
{
	int i;
	float otimo, media, min, max;
	MatSol ms;
	min = 1E+37;
	max = -(1E+37);
	media = 0.0f;
	printf("********* gl = %d\n", gl);
	for(i=0; i<n; i++)
	{
		ms = msRLDAMalhaGL(tamRede, gl);
		atribuiTopologiaBij(lp, ms);
		msDelMatSol(ms);
		otimo = gwValorLP(lp);
		media += otimo;
		min = min>otimo?otimo:min;
		max = max<otimo?otimo:max;
		printf("%f\n", otimo);
	}
	//printf("\n");
	printf("######### media: %f\t######### max: %f\t######### min: %f\n", media/n, max, min);
}

/* Imprime valor otimo de n testes para todos graus logicos possiveis */
void testes(char *modelo, char *dados, int n)
{
	int i, tamRede;
	LPX *lp;
	tamRede = LocalizaParametroInt(dados, "N");
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);
	
	srand(time(NULL));
	for(i=1; i<tamRede; i++)
	{
		testeGL(lp, tamRede, i, n);
	}
	
	gwFinalizaLP(lp);
}

int main(int argc, char **argv)
{
	int quantTestes;
	quantTestes = 10;
	
	if(argc>3)
		quantTestes = atoi(argv[3]);
	
	testes(argv[1], argv[2], quantTestes);
	
	return 0;
}

