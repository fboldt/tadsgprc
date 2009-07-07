#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
//#include "glpk.h"
#include "glpkwrapper.h"
#include "toolkit.h"
#include "optical_network.h"

//-----------------------------------------------------------------------------

void RecebeParametros(int argc, char **argv,
	int *GrauLimite, int *TamInd)
{
	if (argc > 2)
	{
		*GrauLimite = atoi(argv[1]);
		*TamInd     = atoi(argv[2]);
	}
	else
	{
		printf("Uso: %s <GrauLimite> <TamInd>\n", argv[0]);
		exit(-1);
	};
}

int main(int argc, char **argv)
{
	int i, j, *AnelBasico,
		TamRede, GrauLimite, TamInd;
	float **Matriz;
	struct timeval t1;
	TOptNet *OptNet;
	LPX *lp;
	//LPGW lp;
	// Le parametros da linha de comando
	RecebeParametros(argc, argv, &GrauLimite, &TamInd);
	TamInd = 1;

	// Imprime os tamanhos dos principais tipos
	printf("sizeof(void)=%d sizeof(int)=%d sizeof(long)=%d sizeof(void*)=%d\n",
		sizeof(void), sizeof(int), sizeof(long), sizeof(void*));

	// le o tamanho da rede e matriz de trafego
	// a partir do arquivo de dados
	TK_LeDadosLP("testes/p2_ivtd.dat", &TamRede, &Matriz);

	// Gera Semente aleatoria baseada na hora atual
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec+t1.tv_sec);

	printf("Alocando Anel Básico (%d)...\n", TamRede);
	AnelBasico = (int*) TK_AlocaVetor(TamRede, sizeof(int));
	
	printf("Seleciona Anel Básico...\n");
	OPT_SelecionaAnelBasico(OPT_Congestionamento, 20000, TamRede, Matriz, AnelBasico);
	//SelecionaAnelBasico(FTnet, 5, TamRede, Matriz, AnelBasico);

	TK_ImprimeVetor_int(TamRede, AnelBasico);
	printf("Congestionamento: %f\n", OPT_Congestionamento(TamRede, AnelBasico, Matriz));
	//printf("FTnet:            %f\n", FTnet(TamRede, AnelBasico, Matriz));


	OptNet  = OPT_CriaOptNet(TamRede);
	OPT_InicializaOptNet(TamRede, OptNet);

	OPT_ImprimeTOptNet(TamRede,OptNet);

	//MapeiaAnelMatrizTopologiaVirtual
	OPT_MapeiaAnelMatrizTopologiaVirtual(TamRede, AnelBasico,OptNet);



	// Aloca vetor de individuos	
	printf("\nAlocando vetor INDIVIDUOs (%d)...\n", TamInd);
	//Individuo = AlocaIndividuos(TamRede, TamInd);
	// Inicializa vetor de individuos contiguos com valores de teste
	printf("\nInicializando OptNet...\n\n");
		for (i=0; i<TamRede; i++)
		{
			for (j=0; j<TamRede; j++)
			{
				OptNet->TopologiaVirtual[i][j] = 1;
			}
		}
		OPT_AtualizaGrauLogico(TamRede, OptNet);
	printf("\nImprimindo OptNet...\n\n");
	OPT_ImprimeTOptNet(TamRede, OptNet);
	// Le modelo LP a partir do arquivo
	printf("\nLendo Modelo...\n");
	lp = TK_LeModeloLP("testes/milp_hmax_agg.mod", "testes/p2_ivtd.dat", "foo.txt");
	TK_SetaParametrosLP(lp);
	OPT_CorrigeGrauLogicoSaida(TamRede,AnelBasico,GrauLimite,OptNet);
	OPT_CorrigeGrauLogicoEntrada(TamRede,AnelBasico,GrauLimite,OptNet);
	OPT_AtualizaGrauLogico(TamRede, OptNet);
	printf("\nImprimindo OptNet...\n\n");
	OPT_ImprimeTOptNet(TamRede, OptNet);

	OPT_DestroiOptNet(OptNet);

	// Libera Memoria alocada pelo modelo
	printf("\nFinalizando Modelo LP...\n");
	TK_FinalizaLP(lp);

	// Libera Memoria alocada por individuos
	//printf("\nLiberando Vetor INDIVIDUOs...\n");
	//LiberaVetorIndividuos(Individuos);

	// Libera Memoria alocada pela Matriz de trafego
	TK_LiberaMatriz((void**) Matriz);
/*
//*/

	return 0;
}

