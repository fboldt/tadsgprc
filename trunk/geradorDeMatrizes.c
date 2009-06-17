#include "gera_mattraf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Le o arquivo de configuracao e grava as matrizes */
ConfigGeraMatTraf leConf(char *fileName)
{
	int quantInstance, precision, quantSize, quantSD, quantSN, i, seed, intervalo;
	float minP, maxP, minG, maxG;
	int *size;
	float *sd, *sn;
	char comando[256], diretorio[256], comentario[256];
	ConfigGeraMatTraf conf;
	FILE *arq;
	arq = fopen(fileName, "r");
	if(arq==NULL)
	{
		return NULL;
	}
	conf = gmtNewConfigGeraMatTraf();
	strcpy(conf->fileConf,fileName);
	do{
		fscanf(arq, "%s", comando);
		if(comando[0]!='#'){
			if(strcmp("QuantidadeInstancias", comando)==0 ||
			   strcmp("quantInst", comando)==0)
			{
				fscanf(arq, "%d", &quantInstance);
				conf->quantInstance = quantInstance;
			} else
			if(strcmp("Tamanhos", comando)==0 ||
			   strcmp("tam", comando)==0)
			{
				fscanf(arq, "%d", &quantSize);
				conf->quantSize = quantSize;
				size = (int *)malloc(sizeof(int)*quantSize);
				for(i=0; i<quantSize; i++){
					fscanf(arq, "%d", size+i);
				}
				conf->size = size;
			} else
			if(strcmp("SuperDemandas", comando)==0 ||
			   strcmp("sd", comando)==0)
			{
				fscanf(arq, "%d", &quantSD);
				conf->quantSD = quantSD;
				sd = (float *)malloc(sizeof(float)*quantSD);
				for(i=0; i<quantSD; i++){
					fscanf(arq, "%f", sd+i);
				}
				conf->sd = sd;
			}else
			if(strcmp("SuperNos", comando)==0 ||
			   strcmp("sn", comando)==0)
			{
				fscanf(arq, "%d", &quantSN);
				conf->quantSN = quantSN;
				sn = (float *)malloc(sizeof(float)*quantSN);
				for(i=0; i<quantSN; i++){
					fscanf(arq, "%f", sn+i);
				}
				conf->sn = sn;
			}else
			if(strcmp("minPequeno", comando)==0 ||
			   strcmp("minp", comando)==0)
			{
				fscanf(arq, "%f", &minP);
				conf->minP = minP;
			}else
			if(strcmp("maxPequeno", comando)==0 ||
			   strcmp("maxp", comando)==0)
			{
				fscanf(arq, "%f", &maxP);
				conf->maxP = maxP;
			}else
			if(strcmp("minGrande", comando)==0 ||
			   strcmp("ming", comando)==0)
			{
				fscanf(arq, "%f", &minG);
				conf->minG = minG;
			}else
			if(strcmp("maxGrande", comando)==0 ||
			   strcmp("maxg", comando)==0)
			{
				fscanf(arq, "%f", &maxG);
				conf->maxG = maxG;
			}else
			if(strcmp("precision", comando)==0 || 
			   strcmp("prec", comando)==0)
			{
				fscanf(arq, "%d", &precision);
				conf->precision = precision;
			}else
			if(strcmp("seed", comando)==0 ||
			   strcmp("semente", comando)==0)
			{
				fscanf(arq, "%d", &seed);
				conf->seed = seed;
			}else
			if(strcmp("Intervalo", comando)==0 ||
			   strcmp("interv", comando)==0)
			{
				fscanf(arq, "%d", &intervalo);
				conf->intervalo = intervalo;
			}else
			if(strcmp("Diretorio", comando)==0 ||
			   strcmp("dir", comando)==0)
			{
				fscanf(arq, "%s", diretorio);
				strcpy(conf->diretorio, diretorio);
			}else
			if(strcmp("param", comando)==0){
				fscanf(arq, "%s", conf->param);
			}else
			if(strcmp("grauLog", comando)==0){
				fscanf(arq, "%d", &(conf->grauLog));
			}else
			if(strcmp("distMin", comando)==0){
				fscanf(arq, "%f", &(conf->distMin));
			}else
			if(strcmp("distMax", comando)==0){
				fscanf(arq, "%f", &(conf->distMax));
			} else {
				fgets(comentario, 256, arq);
				printf("Comando não reconhecido: %s %s", comando, comentario);
			}
		} else {
			fgets(comentario, 256, arq);
		}
	}while(!feof(arq));
	fclose(arq);
		
	return conf;
}

/* Gera matrizes a partir do arquivo de configuração */
int gerarMatrizes(int argc, char** argv)
{
	ConfigGeraMatTraf conf;
	
	if(argc == 1){
        	printf("\nFalta arquivo de configuração.\nEx.: %s exemplo.conf\n", argv[0]);
        	return 0;
        }
	
	conf = leConf(argv[1]);
	
	if(argc>3)
	{
		if(!strcmp(argv[2],"-s"))
		{
			conf->seed = atoi(argv[3]);
			strcat(conf->diretorio, argv[3]);
		}
	}
	
	srand48(conf->seed);
	
	gmtMatrixGen(conf);
	
	gmtDelConfigGeraMatTraf(conf);
	
	return 0;
}

int main(int argc, char** argv)
{
	return gerarMatrizes(argc, argv);
}


