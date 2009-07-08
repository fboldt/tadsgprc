#ifndef __MATTRAF_IO_H_
#define __MATTRAF_IO_H_
#include "mattraf.h"

/* Grava vetor de MatTraf no formato MatProg */
void mtioGravaVetMatTraf(char *nomeBase, int tamVet, MatTraf *vetMT, char *param)
;

/* Grava matriz no formato de dados em MathProg */
void mtioGravaMatTrafMathProg(char *fileName, MatTraf mt, char *param)
;

/* Retorna o maior elemento da matriz */
float mtioMaxElem(int tam, float **m)
;
/* Retorna a quantidade de casas antes da vírgula do número n */
int mtioAlign(float n)
;

/* Localiza parametro em arquivo de dados MathProg */
int mtioLocalizaParametroInt(char *dados, char *parametro)
;

/* Realiza a leitura de um arquivo de dados do problema LP
Retorna: Estrutura MatTraf preenchida 
AVISO: FUNCAO EM TESTE!!! FUNCIONAMENTO IMPERFEITO!!! */
MatTraf mtioCarregaMatTraf(char *NomeArqDados)
;

#endif


