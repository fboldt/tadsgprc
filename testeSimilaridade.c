#include "matsol.h"
#include <stdio.h>
#include <stdlib.h>

/* Avalia a similaridade de n MatSol para o grau logico dado como parametro */
void msTesteGL(int tam, int n, int gl)
{
	int j, k, quant;
	double media, soma, max, min, tmp;
	MatSol vetms[n];
	
	soma = 0.0f;
	quant = 0;
	max = 0;
	min = 1;
	//printf("gl = %d\n", gl);
	for(j=0; j<n; j++)
	{
		vetms[j] = msRLDAMalhaGL(tam, gl);
		for(k=0; k<j; k++)
		{
			tmp = msSimilaridade(vetms[k], vetms[j]);
			soma += tmp;
			quant++;
			max = tmp>max?tmp:max;
			min = tmp<min?tmp:min;
			printf("%.0lf\n",tmp);
		}			
	}
	for(j=0; j<n; j++)
	{
		//msImprimeMatSol(vetms[j]);
		msDelMatSol(vetms[j]);
	}
	media = soma/quant;
	//printf("Similaridade media: %.2lf%%\tSimilaridade maxima: %.2lf%%\tSimilaridade minima: %.2lf%%\n", media*100, max*100, min*100);
	//printf("Similaridade media: %lf%%\tSimilaridade maxima: %lf%%\tSimilaridade minima: %lf%%\n", media*100, max*100, min*100);

}

/* Avalia a similaridade de n MatSol em todos graus logicos possiveis */
void msTeste(int tam, int n)
{
	int i;
	for(i=1; i<tam; i++)
	{
		msTesteGL(tam, n, i);
	}
}

int main(int argc, char **argv)
{
	int tam, quant;
	if(argc < 2)
		printf("Uso: %s [tamanhoRede] [quantMatSol] [grauLogico]\ndefault: tamanhoRede=14, quantMatSol=100, grauLogico=1..tamanhRede-1\n", argv[0]);
	
	if(argc > 1)
		tam = atoi(argv[1]);
	else
		tam = 14;
	
	if(argc > 2)
		quant = atoi(argv[2]);
	else
		quant = 100;
	
	if(argc > 3)
		msTesteGL(tam, quant, atoi(argv[3]));
	else
		msTeste(tam, quant);
	
	return 0;
}

