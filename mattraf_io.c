#include "mattraf.h"
#include "mattraf_io.h"
#include <stdio.h>
#include <string.h>

#define TAM_STR 128

/* Grava vetor de MatTraf no formato MatProg */
void mtioGravaVetMatTraf(char *nomeBase, int tamVet, MatTraf *vetMT, char *param)
{
	int i;
	char nome[TAM_STR];
	for(i=0; i<tamVet; i++)
	{
		sprintf(nome, "%s%d.%s", nomeBase, i+1, param);
		mtioGravaMatTrafMathProg(nome, vetMT[i], param);
	}
}

/* Grava matriz no formato de dados em MathProg */
void mtioGravaMatTrafMathProg(char *fileName, MatTraf mt, char *param)
{
	int i, j, alinha;
	char demandafmt[TAM_STR], nodofmt[TAM_STR];
	FILE *arq;
	
	/*Encontra numero de caracteres para manter as demandas alinhadas no arquivo texto*/
	alinha = mtioAlign(mtioMaxElem(mt->ordem, mt->h));
	arq = fopen(fileName, "w+");
	sprintf(demandafmt, "%%%d.%df ", alinha+7, 6);
	sprintf(nodofmt, "%%%dd ", alinha+7);
	
	/*fprintf(arq, "data;\n");*/
	fprintf(arq, "param N := %d;\n", mt->ordem);
	
	fprintf(arq, "param %s : ", param);
	for(j = 0; j < mt->ordem; j++)
	{
		fprintf(arq, nodofmt, j+1);
	}
	fprintf(arq, " := \n");
	sprintf(nodofmt, "%%%dd ", strlen(param)+8);
	for(i = 0; i < mt->ordem; i++){
		fprintf(arq, nodofmt, i+1);
		for(j = 0; j < mt->ordem; j++)
			fprintf(arq, demandafmt, mt->h[i][j]);
		fprintf(arq, "\n");
	}
	fprintf(arq, ";\nend;\n");
	fclose(arq);
}

/* Retorna o maior elemento da matriz */
float mtioMaxElem(int tam, float **m)
{
	float max;
	int i, j;
	max = 0;
	for(i=0;i<tam;i++)
	{
		for(j=0;j<tam;j++)
		{
			max = m[i][j]>max?m[i][j]:max;
		}
	}
	return max;
}
/* Retorna a quantidade de casas antes da vírgula do número n */
int mtioAlign(float n)
{
	int i;
	for(i=0;n>1;i++){
		n /= 10;
	}
	return i;
}

