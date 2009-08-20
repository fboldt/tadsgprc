#include "matsol.h"
#include "glpkwrapper.h"
#include "milp_hmax_agg.c"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* Imprime o otimo de n testes para um grau logico */
void testeGL(LPGW lp, int tamRede, int gl, int n)
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
		hmaxAtribuiTopologiaBijLP(lp, ms);
		msDelMatSol(ms);
		otimo = gwValorLP(lp);
		media += otimo;
		min = min>otimo?otimo:min;
		max = max<otimo?otimo:max;
		printf("%f\n", otimo);
	}
	//printf("\n");
	//printf("######### media: %f\t######### max: %f\t######### min: %f\n", media/n, max, min);
}

/* Imprime valor otimo de n testes para todos graus logicos possiveis */
void testes(char *modelo, char *dados, int quantTestes)
{
	int i, tamRede;
	LPGW lp;
	tamRede = mtioLocalizaParametroInt(dados, "N");
	lp = gwCarregaModeloLP(modelo, dados, "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);
	
	srand(time(NULL));
	for(i=1; i<tamRede; i++)
	{
		testeGL(lp, tamRede, i, quantTestes);
	}
	
	gwFinalizaLP(lp);
}

int solucoesAleatorias(int argc, char **argv)
{
	int quantTestes;
	quantTestes = 10;
	
	if(argc<3)
	{
		printf("\n\tUso: %s <modelo> <dados> [quantidade de testes]\n\n", argv[0]);
		return -1;
	}
	
	if(argc>3)
		quantTestes = atoi(argv[3]);
	
	testes(argv[1], argv[2], quantTestes);
	
	return 0;
}

