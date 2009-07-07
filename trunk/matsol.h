#ifndef __MATSOL_H_
#define __MATSOL_H_

typedef struct matSol
{
	char **b;
	int *qLin, *qCol;
	int ordem;
} *MatSol;

/* =============================================== */



/* Atribui zero a todas posicoes da matriz em ms */
void msZeraMatSol(MatSol ms)
;

/* Construtor MatSol */
MatSol msNewMatSol(int ordem)
;

/* Destrutor MatSol */
void msDelMatSol(MatSol ms)
;

/* Preenche um vetor de int de zero a tam-1 */
void msPreencheVetorOrdenado(int tam, int *anel)
;

/* Troca os valores de dois ponteiros de int */
void msTrocaValInt(int *a, int *b)
;

/* Mistura valores de um vetor de int */
void msMisturaVetor(int tam, int *anel)
;

/* Incrementa 1 na posicao (i,j) de b */
void msIncrementaMatSol(MatSol ms, int i, int j)
;

/* Decrementa 1 na posicao (i,j) de b */
void msDecrementaMatSol(MatSol ms, int i, int j)
;

/* Atribui val na posicao (i,j) de b */
void msAtribuiMatSol(MatSol ms, int i, int j, int val)
;

/* Adiciona enlace atribuindo 1 na posicao (i,j) de b */
void msAdicionaEnlace(MatSol ms, int i, int j)
;

/* Adiciona enlace atribuindo 1 na posicao (i,j) de b restringindo a um valor maximo e a um grau logico */
void msAdicionaEnlaceMaxGL(MatSol ms, int i, int j, int max, int gl)
;

/* Remove enlace atribuindo 0 na posicao (i,j) de b */
int msRemoveEnlace(MatSol ms, int i, int j)
;

/* Atribui um vetor como anel em ms */
void msAtribuiAnelMatSol(MatSol ms, int *anel)
;

/* Atribui um vetor como anel em ms com valor maximo*/
void msAtribuiAnelMatSolMax(MatSol ms, int *anel, int max, int gl)
;

/* Atribui um vetor como anel full duplex em ms com valor maximo*/
void msAtribuiAnelMatSolMaxSimetrico(MatSol ms, int *anel, int max, int gl)
;

/* Preeche vetor com valores aleatorios nao repetidos de zero a tam-1 */
void msPreencheVetorAleatorio(int tam, int *anel)
;

/* Atribui anel basico em ms */
void msAnelBasico(MatSol ms)
;

/* Imprime no terminal de texto uma MatSol */
void msImprimeMatSol(MatSol ms)
;

/* Cria matriz aleatoria em malha */
MatSol msRLDAMalhaGL(int ordem, int gl)
;

/* Cria matriz aleatoria simetrica em malha */
MatSol msRLDAMalhaGLSimetrico(int ordem, int gl)
;

/* Mede o grau de similaridade entre duas MatSol com valores entre 0 e 1 */
float msSimilaridade(MatSol ms1, MatSol ms2)
;

/* Atribui a ms uma topologia totalmente conectada */
void msTotalmenteConexa(MatSol ms)
;

#endif


