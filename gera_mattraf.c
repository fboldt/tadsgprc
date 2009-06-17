#include "gera_mattraf.h"
#include "mattraf.h"
#include "mattraf_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* Cria diretorios */
void gmtCriaDiretorio(char *diretorio)
{
	mkdir(diretorio, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
}

/* Aloca uma estrutura de configGeraMatTraf */
ConfigGeraMatTraf gmtNewConfigGeraMatTraf()
{
	return 	(ConfigGeraMatTraf)malloc(sizeof(struct configGeraMatTraf));
}

/* Libera um estrutura de configGeraMatTraf */
void gmtDelConfigGeraMatTraf(ConfigGeraMatTraf conf)
{
	free(conf->size);
	free(conf->sd);
	free(conf->sn);
	free(conf);
}

/* Gera e grava matrizes de trafego para uma determinada distribuicao */
void gmtGeraDistribuicao(ConfigMatTraf confMat, ConfigGeraMatTraf conf)
{
	MatTraf *vetMT, *vetInterv;
	char nome[128], tipoDist[128];
	int i;
	/*Cria diretorio especificado se nao existir*/
	gmtCriaDiretorio(conf->diretorio);
	/*Formata a string tipoDist com o tipo de distribuicao de confMat*/
	switch(confMat->tipoDistribuicao)
	{
		case UNI: sprintf(tipoDist, "UNI");
				  break;
		case SD: sprintf(tipoDist, "SD%.0f", (confMat->porcentagemDistribuicao*100));
				 break;
		case SN: sprintf(tipoDist, "SN%.0f", (confMat->porcentagemDistribuicao*100));
				 break;
	}
	/*Cria um vetor de instancias de MatTraf com as configuracoes de confMat*/
	vetMT  = mtCriaInstancias(confMat, conf->quantInstance);
	/*Formata o nomeBase para os arquivos que serao gravados com as MatTraf*/
	sprintf(nome, "%s/%s_N%d_%s_p", conf->diretorio, conf->fileConf, confMat->ordem, tipoDist);
	/*Grava arquivos da MatTraf contidas no vetor de Mattrafs vetMT*/
	mtioGravaVetMatTraf(nome, conf->quantInstance, vetMT, conf->param);
	for(i = 0; i < conf->quantInstance -1; i++)
	{
		/*Cria um vetor de instancias de MatTraf com demandas de duas MatTrafs interpoladas*/
		vetInterv = mtVetMatTrafIntervalar(vetMT[i], vetMT[i+1], conf->intervalo);
		/*Formata o nomeBase para as MatTrafs interpoladas*/
		sprintf(nome, "%s/%s_N%d_%s_p%d_i", conf->diretorio, conf->fileConf, confMat->ordem, tipoDist, i+1);
		/*Grava arquivos da MatTraf contidas no vetor de Mattrafs intervalares vetInterv*/
		mtioGravaVetMatTraf(nome, conf->intervalo, vetInterv, conf->param);
		/*Libera o vetor de MatTrafs intervalaras vetInterv*/
		mtLiberaVetorMatTraf(vetInterv, conf->intervalo);
	}
	/*Libera o vetor de MatTrafs vetMT*/
	mtLiberaVetorMatTraf(vetMT, conf->quantInstance);
}


/* Gera todas a matrizes de acordo com o dados de ConfigGeraMatTraf */
void gmtMatrixGen(ConfigGeraMatTraf conf)
{	
	int ord, i;
	ConfigMatTraf confMat = mtNewConfigMatTraf();
	confMat->minP = conf->minP;
	confMat->minG = conf->minG;
	confMat->maxP = conf->maxP;
	confMat->maxG = conf->maxG;
	for(ord = 0; ord < conf->quantSize; ord++)
	{
		confMat->ordem = conf->size[ord];
		/*Gera e grava matrizes uniformes*/
		confMat->tipoDistribuicao = UNI;
		gmtGeraDistribuicao(confMat, conf);
		/*Gera e grava matrizes de super demandas*/
		confMat->tipoDistribuicao = SD;
		for(i = 0; i < conf->quantSD; i++)
		{
			confMat->porcentagemDistribuicao = conf->sd[i];
			gmtGeraDistribuicao(confMat, conf);
		}
		/*Gera e grava matrizes de super nos*/
		confMat->tipoDistribuicao = SN;
		for(i = 0; i < conf->quantSN; i++)
		{
			confMat->porcentagemDistribuicao = conf->sn[i];
			gmtGeraDistribuicao(confMat, conf);
		}		
	}
}

