#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "toolkit.h"
#include "optical_network.h"
#include "ga.h"

#define MAX_ITER 1000

char Modelo[256], Dados[256], Saida[256]; // foi nescessario criar essas variaveis globais para receber os parametros argv

float CalculaLBD(int GrauLogico, int TamRede, float **Matriz)
{
	int i, j, soma_lin_i, soma_col_i;
	float lbd = -1E37;
	for (i=0; i<TamRede; i++)
	{
		soma_lin_i = 0;
		soma_col_i = 0;
		for(j=0; j<TamRede; j++)
		{
			soma_lin_i += (int) Matriz[i][j];
			soma_col_i += (int) Matriz[j][i];
		}
		if (lbd < soma_lin_i) lbd = soma_lin_i;
		if (lbd < soma_col_i) lbd = soma_col_i;
	}
	lbd = lbd/GrauLogico;
	return lbd;
}

void RecebeParametros(int argc, char **argv,
	int *GrauLimite, int *TamPop, int *Custo,
	float *PercentualMutacao, int *Crossover,
	int *MinGeracoes, int *GeracoesRepetidas)
{
	if (argc == 9)
	{
		*GrauLimite         = atoi(argv[1]);
		*TamPop             = atoi(argv[2]);
		*Custo		        = atoi(argv[3]);
		*PercentualMutacao	= atoi(argv[4]);
		*Crossover	        = atoi(argv[5]);
		strcpy(Dados, argv[6]);
		*MinGeracoes	    = atoi(argv[7]);
		*GeracoesRepetidas  = atoi(argv[8]);
	}
	else
	{
		printf("Uso:\n"
			"%s <GrauLimite> <TamPop> <Custo[1 ou 2]> "
			"<PercentualMutacao> <Crossover> <Dados> "
			"<MinGeracoes> <GeracoesRepetidas>\n", argv[0]);
		exit(-1);
	};
}

int main(int argc, char **argv)
{
	int i, *AnelBasico,
		TamRede, GrauLimite, TamPop, Custo, NumMutacao,
		Crossover, MinGeracoes, GeracoesRepetidas,
		criterio_parada, vezes_repetidas;
	float t, PercentualMutacao, **Matriz, sigmaavaliacao,
		lower_bound, FitnessAnterior;
	float (*FuncaoCusto)();

	struct timeval t1, t2;
	TPopulacao *P;
	gettimeofday(&t1, NULL);
	// Le parametros da linha de comando
	RecebeParametros(argc, argv, &GrauLimite,
		&TamPop, &Custo, &PercentualMutacao, &Crossover,
		&MinGeracoes, &GeracoesRepetidas);

	// le o tamanho da rede e matriz de trafego a partir do arquivo de dados
	TK_LeDadosLP(Dados, &TamRede, &Matriz);
	// Calcula Lower Bound Deterministico
	lower_bound = CalculaLBD(GrauLimite, TamRede, Matriz);
	// Gera Semente aleatoria baseada na hora atual
	srand(t1.tv_usec+t1.tv_sec);
	// Aloca e Seleciona Anel Basico
	AnelBasico = (int*) TK_AlocaVetor(TamRede, sizeof(int));
	switch (Custo)
	{
		case 1:
			{
				FuncaoCusto = OPT_Congestionamento;
				strcpy(Modelo, "haggbif.mod4");
				break;
			}
		case 2:
			{
				FuncaoCusto = OPT_FTnet;
				strcpy(Modelo, "ftaggbif.mod4");
				break;
			}
		default:
			{
				printf("Numero Invalido de Funcao Custo\n");
				exit(-1);
			}
	};
	OPT_SelecionaAnelBasico(FuncaoCusto, 50000, TamRede, Matriz, AnelBasico);
	
	// Numero de Enlaces que sofrerao mutacao
	NumMutacao = (PercentualMutacao*TamRede*(TamRede-2)/100);

	printf("\n\n---------------------------------------------\n");
	printf("Mingeracoes:\t%d\n", MinGeracoes);
	printf("GrauLogico:\t%d\n", GrauLimite);
	printf("PercentualMutacao:\t%f\n", PercentualMutacao);
	printf("NumMutacao:\t%d\n", NumMutacao);
	printf("Crossover:\t%d\n", Crossover);
	printf("------------------------------------------------\n\n");

	strcpy(Saida, "saida.dat");
	// POPULACAO
	P = GA_CriaPopulacao(GrauLimite, TamRede, AnelBasico,
		TamPop, Modelo, Dados, Saida, "B[1,1]");

	// Avalia vetor Ind (Pop+Filhos) para selecao dos sobreviventes
	GA_AvaliaInd(P);
	// inicializa variaveis do loop (Algoritmo Genetico)
	i=0;
	vezes_repetidas = 0;
	FitnessAnterior = 1E37;
	do
	{
		//Selecionando Sobreviventes da populacao
		GA_SelecionaSobreviventes(P);
		// Avalia populacao para selecao dos pais
		sigmaavaliacao = GA_AvaliaPop(P);
		// Selecao dos Pais
		GA_SelecionaPais(P);
		// Crossover dos Pais para gerar Filhos
		GA_Crossover(Crossover, P);
		// Mutacao dos Filhos
		GA_Mutacao(NumMutacao, P);
		// Avaliacao
		GA_AvaliaFilhos(P);
		// Imprime o melhor individuo
		printf("----------------------------------------------------------------"
			"\tGERACAO %d\n",i);
		GA_ImprimeIndividuos(TamRede, P->TamInd, P->Ind);

		i++; // numero de iteracoes
		if ((FitnessAnterior - P->MinFitness) < 0.0005)
		{
			vezes_repetidas++;
		}
		else
		{
			vezes_repetidas = 0;
			FitnessAnterior =  P->MinFitness;
		};
		criterio_parada =
			(
				(vezes_repetidas > GeracoesRepetidas) && (i > MinGeracoes)
			);
	} while (!criterio_parada); //NaoAtingiuCriterioDeParada
	GA_SelecionaSobreviventes(P);
	//Imprime Solucao;
	gettimeofday(&t2, NULL);
	t =	(float) (t2.tv_sec-t1.tv_sec) + 1E-6 * (float) (t2.tv_usec-t1.tv_usec);
	printf("\nMelhor Individuo\n");
	OPT_ImprimeTOptNet(TamRede, P->Pop[0]->OptNet);	
	printf("\n\n---------------------------------------------\n");
	printf("Ngeracoes:\t%d\n",i);
	printf("GrauLogico:\t%d\n",GrauLimite);
	printf("PercentualMutacao:\t%f\n", PercentualMutacao);
	printf("Crossover:\t%d\n",Crossover);
	printf("Tempo:\t%f\n", t);
	printf("Melhor solucao:\t%f\n",P->Ind[0]->Fitness);
	printf("LowerBound:\t%f\n",lower_bound);
	printf("------------------------------------------------\n\n");
	GA_DestroiPopulacao(P);
	TK_LiberaMatriz((void**) Matriz);
	return 0;
}
