#include <stdlib.h>
#include <stdio.h>
#include "toolkit.h"

//================================================================================
// FUNCOES PARA MANIPULACAO DE VETORES E MATRIZES 
//--------------------------------------------------------------------------------

// Testa se a alocacao foi bem sucedida, senao aborta programa
void tkTestaAlocacao(void *p)
{
	if (p==NULL)
	{
		printf("Memoria Insuficiente\n");
		exit(1);
	};
}

// Funcao de alocacao dinamica de vetor
void* tkAlocaVetor(int TamVet, int TamTipo)
{
	void *v;
	v = (void*) malloc(tkTamanhoContiguoVetor(TamVet, TamTipo));
	tkTestaAlocacao(v);
	return v;
}

//Funcao para liberar memoria do vetor alocado dinamicamente
void tkLiberaVetor(void *Vetor)
{
	free(Vetor);
}

// Retorna tamanho de vetor alocado em um espaco contiguo
// O tamanho calculado deve ser múltiplo do tamanho de um
// ponteiro para o alinhamento em diferentes arquiteturas
int tkTamanhoContiguoVetor(int TamVet, int TamTipo)
{
	int tam_ponteiro, tam_vetor, alinhamento;
	tam_ponteiro = sizeof(void*);
	tam_vetor = (TamVet * TamTipo);
	// checa alinhamento:
	// (se tamanho da matriz eh multiplo do tamanho de ponteiros)
	// se nao estiver alinhado, alinha para aumentar desempenho.
	alinhamento = tam_vetor % tam_ponteiro;
	if (alinhamento != 0)
		tam_vetor += (tam_ponteiro-alinhamento);
	return tam_vetor;
}

// Retorna tamanho de matriz alocada em um espaco contiguo
// O tamanho calculado deve ser múltiplo do tamanho de um
// ponteiro para o alinhamento em diferentes arquiteturas
int tkTamanhoContiguoMatriz(int NLin, int NCol, int TamTipo)
{
	int tam_ponteiro, tam_matriz, alinhamento;
	tam_ponteiro = sizeof(void*);
	tam_matriz =
		(
			NLin * tam_ponteiro + 
			NLin * NCol * TamTipo
		);
	// checa alinhamento:
	// (se tamanho da matriz eh multiplo do tamanho de ponteiros)
	// se nao estiver alinhado, alinha para aumentar desempenho.
	alinhamento = tam_matriz % tam_ponteiro;
	if (alinhamento != 0)
		tam_matriz += (tam_ponteiro-alinhamento);
	return tam_matriz;
}

// Mapeia matriz em um espaco contiguo alocado para
// vetor de ponteiros e para os elementos da matriz.
void tkMapeiaContiguoMatriz(int NLin, int NCol, int TamTipo, void **Matriz)
{
	int i;
	void **m;
	m = Matriz;
	// salta vetor de ponteiros
	// inicio dos elementos da matriz
	m[0] = (void*) &m[NLin];
	for (i=1; i<NLin; i++)
		m[i] = (m[0]+i*NCol*TamTipo); // endereco da linha i
}

// Funcao que aloca dinamicamente uma matriz
// O vetor de ponteiros e elementos da matriz
// sao alocados em um unico espaco contiguo.
void** tkAlocaMatriz(int NLin, int NCol, int TamTipo)
{
	void** matriz;
	matriz = (void**) malloc
			(
				tkTamanhoContiguoMatriz(NLin, NCol, TamTipo)
			);
	tkTestaAlocacao((void*) matriz);
	tkMapeiaContiguoMatriz(NLin, NCol, TamTipo, matriz);
	return matriz;
}

// Funcao para liberar memoria da matriz alocada dinamicamente
// Visto que a matriz eh alocada de forma contigua (unindo vetor
// de ponteiros com elementos) libera apenas o primeiro endereco.
void tkLiberaMatriz(void **Matriz)
{
	free(Matriz);
}

//--------------------------------------------------------------------------------

// Inicializa vetor int como nulo
void tkInicializaVetor_int(int N, int *Vetor)
{
	int i;
	for(i=0; i<N; i++)
		Vetor[i] = 0;
}

// Inicializa matriz char como nula explorando vetor contíguo
void tkInicializaMatriz_char(int NLin, int NCol, char **Matriz)
{
	int i;
	for(i=0; i<NLin*NCol; i++)
		Matriz[0][i] = 0;
}

// Retorna a soma dos elementos de uma Matriz de Floats
float tkSomaMatriz_float(int NLin, int NCol, float **Matriz)
{
	int i;
	float soma=0.0;
	for(i=0; i<NLin*NCol; i++)
		soma += Matriz[0][i];
	return soma;
}


// Funcao que copia vetor de inteiros
void tkCopiaVetor_int(int N, int *VetOrigem, int *VetDestino)
{
	int k;
	for (k=0; k<N; k++)
		VetDestino[k] = VetOrigem[k];
}

// Funcao que copia matriz de float's
void tkCopiaMatriz_float(int NLin, int NCol,
	float **MatOrigem, float **MatDestino)
{
	int i, j;
	for (i=0; i<NLin; i++)
		for (j=0; j<NCol; j++)
			MatDestino[i][j] = MatOrigem[i][j];
}
//--------------------------------------------------------------------------------

// Funcao que Permuta vetor inteiro aleatoriamente
// preenchendo o vetor com valores de 0 a (N-1)
void tkPermutacaoVetorAleatorio_int(int TamVetor, int *Vetor)
{
	// Permutacao(N) = Arranjo(N,N)
	tkArranjoVetorAleatorio_int(TamVetor, TamVetor, Vetor);
}

// Funcao que Arranja vetor inteiro aleatoriamente
// onde NElem: numero de elementos a ser arranjado
// e TamVetor: total de elementos do vetor.
// Se NElem=TamVetor, entao Arranjo = Permutacao
void tkArranjoVetorAleatorio_int(int NElem, int TamVetor, int *Vetor)
{
	int valor, ordem, k, r, tmp;
	// sorteia ordem de preenchimento no vetor crescente/decrescente
	ordem = rand()%2;
	// sorteia valor inicial para preencher vetor
	valor = rand()%TamVetor;
	// inicializa vetor na ordem sorteada partindo de valor inicial
	for(k=0; k<TamVetor; k++)
	{
		valor += (ordem - !ordem);
		Vetor[k] = (valor + TamVetor) % TamVetor;
	}
	// Arranja vetor (NElem) vezes
	for(k=0; k<NElem; k++)
	{
		r = rand()%TamVetor; // sorteia elemento a ser trocado
		tmp      = Vetor[k];
		Vetor[k] = Vetor[r];
		Vetor[r] = tmp;
	}
}

// Funcao que ordena vetor de inteiros - metodo Selecao
void tkOrdenaVetor_int(int N, int *Vetor)
{
	int i, j, min, tmp;
	for (i=0; i<N-1; i++)
	{
		min=i;
		for (j=i+1; j<N; j++)
			if (Vetor[j] < Vetor[min])
				min = j;
		tmp = Vetor[i];
		Vetor[i] = Vetor[min];
		Vetor[min] = tmp;
	}	
}

//--------------------------------------------------------------------------------

#define PAI(i)   ( (i-1)/2 )
#define F_ESQ(i) ( 2*i+1   )
#define F_DIR(i) ( 2*i+2   )

inline void tkTroca(int *a, int *b)
{
	int tmp;
	tmp = *a;
	*a  = *b;
	*b  = tmp;
}

// Funcao que retorna o maior valor entre dois inteiros
int tkMaior(int a, int b)
{
	int maior;
	maior = (a > b) ? 1 : 0;
	return (maior);
}

// Funcao que retorna o menor valor entre dois inteiros
int tkMenor(int a, int b)
{
	int menor;
	menor = (a < b) ? 1 : 0;
	return (menor);
}

// Eleva o melhor elemento para a raiz da arvore
void tkHeapify(int (Melhor()), int pai, int TamHeap, int *V)
{
	int esq, dir, melhor, continua;
	do
	{
		esq = F_ESQ(pai);
		dir = F_DIR(pai);
		if ((esq < TamHeap) && (Melhor(V[esq], V[pai])))
			melhor = esq;
		else
			melhor = pai;
		if ((dir < TamHeap) && (Melhor(V[dir], V[melhor])))
			melhor = dir;
		continua = (pai != melhor);
		if (continua)
		{
			Troca(&V[pai], &V[melhor]);
			pai = melhor;
		};
	} while (continua) ;
}

// Ordena um vetor de inteiros utilizando o metodo Heapsort
void tkHeapsort(int (Melhor()), int TamVetor, int *Vetor)
{
	int i, TamHeap=TamVetor, tmp;
	// Constroi a arvore Heap
	TamHeap = TamVetor;
	for (i = PAI(TamVetor-1); i>=0; i--)
		Heapify(Melhor, i, TamHeap, Vetor);
	// Transforma a arvore Heap em um vetor ordenado
	for (i=TamVetor-1; i>0; i--)
	{
		tmp = Vetor[0];
		Vetor[0] = Vetor[i];
		Vetor[i] = tmp;
		TamHeap--;
		Heapify(Melhor, 0, TamHeap, Vetor);
	} 
}

//--------------------------------------------------------------------------------

// Funcao que imprime um vetor de inteiros
void tkImprimeVetor_int(int N, int *Vetor)
{
	int k;
	for(k=0; k<N; k++)			
	{
		printf("%4d ", Vetor[k]);
	}
	printf("\n");
}

// Funcao que imprime uma matriz de char
void tkImprimeMatriz_char(int NLinhas, int NColunas, char **Matriz)
{
	int i, j;
	for (i=0; i<NLinhas; i++)
	{
		for (j=0; j<NColunas; j++)
		{
			printf("%4d ", (int)Matriz[i][j]);
		}
		printf("\n");
	}
}

// Funcao que imprime uma matriz de float
void tkImprimeMatriz_float(int NLinhas, int NColunas, float **Matriz)
{
	int i, j;
	for (i=0; i<NLinhas; i++)
	{
		for (j=0; j<NColunas; j++)
		{
			printf("%.4f ", Matriz[i][j]);
		}
		printf("\n");
	}
}

//================================================================================

// Realiza a leitura de um arquivo de dados do problema LP
// Retorna:
// a matriz de trafego alocada e preenchida
// o tamanho da rede
void tkLeDadosLP(char *NomeArqDados,
	int *TamanhoRede, float ***MatrizTrafego)
{
	int i, j, n;
	char s[1024];
	float valor;
	FILE *ArqDados;
	int TamRede;
	float **Matriz;

	// Abre arquivo de dados somente para leitura
	ArqDados = fopen(NomeArqDados, "r");
	if (ArqDados == NULL)
	{
		printf("Não foi possível ler arquivo de entrada...\n");
		exit(-1);
	}
	// Le arquivo de dados para retornar o tamanho da rede
	// e a matriz de trafego alocada e preenchida
	fscanf(ArqDados, "%s%s%s%d%s", s, s, s, &n, s);
	TamRede = n;
	Matriz = (float**) tkAlocaMatriz(TamRede, TamRede, sizeof(float));
	fgets(s, 1024, ArqDados);
	fgets(s, 1024, ArqDados);
	// le arquivo de dados preenchendo a matriz de trafego
	for (i=0; i<n; i++)
	{
		fscanf(ArqDados, "%s", s);
		for (j=0; j<n; j++)
		{
			fscanf(ArqDados, "%f", &valor);
			Matriz[i][j] = valor;
		}
	}
	fclose(ArqDados);
	*TamanhoRede = TamRede;
	*MatrizTrafego = Matriz;
}

