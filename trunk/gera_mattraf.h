#ifndef __GERA_MATTRAF_H_
#define __GERA_MATTRAF_H_

#include "mattraf.h"

/* Estrutura que armazena a configuracao do gera_mattraf */
typedef struct configGeraMatTraf
{
	int quantInstance, precision, quantSize, quantSD, quantSN, seed, intervalo, grauLog;
	float minP, maxP, minG, maxG, distMin, distMax;
	int *size;
	float *sd, *sn;
	char comando[32], param[32], diretorio[256], fileConf[256];
} *ConfigGeraMatTraf;

/*=======================================================================*/



/* Cria diretorios */
void gmtCriaDiretorio(char *diretorio)
;

/* Aloca uma estrutura de configGeraMatTraf */
ConfigGeraMatTraf gmtNewConfigGeraMatTraf()
;

/* Libera um estrutura de configGeraMatTraf */
void gmtDelConfigGeraMatTraf(ConfigGeraMatTraf conf)
;

/* Gera e grava matrizes de trafego para uma determinada distribuicao */
void gmtGeraDistribuicao(ConfigMatTraf confMat, ConfigGeraMatTraf conf)
;


/* Gera todas a matrizes de acordo com o dados de ConfigGeraMatTraf */
void gmtMatrixGen(ConfigGeraMatTraf conf)
;


#endif


