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

/* Localiza parametro em arquivo de dados MathProg */
int mtioLocalizaParametroInt(char *dados, char *parametro)
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
					//printf("%d\n",valor);
				}
			}
		}
	}
	while(!feof(arq) && !encontrado);
	
	return valor;
}

/* Realiza a leitura de um arquivo de dados do problema LP
Retorna: Estrutura MatTraf preenchida 
AVISO: FUNCAO EM TESTE!!! FUNCIONAMENTO IMPERFEITO!!! */
MatTraf mtioCarregaMatTraf(char *NomeArqDados)
{
	int i, j, n;
	char s[1024];
	float valor;
	FILE *ArqDados;
	int tamRede;
	//float **Matriz;
	MatTraf mt;

	// Abre arquivo de dados somente para leitura
	ArqDados = fopen(NomeArqDados, "r");
	if (ArqDados == NULL)
	{
		return NULL;
	}
	
	// Le arquivo de dados para retornar o tamanho da rede
	// e a matriz de trafego alocada e preenchida
	fscanf(ArqDados, "%s%s%s%d%s", s, s, s, &n, s);
	tamRede = n;
	
	mt = mtNewMatTraf(tamRede);
	
	//Matriz = (float**) AlocaMatriz(tamRede, tamRede, sizeof(float));
	fgets(s, 1024, ArqDados);
	fgets(s, 1024, ArqDados);
	
	// le arquivo de dados preenchendo a matriz de trafego
	for (i=0; i<n; i++)
	{
		fscanf(ArqDados, "%s", s);
		for (j=0; j<n; j++)
		{
			fscanf(ArqDados, "%f", &valor);
			mt->h[i][j] = valor;
		}
	}
	fclose(ArqDados);
	return mt;
}

