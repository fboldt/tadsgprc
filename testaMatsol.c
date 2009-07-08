#include "matsol.h"
#include <stdio.h>
#include <stdlib.h>

/* Avalia a similaridade de n MatSol em todos graus logicos possiveis */
void msTeste(int tam, int n)
{
	int i, j, k, quant;
	float media, soma, max, min, tmp;
	MatSol vetms[n];
	for(i=1; i<tam; i++)
	{
		soma = 0.0f;
		quant = 0;
		max = 0;
		min = 1;
		printf("gl = %d\n", i);
		for(j=0; j<n; j++)
		{
			vetms[j] = msRLDAMalhaGL(tam, i);
			for(k=0; k<j; k++)
			{
				tmp = msSimilaridade(vetms[k], vetms[j]);
				soma += tmp;
				quant++;
				max = tmp>max?tmp:max;
				min = tmp<min?tmp:min;
			}			
		}
		for(j=0; j<n; j++)
		{
			//msImprimeMatSol(vetms[j]);
			msDelMatSol(vetms[j]);
		}
		media = soma/quant;
		printf("Similaridade media: %.2f%%\tSimilaridade maxima: %.2f%%\tSimilaridade minima: %.2f%%\n", media*100, max*100, min*100);
		//printf("Similaridade media: %f%%\tSimilaridade maxima: %f%%\tSimilaridade minima: %f%%\n", media*100, max*100, min*100);
	}
}

int main(int argc, char **argv)
{
	int tam, quant;
	if(argc < 2)
		printf("Uso: %s [tamanhoRede] [quantMatSol]\ndefault: tamanhoRede=10, quantMatSol=100\n", argv[0]);
	if(argc > 1)
		tam = atoi(argv[1]);
	else
		tam = 10;
	if(argc > 2)
		quant = atoi(argv[2]);
	else
		quant = 100;
	
	msTeste(tam, quant);
	
	return 0;
}

