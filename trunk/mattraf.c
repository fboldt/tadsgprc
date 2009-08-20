#include "mattraf.h"
#include "toolkit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAM_STR 128

/* Construtor ConfigMatTraf */
ConfigMatTraf mtNewConfigMatTraf()
{
	return (ConfigMatTraf)malloc(sizeof(struct configMatTraf));
}

/* Destrutor ConfigMatTraf */
void mtDelConfigMatTraf(ConfigMatTraf cm)
{
	free(cm);
}

/* Construtor MatTraf */
MatTraf mtNewMatTraf(int ordem)
{
	MatTraf mt = (MatTraf)malloc(sizeof(struct matTraf));
	mt->h = (float**)tkAlocaMatriz(ordem, ordem, sizeof(float));
	mt->ordem = ordem;
	return mt;
}

/* Destrutor MatTraf */
void mtDelMatTraf(MatTraf mt)
{
	if(mt->h != NULL)
		tkLiberaMatriz((void**)mt->h);
	free(mt);
}

/* Cria um vetor de instancias de MatTraf */
MatTraf* mtCriaInstancias(ConfigMatTraf confMat, int quantInst)
{
	int i;
	MatTraf *vetMT;
	vetMT = (MatTraf*)malloc(sizeof(MatTraf)*quantInst);
	for(i=0; i<quantInst; i++)
	{
		vetMT[i] = mtCriaInstancia(confMat);
	}
	return vetMT;
}

/* Desaloca um vetor de MatTrafs */
void mtLiberaVetorMatTraf(MatTraf* vetMT, int quantInst)
{
	int i;
	for(i = 0; i < quantInst; i++)
	{
		mtDelMatTraf(vetMT[i]);
	}
	free(vetMT);
}

/* Cria uma instancia de MatTraf */
MatTraf mtCriaInstancia(ConfigMatTraf confMat)
{
	MatTraf mt = mtNewMatTraf(confMat->ordem);
	mt->ordem = confMat->ordem;
	switch(confMat->tipoDistribuicao)
	{
		case UNI: mt->h = mtUniform(confMat);
				  break;
		case SD: mt->h = mtSuperd(confMat);
				 break;
		case SN: mt->h = mtSupern(confMat);
				 break;
	}
	return mt;
}

/* Cria uma matriz uniforme */
float** mtUniform(ConfigMatTraf confMat)
{
	float** m;
	float n;
	int i, j;
	
	m = (float**)tkAlocaMatriz(confMat->ordem, confMat->ordem, sizeof(float));
	
	for(i=0; i<confMat->ordem; i++){
		for(j=0; j<confMat->ordem; j++){
			n = mtBetween(confMat->minG, confMat->maxG);
			m[i][j] = i==j?0:n;
		}
	}
	return m;
}

/* Cria um matriz de super demanda */
float** mtSuperd(ConfigMatTraf confMat)
{
	float** m;
	float n;
	int i, j, k;
	float p;
	float control;
	float** flag;
	
	m = (float**)tkAlocaMatriz(confMat->ordem, confMat->ordem, sizeof(float));
	
	flag = (float**)tkAlocaMatriz(confMat->ordem, confMat->ordem, sizeof(float));
	
	for(i=0; i<confMat->ordem; i++)
		for(j=0; j<confMat->ordem; j++)
			flag[i][j] = 0.0f;

	p = confMat->porcentagemDistribuicao * (confMat->ordem*(confMat->ordem-1));
	
	for(i=0;i<confMat->ordem;i++)
	{
		for(j=0;j<confMat->ordem;j++)
		{
			n = mtBetween(confMat->minP, confMat->maxP);
			m[i][j] = i==j?0:n;
		}
	}
	for(k=0; k<p; k++)
	{
		i = mtBetween(0, confMat->ordem-1);
		j = mtBetween(0, confMat->ordem-1);
		control = flag[i][j];
		if((i!=j)&&(control==0))
		{
			n = mtBetween(confMat->minG, confMat->maxG);
			m[i][j] = n;
			flag[i][j] = 1.0f;
		}
		else
		{
			k--;
		}
	}
	tkLiberaMatriz((void**)flag);
	return m;
}

/* Cria um matriz de super nos */
float** mtSupern(ConfigMatTraf confMat)
{
	float** m;
	float n;
	int i, j, k;
	float p;
	float control;
	float** flag;
	
	m = (float**)tkAlocaMatriz(confMat->ordem, confMat->ordem, sizeof(float));
	
	flag = (float**)tkAlocaMatriz(confMat->ordem, 1, sizeof(float));
	
	for(i=0;i<confMat->ordem;i++)
		flag[i][0] = 0;

	p = confMat->porcentagemDistribuicao*confMat->ordem;

	for(i=0;i<confMat->ordem;i++)
	{
		for(j=0;j<confMat->ordem;j++)
		{
			n = mtBetween(confMat->minP, confMat->maxP);
			m[i][j] = i==j?0:n;
		}
	}
	for(k=0; k<p; k++)
	{
		i = mtBetween(0, confMat->ordem-1);
		control = flag[i][0];
		if(control==0)
		{
			flag[i][0] = 1;
			for(j=0; j<confMat->ordem; j++)
			{
				if(i!=j)
				{
					n = mtBetween(confMat->minG, confMat->maxG);
					m[i][j] = n;
					n = mtBetween(confMat->minG, confMat->maxG);
					m[j][i] = n;
				}
			}
		}
		else k--;
	}
	tkLiberaMatriz((void**)flag);
	return m;
}

/* Retorna um valor do tipo float entre nim e max */
float mtBetween(float min, float max)
{
	float n;
	n = (float)drand48();
	n *= (max-min);
	n += min;
	return n;
}

/* Retorna o intervalo linear entre a e b */
float mtIntervaloLinear(float a, float b, int intervalo, int posicao)
{
	float c;
	c = b - a;
	c = c / (intervalo + 1);
	c = c * (posicao + 1);
	c = a + c;
	return c;
}

/* Retorna a matriz posicao de um intervalo entre duas matrizes */
MatTraf mtMatTrafIntervalar(MatTraf m1, MatTraf m2, int intervalo, int posicao)
{
	int i, j;
	MatTraf m;
	if(m1->ordem != m2->ordem)
	{
		return NULL;
	}
	m = mtNewMatTraf(m1->ordem);
	for(i = 0 ; i < m1->ordem ; i++)
	{
		for(j = 0; j < m1->ordem; j++)
		{
			m->h[i][j] = mtIntervaloLinear(m1->h[i][j], m2->h[i][j], intervalo, posicao);
		}
	}
	return m;
}

/* Cria vetor de MatTrafs intervalares */
MatTraf* mtVetMatTrafIntervalar(MatTraf m1, MatTraf m2, int intervalo)
{
	int i;
	MatTraf* vetMT;
	vetMT = (MatTraf*)malloc(sizeof(MatTraf)*intervalo);
	for(i=0; i<intervalo; i++)
	{
		vetMT[i] = mtMatTrafIntervalar(m1, m2, intervalo, i);
	}
	return vetMT;
}

/* Imprime no terminal de texto uma MatTraf */
void mtImprimeMatTraf(MatTraf mt)
{
	int i, j;
	printf("\n");
	for(i=0; i<mt->ordem; i++)
	{
		for(j=0; j<mt->ordem; j++)
		{
			printf("%10.6f ", mt->h[i][j]);
		}
		printf("\n");
	}
}

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

