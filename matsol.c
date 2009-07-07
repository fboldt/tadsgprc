#include "matsol.h"
#include "toolkit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Atribui zero a todas posicoes da matriz em ms */
void msZeraMatSol(MatSol ms)
{
	int i, j;
	for(i=0; i<ms->ordem; i++)
	{
		for(j=0; j<ms->ordem; j++)
		{
			ms->b[i][j] = 0;
		}
	}
}

/* Construtor MatSol */
MatSol msNewMatSol(int ordem)
{
	int i;
	MatSol ms = (MatSol)malloc(sizeof(struct matSol));
	ms->b = (char**)tkAlocaMatriz(ordem, ordem, sizeof(char));
	ms->qLin = tkAlocaVetor(ordem, sizeof(int));
	ms->qCol = tkAlocaVetor(ordem, sizeof(int));
	ms->ordem = ordem;
	msZeraMatSol(ms);
	for(i=0; i<ordem; i++)
	{
		ms->qLin[i] = 0;
		ms->qCol[i] = 0;
	}
	return ms;
}

/* Destrutor MatSol */
void msDelMatSol(MatSol ms)
{
	if(ms->b != NULL)
		tkLiberaMatriz((void**)ms->b);
	if(ms->qLin != NULL)
		tkLiberaVetor((void*)ms->qLin);
	if(ms->qCol != NULL)
		tkLiberaVetor((void*)ms->qCol);
	free(ms);
}

/* Preenche um vetor de int de zero a tam-1 */
void msPreencheVetorOrdenado(int tam, int *anel)
{
	int i;
	for(i=0; i<tam; i++)
	{
		anel[i] = i;
	}
}

/* Troca os valores de dois ponteiros de int */
void msTrocaValInt(int *a, int *b)
{
	int aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

/* Mistura valores de um vetor de int */
void msMisturaVetor(int tam, int *anel)
{
	int i, n;
	for(i=0; i<tam; i++)
	{
		n = rand()%tam;
		msTrocaValInt(anel+i, anel+n);
	}
}

/* Incrementa 1 na posicao (i,j) de b */
void msIncrementaMatSol(MatSol ms, int i, int j)
{
	ms->b[i][j]++;
	ms->qLin[i]++;
	ms->qCol[j]++;
}

/* Decrementa 1 na posicao (i,j) de b */
void msDecrementaMatSol(MatSol ms, int i, int j)
{
	ms->b[i][j]--;
	ms->qLin[i]--;
	ms->qCol[j]--;
}

/* Atribui val na posicao (i,j) de b */
void msAtribuiMatSol(MatSol ms, int i, int j, int val)
{
	ms->b[i][j] = val;
	ms->qLin[i] += val;
	ms->qCol[j] += val;
}

/* Adiciona enlace atribuindo 1 na posicao (i,j) de b */
void msAdicionaEnlace(MatSol ms, int i, int j)
{
	ms->b[i][j] = 1;
	ms->qLin[i] += 1;
	ms->qCol[j] += 1;
}

/* Adiciona enlace atribuindo 1 na posicao (i,j) de b restringindo a um valor maximo e a um grau logico */
void msAdicionaEnlaceMaxGL(MatSol ms, int i, int j, int max, int gl)
{
	if(ms->b[i][j]+1<max && ms->qLin[i]<gl && ms->qCol[j]<gl)
	{
		ms->b[i][j]++;
		ms->qLin[i]++;
		ms->qCol[j]++;
	}
}

/* Remove enlace decrementando 1 na posicao (i,j) de b */
int msRemoveEnlace(MatSol ms, int i, int j)
{
	if(ms->qLin[i]>1 && ms->qCol[j]>1)
	{
		ms->b[i][j]--;
		ms->qLin[i]--;
		ms->qCol[j]--;
	}
	else
	{
		return 0;
	}
	return 1;
}

/* Atribui um vetor como anel em ms */
void msAtribuiAnelMatSol(MatSol ms, int *anel)
{
	int i, j, aux;
	for(aux=0; aux<ms->ordem; aux++)
	{
		i = anel[aux];
		j = anel[(aux+1)%ms->ordem];
		msIncrementaMatSol(ms, i, j);
	}
}

void msCompletaGL(MatSol ms, int max, int gl)
{
	int i, j;
	for(i=0; i<ms->ordem; i++)
	{
		if(ms->qLin[i] < gl)
		{
			for(j=0; j<ms->ordem; j++)
			{
				if(i!=j && ms->qCol[j]<gl && ms->b[i][j]<max)
				{
					msIncrementaMatSol(ms, i, j);
				}
			}
		}
	}
}

/* Atribui um vetor como anel em ms com valor maximo*/
void msAtribuiAnelMatSolMax(MatSol ms, int *anel, int max, int gl)
{
	int i, j, a, b, ok, salto;
	for(a=0; a<ms->ordem; a++)
	{
		i = anel[a];
		j = anel[(a+1)%ms->ordem];
		ok = 0;
		salto = ms->ordem/2;
		for(b=0; b<ms->ordem && !ok; b++)
		{
			if(ms->b[i][j] < max && i!=j && ms->qLin[i]<gl && ms->qCol[j]<gl)
			{
				msIncrementaMatSol(ms, i, j);
				ok = 1;
			}
			else
			{
				j = (j+salto)%ms->ordem;
				salto = salto>1?salto/2:1;
			}
		}
	}
	msCompletaGL(ms, max, gl);
}

/* Atribui um vetor como anel full duplex em ms com valor maximo*/
void msAtribuiAnelMatSolMaxSimetrico(MatSol ms, int *anel, int max, int gl)
{
	int i, j, a, b, ok, salto;
	for(a=0; a<ms->ordem; a++)
	{
		i = anel[a];
		j = anel[(a+1)%ms->ordem];
		ok = 0;
		salto = ms->ordem/2;
		for(b=0; b<ms->ordem && !ok; b++)
		{
			if(ms->b[i][j]<max && i!=j && ms->qLin[i]<gl && ms->qCol[j]<gl)
			{
				msIncrementaMatSol(ms, i, j);
				msIncrementaMatSol(ms, j, i);
				ok = 1;
			}
			else
			{
				j = (j+salto)%ms->ordem;
				salto = salto>1?salto/2:1;
			}
		}
	}
}

/* Preeche vetor com valores aleatorios nao repetidos de zero a tam-1 */
void msPreencheVetorAleatorio(int tam, int *anel)
{
	msPreencheVetorOrdenado(tam, anel);
	msMisturaVetor(tam, anel);
}

/* Atribui anel basico em ms */
void msAnelBasico(MatSol ms)
{
	int anel[ms->ordem];
	msPreencheVetorAleatorio(ms->ordem, anel);
	msAtribuiAnelMatSol(ms, anel);
}

/* Imprime no terminal de texto uma MatSol */
void msImprimeMatSol(MatSol ms)
{
	int i, j;
	char fmt[32];
	printf("\n");
	for(i=0; i<ms->ordem; i++)
	{
		for(j=0; j<ms->ordem; j++)
		{
			sprintf(fmt, "%%%dd ", (int)(log(ms->qCol[j]-2>0?ms->qCol[j]-2:1)));
			printf(fmt, ms->b[i][j]);
		}
		printf(" | %d \n", ms->qLin[i]);
	}
	for(i=0; i<ms->ordem; i++)
	{
		printf("--");
		for(j=1; j<(int)(log(ms->qCol[i]-2>0?ms->qCol[i]-2:1)); j++)
		{
			printf("-");
		}
	}
	printf("\n");
	for(j=0; j<ms->ordem; j++)
	{
		printf("%d ", ms->qCol[j]);
	}
	printf("\n");
}

/* Cria matriz aleatoria em malha */
MatSol msRLDAMalhaGL(int ordem, int gl)
{
	int i;
	int anel[ordem];
	int max;
	max = 1;
	MatSol ms = msNewMatSol(ordem);
	for(i=0;i<gl;i++)
	{
		msPreencheVetorAleatorio(ms->ordem, anel);
		msAtribuiAnelMatSolMax(ms, anel, max, i+1);
	}
	return ms;
}

/* Cria matriz aleatoria simetrica em malha */
MatSol msRLDAMalhaGLSimetrico(int ordem, int gl)
{
	int i;
	int anel[ordem];
	int max;
	max = 1;
	MatSol ms = msNewMatSol(ordem);
	for(i=0;i<gl;i++)
	{
		msPreencheVetorAleatorio(ms->ordem, anel);
		msAtribuiAnelMatSolMaxSimetrico(ms, anel, max, i+1);
	}	
	return ms;
}

/* Mede o grau de similaridade entre duas MatSol com valores entre 0 e 1 */
float msSimilaridade(MatSol ms1, MatSol ms2)
{
	int i, j, ordem;
	float similaridade;
	similaridade = 0.0f;
	if(ms1->ordem != ms2->ordem)
		return 0.0f;
	else
		ordem = ms1->ordem;
	for(i=0; i<ordem; i++)
	{
		for(j=0; j<ordem; j++)
		{
			similaridade += ms1->b[i][j]>0 && ms1->b[i][j]==ms2->b[i][j];
			//similaridade += ms1->b[i][j]==ms2->b[i][j];
		}
	}
	similaridade /= ordem*ordem-ordem;
	//similaridade /= ordem*ordem;
	return similaridade;
}

/* Atribui a ms uma topologia totalmente conectada */
void msTotalmenteConexa(MatSol ms)
{
	int i, j;
	for(i=0; i<ms->ordem; i++)
	{
		for(j=0; j<ms->ordem; j++)
		{
			if(i!=j)
			{
				msAdicionaEnlace(ms, i, j);
			}
			//msAtribuiMatSol(ms, i, j, i=j);
		}
	}
}

