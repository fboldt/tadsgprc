#include "mattraf.h"
#include "toolkit.h"
#include <stdlib.h>

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

