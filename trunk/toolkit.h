#ifndef __TOOLKIT_H_
#define __TOOLKIT_H_

//================================================================================
// FUNCOES PARA MANIPULACAO DE VETORES E MATRIZES 
//--------------------------------------------------------------------------------

/* Codigo legado */
#define TK_TestaAlocacao tkTestaAlocacao
#define TK_AlocaVetor tkAlocaVetor
#define TK_LiberaVetor tkLiberaVetor
#define TK_TamanhoContiguoVetor tkTamanhoContiguoVetor
#define TK_TamanhoContiguoMatriz tkTamanhoContiguoMatriz
#define TK_MapeiaContiguoMatriz tkMapeiaContiguoMatriz
#define TK_AlocaMatriz tkAlocaMatriz
#define TK_LiberaMatriz tkLiberaMatriz
#define TK_InicializaVetor_int tkInicializaVetor_int
#define TK_InicializaMatriz_char tkInicializaMatriz_char
#define TK_SomaMatriz_float tkSomaMatriz_float
#define TK_CopiaVetor_int tkCopiaVetor_int
#define TK_CopiaMatriz_float tkCopiaMatriz_float
#define TK_PermutacaoVetorAleatorio_int tkPermutacaoVetorAleatorio_int
#define TK_ArranjoVetorAleatorio_int tkArranjoVetorAleatorio_int
#define TK_OrdenaVetor_int tkOrdenaVetor_int
#define Troca tkTroca
#define Maior tkMaior
#define Menor tkMenor
#define Heapify tkHeapify
#define TK_Heapsort tkHeapsort
#define TK_ImprimeVetor_int tkImprimeVetor_int
#define TK_ImprimeMatriz_char tkImprimeMatriz_char
#define TK_ImprimeMatriz_float tkImprimeMatriz_float
#define TK_LeDadosLP tkLeDadosLP

// Testa se a alocacao foi bem sucedida, senao aborta programa
void tkTestaAlocacao(void *p)
;

// Funcao de alocacao dinamica de vetor
void* tkAlocaVetor(int TamVet, int TamTipo)
;

//Funcao para liberar memoria do vetor alocado dinamicamente
void tkLiberaVetor(void *Vetor)
;

// Retorna tamanho de vetor alocado em um espaco contiguo
// O tamanho calculado deve ser múltiplo do tamanho de um
// ponteiro para o alinhamento em diferentes arquiteturas
int tkTamanhoContiguoVetor(int TamVet, int TamTipo)
;

// Retorna tamanho de matriz alocada em um espaco contiguo
// O tamanho calculado deve ser múltiplo do tamanho de um
// ponteiro para o alinhamento em diferentes arquiteturas
int tkTamanhoContiguoMatriz(int NLin, int NCol, int TamTipo)
;

// Mapeia matriz em um espaco contiguo alocado para
// vetor de ponteiros e para os elementos da matriz.
void tkMapeiaContiguoMatriz(int NLin, int NCol, int TamTipo, void **Matriz)
;

/* Funcao que aloca dinamicamente uma matriz
// O vetor de ponteiros e elementos da matriz
// sao alocados em um unico espaco contiguo. */
void** tkAlocaMatriz(int NLin, int NCol, int TamTipo)
;

/* Funcao para liberar memoria da matriz alocada dinamicamente
// Visto que a matriz eh alocada de forma contigua (unindo vetor
// de ponteiros com elementos) libera apenas o primeiro endereco. */
void tkLiberaMatriz(void **Matriz)
;

//--------------------------------------------------------------------------------

// Inicializa vetor int como nulo
void tkInicializaVetor_int(int N, int *Vetor)
;

// Inicializa matriz char como nula explorando vetor contíguo
void tkInicializaMatriz_char(int NLin, int NCol, char **Matriz)
;

// Retorna a soma dos elementos de uma Matriz de Floats
float tkSomaMatriz_float(int NLin, int NCol, float **Matriz)
;


// Funcao que copia vetor de inteiros
void tkCopiaVetor_int(int N, int *VetOrigem, int *VetDestino)
;

// Funcao que copia matriz de float's
void tkCopiaMatriz_float(int NLin, int NCol,
	float **MatOrigem, float **MatDestino)
;
//--------------------------------------------------------------------------------

// Funcao que Permuta vetor inteiro aleatoriamente
// preenchendo o vetor com valores de 0 a (N-1)
void tkPermutacaoVetorAleatorio_int(int TamVetor, int *Vetor)
;

// Funcao que Arranja vetor inteiro aleatoriamente
// onde NElem: numero de elementos a ser arranjado
// e TamVetor: total de elementos do vetor.
// Se NElem=TamVetor, entao Arranjo = Permutacao
void tkArranjoVetorAleatorio_int(int NElem, int TamVetor, int *Vetor)
;

// Funcao que ordena vetor de inteiros - metodo Selecao
void tkOrdenaVetor_int(int N, int *Vetor)
;

//--------------------------------------------------------------------------------

#define PAI(i)   ( (i-1)/2 )
#define F_ESQ(i) ( 2*i+1   )
#define F_DIR(i) ( 2*i+2   )

inline void tkTroca(int *a, int *b)
;

// Funcao que retorna o maior valor entre dois inteiros
int tkMaior(int a, int b)
;

// Funcao que retorna o menor valor entre dois inteiros
int tkMenor(int a, int b)
;

// Eleva o melhor elemento para a raiz da arvore
void tkHeapify(int (Melhor()), int pai, int TamHeap, int *V)
;

// Ordena um vetor de inteiros utilizando o metodo Heapsort
void tkHeapsort(int (Melhor()), int TamVetor, int *Vetor)
;

//--------------------------------------------------------------------------------

// Funcao que imprime um vetor de inteiros
void tkImprimeVetor_int(int N, int *Vetor)
;

// Funcao que imprime uma matriz de char
void tkImprimeMatriz_char(int NLinhas, int NColunas, char **Matriz)
;

// Funcao que imprime uma matriz de float
void tkImprimeMatriz_float(int NLinhas, int NColunas, float **Matriz)
;

//================================================================================

// Realiza a leitura de um arquivo de dados do problema LP
// Retorna:
// a matriz de trafego alocada e preenchida
// o tamanho da rede
void tkLeDadosLP(char *NomeArqDados,
	int *TamanhoRede, float ***MatrizTrafego)
;


#endif


