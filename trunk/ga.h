#ifndef __GA_H_
#define __GA_H_


#define MAX_PONTOS_CORTE (MAX_TAM_REDE-1)
#define MAX_TAM_POP      (200)
#include "optical_network.h"
#include "ga.h"
#include "glpk.h"


typedef struct
{
	double Fitness;
	double Avaliacao;
	TOptNet *OptNet;

} TIndividuo;


typedef struct
{
	// Fitness dos Individuos
	float MinFitness;	// Minimo
	float MaxFitness;	// Maximo
	float SomaFitness;	// Somatorio

	// Avaliacoes dos individuos
	float MinAvaliacao;		// Minimo
	float MaxAvaliacao;		// Maximo
	float SomaAvaliacao;	// Somatorio

	// Modelo LP GLPK
	LPX *lp;
	// Posicao da variavel "B[1,1]" no modelo GLPK
	int PosVariavel;

	// Grau Limite da Matriz de Topologia Virtual
	int GrauLimite;

	int TamRede;		// Numero de Nos da Rede
	int *AnelBasico;	// Anel Basico dos individuos da populacao

	// Vetor Individuos (Ind) eh dividido em duas partes:
	// a primeira, que  armazena os individuos da populacao e 
	// a segunda que armazena os individuos filhos
	int TamInd;			// Tamanho de Individuos (Pop+Filhos)
	TIndividuo **Ind;	// Vetor Ponteiros para todos os Individuos

	int TamPop;			// Tamanho de Pop (Populacao)
	TIndividuo **Pop;	// Endereco da PRIMEIRA parte de Ind

	int TamFilhos;		// Tamanho de Filhos
	TIndividuo **Filhos;// Endereco da SEGUNDA parte de Ind

	int TamPais;		// Tamanho de Pais
	TIndividuo **Pais;	// Vetor de Ponteiros selecionados de Pop

} TPopulacao;



#define INF_POS	+1E37
#define INF_NEG	-1E37 


// Aloca Individuo:
// Matriz de Topologia Virtual e
// Vetores Grau Logico de Saida e Entrada
TIndividuo *GA_AlocaIndividuo(int TamRede)
;



// Desaloca Individuo:
// como o individuo foi alocado em um unico espaco
// contiguo, libera apenas a primeira posicao
void GA_LiberaIndividuo(TIndividuo *p)
;



// Aloca um vetor de ponteiros para individuos
// e suas respectivas matrizes
// TamRede: Numero de nos da rede
// TamInd:  Numero de elementos do vetor Individuos
TIndividuo **GA_AlocaVetorIndividuos(int TamRede, int TamInd)
;

// Libera vetor de ponteiros para individuos.
// como o vetor de individuos foi alocado em um unico
// espaco contiguo, libera apenas a primeira posicao.
void GA_LiberaVetorIndividuos(TIndividuo **Ind)
;



// Imprime Individuo:
// Fitness, Avaliacao, Matriz de Topologia Virtual e
// Vetores Grau Logico de Saida e Grau Logico de Entrada
void GA_ImprimeIndividuo(int TamRede, TIndividuo *p)
;

// Imprime um vetor de Individuos
void GA_ImprimeVetorIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
;


// Inicializa campos do individuo,
// mapeia anel basico e
// completa Matriz de Topologia Virtual
void GA_GeraIndividuo(int GrauLimite, int N, int *Anel, TIndividuo *p)
;

// Gera Vetor de Individuos
void GA_GeraVetorIndividuos(int GrauLimite, int TamRede, int *Anel,
	int TamInd, TIndividuo **Ind)
;

// Permuta um vetor de individuos pre-existentes
// Pre: Entra com Vetor de Individuos Preenchido
// Pos: Retorna um vetor com os individuos permutados aleatoriamente
void GA_PermutaVetorAleatorio_TIndividuo(int TamVetor, TIndividuo **Vetor)
;

// Arranja um vetor de individuos
// Pre: Entra com Vetor de Individuos PREVIAMENTE Preenchido
// Pos: Retorna um vetor com as M primeiras posicoes arranjadas aleatoriamente
void GA_ArranjoVetorAleatorio_TIndividuo(int M, int TamVetor, TIndividuo **Vetor)
;

// Faz uma copia do individuo para outra alocacao de memoria
void GA_CopiaIndividuo(int TamRede, TIndividuo *Ind, TIndividuo *Ind2)
;


// Entre com dois ponteiros para individuos e troca um com outro
void GA_TrocaIndividuos(TIndividuo **Ind1, TIndividuo **Ind2)
;



//TODO verificar as partes dessa função que deveriam ser resposabilidade de Redes opticas
// Funcao calcula fitness de um individuo (uso do GLPK)
float GA_FitnessIndividuo(LPX *lp, int PosVariavel, int TamRede, TIndividuo *p)
;

// Calcula fitness de um vetor de individuos (uso do GLPK)
// retorna vetor de individuos com o Fitness calculado
void GA_FitnessVetorIndividuos(LPX *lp, int PosVariavel,
		int TamRede, int TamInd, TIndividuo **Ind)
;


// Ordena vetor de Individuos pelo FITNESS - metodo Selecao
// ordem crescente: MENOR valor FITNESS -> MELHOR individuo
void GA_OrdenaVetorIndividuos(int TamInd, TIndividuo **Ind)
;



//================================================================================

// Retorna o espaco (em bytes) necessario para um
// individuo ser alocado em um espaco contiguo.
//TODO MODIFICADO
int GA_TamanhoContiguoIndividuo(int TamRede)
;






// Mapeia Individuo em um espaco alocado contiguo (*Ind).
// Matriz e vetores de (**Ind) sao mapeados apos a rotina.
// O espaco contiguo devera ser suficiente para comportar:
// - Estrutura TIndividuo
// - Matriz TopologiaVirtual
// - Vetor GrauLogicoSaida
// - Vetor GrauLogicoEntrada
//TODO FUNCAO MODIFICADA
void GA_MapeiaContiguoIndividuo(int TamRede, TIndividuo *p)
;






// Retorna tamanho de um Vetor de Individuos Contiguo
int GA_TamanhoContiguoVetorIndividuos(int TamRede, int TamInd)
;

// Mapeia Vetor de Individuos em um espaco alocado contiguo.
void GA_MapeiaContiguoVetorIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
;


// Inicializa campos do individuo
void GA_InicializaIndividuo(int N, TIndividuo *p)
;
//-----------------------------------------------------------------------------


//TODO VERIFICAR
// Aborta programa caso algum parametro esteja fora da faixa
void GA_ErroParametro(const char *param, int codigoerro)
;
//TODO OK
// Checa os parametros de entrada e em caso de erro, aborta programa
void GA_ChecaParametrosPopulacao(int GrauLimite, int TamRede, int TamPop)
;


//TODO VERIFICAR
// Aloca Memoria e Gera uma populacao
TPopulacao *GA_CriaPopulacao(
	int GrauLimite, int TamRede, int *AnelBasico, int TamPop,
	char NomeArqModelo[], char NomeArqDadosEntrada[], char NomeArqDadosSaida[],
	char NomeVariavel[])
;
//TODO VERIFICAR
// Libera a memoria alocada pela Populacao
void GA_DestroiPopulacao(TPopulacao *P)
;


//---------------------------------------------------------------------
//TODO VERIFICAR
// Realiza o Crossover a partir de dois individuos pais, para dois individuos filhos
// com o numero de pontos de corte especificado
void GA_CrossoverIndividuo(int NumPontosCorte, int N, int *Anel, int GrauLimite,
	TIndividuo *Pai1,   TIndividuo *Pai2,
	TIndividuo *Filho1, TIndividuo *Filho2)
;

//TODO VERIFICAR
// Crossover dos Pais Selecionados
void GA_Crossover(int NPontosCorte, TPopulacao *P)
;

// Gera Mutacao em um individuo, adicionando (ou removendo) enlaces
// NumMutacoes eh o numero de mutacoes - enlaces a serem alterados
void GA_MutacaoIndividuo(int NumMutacoes,
	int GrauLimite, int TamRede, int *Anel, TIndividuo *p)
;

// Mutacao dos Filhos Gerados
void GA_Mutacao(int NumMutacoes, TPopulacao *P)
;


// Calcula o Fitness e a Pre-Avaliacao do vetor de Individuos Ind
void GA_AvaliaInd(TPopulacao *P)
;

// Calcula o Fitness e a Pre-Avaliacao do vetor de Individuos Ind
void GA_AvaliaFilhos(TPopulacao *P)
;



// Calcula Avaliacao dos individuos de Pop para que os pais
// possam ser selecionados por este valor na funcao selecao.
// Avalicao atribui o MAIOR valor ao MELHOR individuo.
// Antes de avaliar, realiza uma pre-avaliacao no vetor Pop.
// Retorna o desvio-padrao (Sigma) do Fitness de Pop
// PRE: campo Fitness dos individuos (Pop) deve estar calculado
float GA_AvaliaPop(TPopulacao *P)
;

//---------------------------------------------------------------------

// Seleciona em Ind (Pop+Filhos), quais individuos
// farao parte da nova Pop (sobreviventes).
// Os valores min, max e soma de fitness sao setados
void GA_SelecionaSobreviventes(TPopulacao *P)
;

// Seleciona os Pais para realizarem Crossover
void GA_SelecionaPais(TPopulacao *P)
;

//---------------------------------------------------------------------

// Calcula a pre-avaliacao de todos os individuos em funcao do Fitness e da
// direcao de otimizacao (MIN ou MAX) para que o maior valor corresponda ao
// melhor individuo. Em Seguida, varre vetor de individuos (Ind) em busca 
// dos valores Minimo, Maximo e Somatorio de Fitness e Avaliacao de (Ind).
void GA_PreAvaliacao(int TamInd, TIndividuo **Ind, TPopulacao *P)
;

//---------------------------------------------------------------------

// Retorna o Desvio Padrao (sigma) da Avaliacao da populacao (Pop)
float GA_CalculaSigmaAvaliacao(TPopulacao *P)
;

// Avaliacao dos individuos usando a tecnica de Normalizacao
float GA_Avaliacao_Normalizacao(float AvaliacaoMax, float Decremento, TPopulacao *P)
;

// Avaliacao dos individuos usando a tecnica de Windowing
// PercentualMinAvaliacao (<100%) eh usado para o 
// calculo do valor base a ser subtraido - tipico ~0.99 ou ~99%
float GA_Avaliacao_Windowing(float PercentualMinAvaliacao, TPopulacao *P)
;

// Avaliacao dos individuos usando a tecnica de Escalonamento Sigma
float GA_Avaliacao_Sigma(TPopulacao *P)
;

//---------------------------------------------------------------------

// Seleciona pelo metodo da roleta viciada
void GA_Selecao_RoletaViciada(TPopulacao *P)
;

// Seleciona pelo metodo do Torneio
void GA_Selecao_Torneio(int TamTorneio, TPopulacao *P)
;

// Seleciona pelo metodo da Amostragem Estocastica Uniforme
void GA_Selecao_AmostragemEstocasticaUniforme(TPopulacao *P)
;

//---------------------------------------------------------------------

// Imprime os pais selecionados para Crossover
void GA_ImprimePaisSelecionados(TPopulacao *P)
;

// Imprime os individuos de um vetor Ind
void GA_ImprimeIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
;

//---------------------------------------------------------------------

#endif


