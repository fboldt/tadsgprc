#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "toolkit.h"
#include "optical_network.h"
#include "ga.h"
#include "glpkwrapper.h"

#define INF_POS	+1E37
#define INF_NEG	-1E37 


// Aloca Individuo:
// Matriz de Topologia Virtual e
// Vetores Grau Logico de Saida e Entrada
TIndividuo *GA_AlocaIndividuo(int TamRede)
{
	TIndividuo *p;
	// Alocacao de espaco contiguo
	p = (TIndividuo*) malloc
			(
				GA_TamanhoContiguoIndividuo(TamRede)
			);
	// Mapeia individuo no espaco contiguo alocado
	GA_MapeiaContiguoIndividuo(TamRede, p);

	return p;
}



// Desaloca Individuo:
// como o individuo foi alocado em um unico espaco
// contiguo, libera apenas a primeira posicao
void GA_LiberaIndividuo(TIndividuo *p)
{
	free(p);
}



// Aloca um vetor de ponteiros para individuos
// e suas respectivas matrizes
// TamRede: Numero de nos da rede
// TamInd:  Numero de elementos do vetor Individuos
TIndividuo **GA_AlocaVetorIndividuos(int TamRede, int TamInd)
{
	TIndividuo **Ind;

	// Alocacao de espaco contiguo
	Ind = (TIndividuo**) malloc
			(
				GA_TamanhoContiguoVetorIndividuos(TamRede, TamInd)
			);
			
	// Mapeamento do Vetor de Individuos em espaco contiguo
	GA_MapeiaContiguoVetorIndividuos(TamRede, TamInd, Ind);

	return Ind;
}

// Libera vetor de ponteiros para individuos.
// como o vetor de individuos foi alocado em um unico
// espaco contiguo, libera apenas a primeira posicao.
void GA_LiberaVetorIndividuos(TIndividuo **Ind)
{
	free(Ind);
}



// Imprime Individuo:
// Fitness, Avaliacao, Matriz de Topologia Virtual e
// Vetores Grau Logico de Saida e Grau Logico de Entrada
void GA_ImprimeIndividuo(int TamRede, TIndividuo *p)
{
	printf("%f\n%f\n", p->Fitness, p->Avaliacao);
	OPT_ImprimeTOptNet(TamRede, p->OptNet);
	printf("\n");
}

// Imprime um vetor de Individuos
void GA_ImprimeVetorIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
{
	int k;
	for (k=0; k<TamInd; k++)
		GA_ImprimeIndividuo(TamRede, Ind[k]);
}


// Inicializa campos do individuo,
// mapeia anel basico e
// completa Matriz de Topologia Virtual
void GA_GeraIndividuo(int GrauLimite, int N, int *Anel, TIndividuo *p)
{
	GA_InicializaIndividuo(N, p);
	OPT_MapeiaAnelMatrizTopologiaVirtual(N, Anel, p->OptNet);
	OPT_CompletaMatrizTopologiaVirtual(GrauLimite, N, p->OptNet);
}

// Gera Vetor de Individuos
void GA_GeraVetorIndividuos(int GrauLimite, int TamRede, int *Anel,
	int TamInd, TIndividuo **Ind)
{
	int k;
	for (k=0; k<TamInd; k++)
		GA_GeraIndividuo(GrauLimite, TamRede, Anel, Ind[k]);
}

// Permuta um vetor de individuos pre-existentes
// Pre: Entra com Vetor de Individuos Preenchido
// Pos: Retorna um vetor com os individuos permutados aleatoriamente
void GA_PermutaVetorAleatorio_TIndividuo(int TamVetor, TIndividuo **Vetor)
{
	GA_ArranjoVetorAleatorio_TIndividuo(TamVetor, TamVetor, Vetor);
}

// Arranja um vetor de individuos
// Pre: Entra com Vetor de Individuos PREVIAMENTE Preenchido
// Pos: Retorna um vetor com as M primeiras posicoes arranjadas aleatoriamente
void GA_ArranjoVetorAleatorio_TIndividuo(int M, int TamVetor, TIndividuo **Vetor)
{
	int k, r;
	TIndividuo *individuo;
	// Arranja vetor (M) vezes
	for(k=0; k<M; k++)
	{
		r = rand()%TamVetor; // sorteia elemento a ser trocado
		individuo = Vetor[k];
		Vetor[k]  = Vetor[r];
		Vetor[r]  = individuo;
	}
}

// Faz uma copia do individuo para outra alocacao de memoria
void GA_CopiaIndividuo(int TamRede, TIndividuo *Ind, TIndividuo *Ind2){
	int *I1,*I2;
	int TamInd = GA_TamanhoContiguoIndividuo(TamRede); 
	int i;
	I1 = Ind;
	I2 = Ind2;
	for(i=0;i<TamInd/sizeof(int);i++)
		I2[i]=I1[i];
	GA_MapeiaContiguoIndividuo(TamRede, Ind2);
}

// Entre com dois ponteiros para individuos e troca um com outro
void GA_TrocaIndividuos(TIndividuo **Ind1, TIndividuo **Ind2)
{
	TIndividuo *P;
	P = *Ind1;
	*Ind1 = *Ind2;
	*Ind2 = P;
}

//TODO verificar as partes dessa função que deveriam ser resposabilidade de Redes opticas
// Funcao calcula fitness de um individuo (uso do GLPK)
float GA_FitnessIndividuo(LPX *lp, int PosVariavel, int TamRede, TIndividuo *p)
{
	int k, tam, lin, col;
	// Como a alocacao de variaveis no GLPK eh linear, 
	// ao inves de varrer ij ate n, varre-se i ate n*n.
	tam = TamRede*TamRede;
	for(k=0; k<tam; k++)
	{
		lin = k/TamRede;
		col = k%TamRede;
		// na funcao lpx_set_col_bnds, type eh o tipo de limitacao das variaveis.
		// LPX_FX significa LB = x = UB (lower bound = upper bound)
		// O GLPK faz a indexacao de modo linear e a partir de 1.
		// desta forma, eh necessario incrementar 1 ao contador: (k+1).
		lpx_set_col_bnds(lp, (PosVariavel+k), LPX_FX,
			p->OptNet->TopologiaVirtual[lin][col],
			p->OptNet->TopologiaVirtual[lin][col]); 
	}
	// Roda o simplex e testa erro
	if (lpx_simplex(lp) != LPX_E_OK)
	{           
		printf("Nao foi possivel executar o Simplex para a topologia.\n");
		exit(2);
	}
	p->Fitness = lpx_get_obj_val(lp);
	return p->Fitness;
}

// Calcula fitness de um vetor de individuos (uso do GLPK)
// retorna vetor de individuos com o Fitness calculado
void GA_FitnessVetorIndividuos(LPX *lp, int PosVariavel,
		int TamRede, int TamInd, TIndividuo **Ind)
{
	int k;
	for (k=0; k<TamInd; k++)
		GA_FitnessIndividuo(lp, PosVariavel, TamRede, Ind[k]);
}

// Ordena vetor de Individuos pelo FITNESS - metodo Selecao
// ordem crescente: MENOR valor FITNESS -> MELHOR individuo
void GA_OrdenaVetorIndividuos(int TamInd, TIndividuo **Ind)
{
	int i, j, min, N=TamInd;
	TIndividuo *tmp, **Vetor = Ind;
	for (i=0; i<N-1; i++)
	{
		min=i;
		for (j=i+1; j<N; j++)
			if (Vetor[j]->Fitness < Vetor[min]->Fitness)
				min = j;
		tmp = Vetor[i];
		Vetor[i] = Vetor[min];
		Vetor[min] = tmp;
	}	
}

//================================================================================

// Retorna o espaco (em bytes) necessario para um
// individuo ser alocado em um espaco contiguo.
int GA_TamanhoContiguoIndividuo(int TamRede)
{
	int NLin, NCol, tam;
	NLin = NCol = TamRede;
	tam =
		(
			// Estrutura Individuo
			sizeof(TIndividuo) +
			// Estrutura Redes Opticas
			OPT_TamanhoContiguoOptNet(TamRede)
		);
	return tam;
}

// Mapeia Individuo em um espaco alocado contiguo (*Ind).
// Matriz e vetores de (**Ind) sao mapeados apos a rotina.
// O espaco contiguo devera ser suficiente para comportar:
// - Estrutura TIndividuo
// - Matriz TopologiaVirtual
// - Vetor GrauLogicoSaida
// - Vetor GrauLogicoEntrada
void GA_MapeiaContiguoIndividuo(int TamRede, TIndividuo *p)
{
	void *posicao;
	// posicao da Estrutura TOptNet apos estrutura TIndividuo
	posicao  = (void*) p + (sizeof(TIndividuo));
	p->OptNet = posicao;
	OPT_MapeiaContiguoOptNet(TamRede, p->OptNet);
}

// Retorna tamanho de um Vetor de Individuos Contiguo
int GA_TamanhoContiguoVetorIndividuos(int TamRede, int TamInd)
{
	int tam =
		(TamInd * sizeof(TIndividuo*)) +
		(TamInd * GA_TamanhoContiguoIndividuo(TamRede));
	return tam;
}

// Mapeia Vetor de Individuos em um espaco alocado contiguo.
void GA_MapeiaContiguoVetorIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
{
	int k;
	// Mapeamento do vetor de ponteiros para individuos
	TK_MapeiaContiguoMatriz(TamInd, 1,GA_TamanhoContiguoIndividuo(TamRede), (void**) Ind);
	// Mapeamento dos vetores e matriz de cada individuo
	for (k=0; k<TamInd; k++){
		GA_MapeiaContiguoIndividuo(TamRede, Ind[k]);
	}

}

// Inicializa campos do individuo
void GA_InicializaIndividuo(int N, TIndividuo *p)
{
	p->Fitness   = 0.0;
	p->Avaliacao = 0.0;
	TK_InicializaMatriz_char(N, N, p->OptNet->TopologiaVirtual);
	TK_InicializaVetor_int(N, p->OptNet->GrauLogicoSaida);
	TK_InicializaVetor_int(N, p->OptNet->GrauLogicoEntrada);
}

//-----------------------------------------------------------------------------

// Aborta programa caso algum parametro esteja fora da faixa
void GA_ErroParametro(const char *param, int codigoerro)
{
	printf("Erro: %s..\n", param);
	exit(codigoerro);
}
// Checa os parametros de entrada e em caso de erro, aborta programa
void GA_ChecaParametrosPopulacao(int GrauLimite, int TamRede, int TamPop)
{
	if (!(GrauLimite > 0 && GrauLimite < TamRede))
		GA_ErroParametro("GrauLimite fora da faixa", 101);
	if (!(TamRede > 0 && TamRede < MAX_TAM_REDE))
		GA_ErroParametro("TamRede fora da faixa",    102);
	if (!(TamPop > 0))
		GA_ErroParametro("TamPop fora da faixa",     103);
//	if (!(TamPop%2 == 0))
//		ErroParametro("TamPop deve ser par",      104);
}

// Aloca Memoria e Gera uma populacao
TPopulacao *GA_CriaPopulacao(
	int GrauLimite, int TamRede, int *AnelBasico, int TamPop,
	char NomeArqModelo[], char NomeArqDadosEntrada[], char NomeArqDadosSaida[],
	char NomeVariavel[])
{
	TPopulacao *P;

	// Checa se os parametros de entrada sao validos
	GA_ChecaParametrosPopulacao(GrauLimite, TamRede, TamPop);
	
	// Alocacao

	// Aloca TPopulacao
	P = (TPopulacao*) malloc(sizeof(TPopulacao));
	// Vetor de Inteiros que armazena o Anel Basico
	P->AnelBasico = (int*) TK_AlocaVetor(TamRede, sizeof(int));
	// Vetor de ponteiros para estruturas alocadas que armazenam os individuos

	P->Ind        = GA_AlocaVetorIndividuos(TamRede, (TamPop+TamPop));
	// Vetor de ponteiros para armazenar enderecos alocados no Individuo
	P->Pais       = (TIndividuo**) TK_AlocaVetor(TamPop, sizeof(TIndividuo*));

	// Posiciona ponteiros Pop e Filhos
	P->Pop    = &P->Ind[0];
	P->Filhos = &P->Ind[TamPop];

	// Incializacao

	// Fitness
	P->MinFitness = INF_POS;
	P->MaxFitness = INF_NEG;
	P->SomaFitness = 0.0;
	// Avaliacao
	P->MinAvaliacao = INF_POS;
	P->MaxAvaliacao = INF_NEG;
	P->SomaAvaliacao = 0.0;

	// Le Modelo GLPK
	P->lp = TK_LeModeloLP(NomeArqModelo, NomeArqDadosEntrada, NomeArqDadosSaida);
	// Seta Paramentros de Inicializacao
	TK_SetaParametrosLP(P->lp);
	// Cria Indice de variaveis do Modelo LP
	TK_CriaIndiceLP(P->lp);
	// Encontra Posicao da Variavel no Modelo LP
//	P->PosVariavel = TK_PosicaoVariavelLP(NomeVariavel, P->lp);
	P->PosVariavel = gwPosicaoVariavelLP(P->lp, NomeVariavel);
	// Grau Logico Limite (Maximo)
	P->GrauLimite = GrauLimite;
	// Quantidade de Nos da Rede
	P->TamRede = TamRede;
	// Tamanho dos Vetores
	P->TamInd    = (TamPop+TamPop);	// (Pop+Filhos)
	P->TamPop    = TamPop;
	P->TamFilhos = TamPop;
	P->TamPais   = TamPop;	

	// Copia Anel Basico para estrutura interna da Populacao
	TK_CopiaVetor_int(P->TamRede, AnelBasico, P->AnelBasico);
	
	// Gera Vetor de Individuos da Populacao (Pop+Filhos)
	GA_GeraVetorIndividuos(P->GrauLimite, P->TamRede, P->AnelBasico, P->TamInd, P->Ind);

	return P;
}
// Libera a memoria alocada pela Populacao
void GA_DestroiPopulacao(TPopulacao *P)
{
	// Finaliza Modelo LP do GLPK
	TK_FinalizaLP(P->lp);
	// Libera areas de memoria alocadas pela populacao	
	TK_LiberaVetor(P->AnelBasico);
	GA_LiberaVetorIndividuos(P->Ind);
	TK_LiberaVetor(P->Pais);
	free(P);
}

//---------------------------------------------------------------------
// Realiza o Crossover a partir de dois individuos pais, para dois individuos filhos
// com o numero de pontos de corte especificado
void GA_CrossoverIndividuo(int NumPontosCorte, int N, int *Anel, int GrauLimite,
	TIndividuo *Pai1,   TIndividuo *Pai2,
	TIndividuo *Filho1, TIndividuo *Filho2)
{
	int i, inicio, fim, indice;
	// um a mais para armazenar o elemento da ultima secao
	int pontosdecorte[MAX_PONTOS_CORTE+1];

	void (*CopiaVetor)();
	void (*CorrigeGrauLogico)();

	// sorteia se crossover sera de linhas ou colunas
	if (rand()%2)
	{
		CopiaVetor        = OPT_CopiaLinhaTopologia;
		CorrigeGrauLogico = OPT_CorrigeGrauLogicoEntrada;
	}
	else
	{
		CopiaVetor        = OPT_CopiaColunaTopologia;
		CorrigeGrauLogico = OPT_CorrigeGrauLogicoSaida;
	};

	// Gera aleatoriamente os pontos de cortes: Arranjo(NPC, N-1)
	TK_ArranjoVetorAleatorio_int(NumPontosCorte, N-1, pontosdecorte);
	TK_OrdenaVetor_int(NumPontosCorte, pontosdecorte);
	pontosdecorte[NumPontosCorte] = N-1;

	inicio = 0;
	for (i=0; i<NumPontosCorte+1; i++)
	{
		fim = pontosdecorte[i] + 1;
		for (indice=inicio; indice<fim; indice++)
		{
			CopiaVetor(indice, N, Pai1->OptNet, Filho1->OptNet);
			CopiaVetor(indice, N, Pai2->OptNet, Filho2->OptNet);
		}
		GA_TrocaIndividuos(&Pai1, &Pai2);
		inicio = fim;
	}

	OPT_AtualizaGrauLogico(N, Filho1->OptNet);
	OPT_AtualizaGrauLogico(N, Filho2->OptNet);
	CorrigeGrauLogico(N, Anel, GrauLimite, Filho1->OptNet);
	CorrigeGrauLogico(N, Anel, GrauLimite, Filho2->OptNet);

}

// Crossover dos Pais Selecionados
void GA_Crossover(int NPontosCorte, TPopulacao *P)
{
	int k;
	// sorteia a ordem em que os pais escolhido serao combinados
	GA_PermutaVetorAleatorio_TIndividuo(P->TamPais, P->Pais);
	// realiza o crossover 2 a 2 do vetor Pais para vetor Filhos

 	for (k=0; k<P->TamPais; k+=2)
	{
		GA_CrossoverIndividuo
		(
			NPontosCorte, P->TamRede, P->AnelBasico, P->GrauLimite,
			P->Pais[k],   P->Pais[k+1],
			P->Filhos[k], P->Filhos[k+1]
		);
	}
}

// Gera Mutacao em um individuo, adicionando (ou removendo) enlaces
// NumMutacoes eh o numero de mutacoes - enlaces a serem alterados
void GA_MutacaoIndividuo(int NumMutacoes,
	int GrauLimite, int TamRede, int *Anel, TIndividuo *p)
{
	int k, linha, numlinhas;
	int ordemlinhas[MAX_TAM_REDE];
	
	do
	{
		if (NumMutacoes>=TamRede)
		{
			numlinhas = TamRede;
			NumMutacoes -= TamRede;		
		}
		else
		{
			numlinhas = NumMutacoes;
			NumMutacoes = 0;
		}
		// Remove (numlinhas) Enlaces
		TK_ArranjoVetorAleatorio_int(numlinhas, TamRede, ordemlinhas);
		for (k=0; k<numlinhas; k++)
		{
			linha = ordemlinhas[k];
			OPT_RemoveEnlaceLinha(linha, TamRede, Anel, p->OptNet);
		}
	} while (NumMutacoes>0);
	
	// Adiciona Enlaces ate completar matriz de topologia virtual
	OPT_CompletaMatrizTopologiaVirtual(GrauLimite, TamRede, p->OptNet);
}

// Mutacao dos Filhos Gerados
void GA_Mutacao(int NumMutacoes, TPopulacao *P)
{
	int k;
	for (k=0; k<(P->TamFilhos); k++)
		GA_MutacaoIndividuo(NumMutacoes,
			P->GrauLimite, P->TamRede, P->AnelBasico, P->Filhos[k]);
}


// Calcula o Fitness e a Pre-Avaliacao do vetor de Individuos Ind
void GA_AvaliaInd(TPopulacao *P)
{
	GA_FitnessVetorIndividuos(P->lp, P->PosVariavel, P->TamRede, P->TamInd, P->Ind);
}

// Calcula o Fitness e a Pre-Avaliacao do vetor de Individuos Ind
void GA_AvaliaFilhos(TPopulacao *P)
{
	GA_FitnessVetorIndividuos(P->lp, P->PosVariavel, P->TamRede, P->TamFilhos, P->Filhos);
}

// Calcula Avaliacao dos individuos de Pop para que os pais
// possam ser selecionados por este valor na funcao selecao.
// Avalicao atribui o MAIOR valor ao MELHOR individuo.
// Antes de avaliar, realiza uma pre-avaliacao no vetor Pop.
// Retorna o desvio-padrao (Sigma) do Fitness de Pop
// PRE: campo Fitness dos individuos (Pop) deve estar calculado
float GA_AvaliaPop(TPopulacao *P)
{
	float sigmafitness;

	GA_PreAvaliacao(P->TamPop, P->Pop, P);

	sigmafitness=
		//Avaliacao_Normalizacao(100, 100/P->TamPop, P);
		GA_Avaliacao_Windowing(0.99, P);
		//Avaliacao_Sigma(P);
	return sigmafitness;
}

//---------------------------------------------------------------------

// Seleciona em Ind (Pop+Filhos), quais individuos
// farao parte da nova Pop (sobreviventes).
// Os valores min, max e soma de fitness sao setados
void GA_SelecionaSobreviventes(TPopulacao *P)
{
	// Ordena Vetor de Individuos da Populacao pelo Fitness
	// para selecionar os melhores individuos para Pop
	GA_OrdenaVetorIndividuos(P->TamInd, P->Ind);
}

// Seleciona os Pais para realizarem Crossover
void GA_SelecionaPais(TPopulacao *P)
{
	//Selecao_RoletaViciada(P);
	//Selecao_Torneio(2, P);
	GA_Selecao_AmostragemEstocasticaUniforme(P);
}

//---------------------------------------------------------------------

// Calcula a pre-avaliacao de todos os individuos em funcao do Fitness e da
// direcao de otimizacao (MIN ou MAX) para que o maior valor corresponda ao
// melhor individuo. Em Seguida, varre vetor de individuos (Ind) em busca 
// dos valores Minimo, Maximo e Somatorio de Fitness e Avaliacao de (Ind).
void GA_PreAvaliacao(int TamInd, TIndividuo **Ind, TPopulacao *P)
{
	int k, direcao_min;
	float fitness, avaliacao;

	P->MinFitness  = P->MinAvaliacao  = INF_POS;
	P->MaxFitness  = P->MaxAvaliacao  = INF_NEG;
	P->SomaFitness = P->SomaAvaliacao = 0.0;

	// Min, Max e Soma dos valores de Fitness
	for (k=0; k<TamInd; k++)
	{
		fitness = Ind[k]->Fitness;
		if (fitness < P->MinFitness) P->MinFitness = fitness;
		if (fitness > P->MaxFitness) P->MaxFitness = fitness;
		P->SomaFitness += fitness;
	}
	// Pre-Avaliacao
	direcao_min = (lpx_get_obj_dir(P->lp) == LPX_MIN);
	for (k=0; k<TamInd; k++)
	{
		fitness = Ind[k]->Fitness;
		// Calcula avaliacao em funcao do Fitness e direcao.
		// Se estiver minimizando, a avaliacao eh calculada como
		// complemento do Fitness, caso contrario, eh o proprio Fitness.
		// Isto garante que o MELHOR individuo tenha a MAIOR avaliacao
		// independente da direcao.
		Ind[k]->Avaliacao = 
			( direcao_min * (P->MinFitness+(P->MaxFitness-fitness))) +
			(!direcao_min * (fitness)           );
		avaliacao = Ind[k]->Avaliacao;
		// Min, Max e Soma dos valores de Avaliacao
		if (avaliacao < P->MinAvaliacao) P->MinAvaliacao = avaliacao;
		if (avaliacao > P->MaxAvaliacao) P->MaxAvaliacao = avaliacao;
		P->SomaAvaliacao += avaliacao;
	}
}

//---------------------------------------------------------------------

// Retorna o Desvio Padrao (sigma) da Avaliacao da populacao (Pop)
float GA_CalculaSigmaAvaliacao(TPopulacao *P)
{
	int k;
	float fator, MediaAvaliacao, SigmaAvaliacao = 0.0;
	MediaAvaliacao = (P->SomaAvaliacao / P->TamPop);
	for (k=0; k<P->TamPop; k++)
	{
		fator = (P->Pop[k]->Avaliacao - MediaAvaliacao);
		SigmaAvaliacao += (fator * fator);
	}
	 // variancia
	SigmaAvaliacao = (SigmaAvaliacao / P->TamPop);
	// desvio padrao
	SigmaAvaliacao = sqrt (SigmaAvaliacao); 
	return SigmaAvaliacao;
}

// Avaliacao dos individuos usando a tecnica de Normalizacao
float GA_Avaliacao_Normalizacao(float AvaliacaoMax, float Decremento, TPopulacao *P)
{
	int k;
	GA_OrdenaVetorIndividuos(P->TamPop, P->Pop);
	P->SomaAvaliacao = 0.0;
	for (k=0; k<P->TamPop; k++)
	{
		P->Pop[k]->Avaliacao = AvaliacaoMax - k*Decremento;
		P->SomaAvaliacao += P->Pop[k]->Avaliacao;
	}
	// Apos ordenacao, o maior elemento eh o primeiro
	P->MaxAvaliacao = P->Pop[0]->Avaliacao;
	P->MinAvaliacao = P->Pop[P->TamPop-1]->Avaliacao;
	return GA_CalculaSigmaAvaliacao(P);
}

// Avaliacao dos individuos usando a tecnica de Windowing
// PercentualMinAvaliacao (<100%) eh usado para o 
// calculo do valor base a ser subtraido - tipico ~0.99 ou ~99%
float GA_Avaliacao_Windowing(float PercentualMinAvaliacao, TPopulacao *P)
{
	int k;
	float valorbase;
	valorbase = P->MinAvaliacao * PercentualMinAvaliacao;
	P->SomaAvaliacao = 0.0;
	for (k=0; k<P->TamPop; k++)
	{
		P->Pop[k]->Avaliacao = P->Pop[k]->Avaliacao - valorbase;
		P->SomaAvaliacao    += P->Pop[k]->Avaliacao;
	}
	P->MinAvaliacao  = P->MinAvaliacao - valorbase;
	P->MaxAvaliacao  = P->MaxAvaliacao - valorbase;
	return GA_CalculaSigmaAvaliacao(P);
}

// Avaliacao dos individuos usando a tecnica de Escalonamento Sigma
float GA_Avaliacao_Sigma(TPopulacao *P)
{
	float MediaAvaliacao, SigmaAvaliacao, Avaliacao;
	int k;
	MediaAvaliacao = P->SomaAvaliacao / P->TamPop;
	// Calcula Desvio Padrao (sigma) da Avaliacao de Pop
	SigmaAvaliacao = GA_CalculaSigmaAvaliacao(P);
	// Avaliacao dos individuos de acordo com o valor Sigma
	if (SigmaAvaliacao == 0.0)
	{
		for (k=0; k<P->TamPop; k++)
		{
			P->Pop[k]->Avaliacao = 1.0;
		}
		P->MinAvaliacao  = 1.0;
		P->MaxAvaliacao  = 1.0;
		P->SomaAvaliacao = 1.0 * P->TamPop;
	}
	else
	{
		P->MinAvaliacao  = INF_POS;
		P->MaxAvaliacao  = INF_NEG;
		P->SomaAvaliacao = 0.0;
		for (k=0; k<P->TamPop; k++)
		{
			// calculo da avaliacao do individuo
			Avaliacao = 1.0 + 
				(P->Pop[k]->Avaliacao - MediaAvaliacao) / (2 * SigmaAvaliacao);
			// se avaliacao for nao-positiva, atribui valor minimo
			if (Avaliacao <= 0) { Avaliacao = 0.1; }
			// atribuicao de valores
			P->Pop[k]->Avaliacao = Avaliacao;
			if (Avaliacao < P->MinAvaliacao) P->MinAvaliacao = Avaliacao;
			if (Avaliacao > P->MaxAvaliacao) P->MaxAvaliacao = Avaliacao;
			P->SomaAvaliacao += Avaliacao;
		}
	}
	return GA_CalculaSigmaAvaliacao(P);
}

//---------------------------------------------------------------------

// Seleciona pelo metodo da roleta viciada
void GA_Selecao_RoletaViciada(TPopulacao *P)
{
	int k, t;
	float roleta, soma_acumulada;
	for (k=0; k<P->TamPais; k++)
	{
		t = 0;
		soma_acumulada = (P->Pop[t]->Avaliacao / P->SomaAvaliacao);
		roleta = ((float) rand()/RAND_MAX);
		while (soma_acumulada < roleta)
		{
			t++;
			soma_acumulada += (P->Pop[t]->Avaliacao / P->SomaAvaliacao);
		}
		P->Pais[k] = P->Pop[t];
	}
}

// Seleciona pelo metodo do Torneio
void GA_Selecao_Torneio(int TamTorneio, TPopulacao *P)
{
	int k, t, ind_pop[MAX_TAM_POP];
	TIndividuo *vencedor, *individuo;
	for (k=0; k<P->TamPais; k++)
	{
		TK_ArranjoVetorAleatorio_int(TamTorneio, P->TamPais, ind_pop);
		// Realiza Torneio nos (TamTorneio) primeiros elementos
		// de ind_pop: descobre o melhor elemento
		vencedor = P->Pop[ind_pop[0]];
		for (t=1; t<TamTorneio; t++)
		{
			individuo = P->Pop[ind_pop[t]];
			if (individuo->Avaliacao > vencedor->Avaliacao)
			{
				vencedor = individuo;
			}
		}
		P->Pais[k] = vencedor;
	}
}

// Seleciona pelo metodo da Amostragem Estocastica Uniforme
void GA_Selecao_AmostragemEstocasticaUniforme(TPopulacao *P)
{
	int k, t;
	float posicao, soma_acumulada;
	TIndividuo *individuo;
	t = 0;
	soma_acumulada = (P->Pop[0]->Avaliacao / P->SomaAvaliacao);
	posicao = ((float) rand()/RAND_MAX) / P->TamPais;
	for (k=0; k<P->TamPais; k++)
	{
		while (soma_acumulada <= posicao)
		{
			t++;
			individuo = P->Pop[t];
			soma_acumulada += (individuo->Avaliacao / P->SomaAvaliacao);
		}
		P->Pais[k] = P->Pop[t];
		posicao += 1.0 / P->TamPais;
	}
}

//---------------------------------------------------------------------

// Imprime os pais selecionados para Crossover
void GA_ImprimePaisSelecionados(TPopulacao *P)
{
	int k, t;
	for (k=0; k<P->TamPais; k++)
	{
		t=0;
		while(P->Pais[k] != P->Pop[t])
			t++;
		printf("Pai[%d]=Pop[%d]\tFitness\t%10.3f\t"
			"Avaliacao\t%10.3f\t(%p)\n",
			k, t, P->Pais[k]->Fitness, P->Pais[k]->Avaliacao, P->Pais[k]);
	}
}

// Imprime os individuos de um vetor Ind
void GA_ImprimeIndividuos(int TamRede, int TamInd, TIndividuo **Ind)
{
	int k;
	float diversidade;
	for (k=0; k<TamInd; k++)
	{
		diversidade = 100.0 * OPT_Diversidade(TamRede, Ind[0]->OptNet, Ind[k]->OptNet);
		printf("Ind[%d]\tFit\t%10.3f\t"
			"Aval\t%10.3f\tDiv\t%5.2f\t(%p)\n",
			k, Ind[k]->Fitness, Ind[k]->Avaliacao, diversidade, Ind[k]);
	}
}

//---------------------------------------------------------------------

