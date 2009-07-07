#include <stdlib.h>
#include <stdio.h>
#include "toolkit.h"
#include "optical_network.h"

//-----------------------------------------------------------------------------

// Calcula o FTj (trafico restransmitido eletronicamente) de cada no em um anel
float OPT_FT_J(int Indice, int TamRede, int *Anel, float **Matriz)
{
	int i, j, entrada, saida, N=TamRede;
	float demanda = 0.0;
	// i: varia de (indice+2) a (indice-1) salta 2 nos {Indice,Indice+1}
	for (i=(Indice+2)%N; i!=Indice; i=(i+1)%N)
	{
		saida = Anel[i];
		// j: varia de (indice+1) a (i-1)
		for (j=(Indice+1)%N; j!=i; j=(j+1)%N)
		{
			entrada = Anel[j];
			demanda += Matriz[saida][entrada];
		}
	}
	return demanda;
}

// Calcula o FTnet somando todos os FTj's
float OPT_FTnet(int TamRede, int *Anel, float **Matriz)
{
	int i;
	float Ftnet=0.0;
	for (i=0; i<TamRede; i++)
		Ftnet += OPT_FT_J(i, TamRede, Anel, Matriz);
	return Ftnet;		
}

// Calcula o carregamento de trafego no enlace I->J de um Anel
float OPT_Demanda_IJ(int I, int J, int TamRede, int *Anel, float **Matriz)
{
	int origem, destino, saida, entrada, N=TamRede;
	float demanda_ij=0.0;
	// origem: varre o anel de (J+1) ate (J-1) - salta o J
	for (origem=(J+1)%N; origem!=J; origem=(origem+1)%N)
	{
		saida = Anel[origem];
		// destino: varre o anel de J ate (origem-1)
		for (destino=J ; destino!=origem; destino=(destino+1)%N)
		{
			entrada = Anel[destino];
			demanda_ij += Matriz[saida][entrada];
		}
	}
	return demanda_ij;
}

// Retorna o Congestionamento: quantidade de trafego designada ao enlace logico mais carregado
float OPT_Congestionamento(int TamRede, int *Anel, float **Matriz)
{
	int i, j;
	float d, Hmax=0.0;
	// i varre todos os nos do anel
	for (i=0; i<TamRede; i++)
	{
		j = (i+1)%TamRede; // j eh o no subsequente ao no i (enlace i->j)
		d = OPT_Demanda_IJ(i, j, TamRede, Anel, Matriz);
		if (d > Hmax)
			Hmax = d;
	}
	return Hmax;
}

//-----------------------------------------------------------------------------

// Funcao que seleciona o melhor anel basico dentre
// diversos aneis aleatorios gerados
// FuncaoCusto pode ser Congestionamento ou FTnet
void OPT_SelecionaAnelBasico(float (*FuncaoCusto)(),
	int Iteracoes, int N, float **MatrizTrafego, int *AnelBasico)
{
	int i, r, s, tmp, *novoanel;
	float novocusto, menorcusto;

	novoanel = (int*) TK_AlocaVetor(N, sizeof(int));

	TK_PermutacaoVetorAleatorio_int(N-1, AnelBasico);
	// Fixa o ultimo elemento na ultima posicao
	// para evitar arranjos repetidos de aneis
	AnelBasico[N-1] = (N-1);
	menorcusto = FuncaoCusto(N, AnelBasico, MatrizTrafego);
	TK_CopiaVetor_int(N, AnelBasico, novoanel);
	for (i=0; i<Iteracoes; i++)
	{
		if (i%2 == 0)
			TK_CopiaVetor_int(N, AnelBasico, novoanel);
		// sorteia indice dos elementos a serem trocados
		r = rand()%(N-1);
		s = rand()%(N-1);
		// troca elementos
		tmp         = novoanel[r];
		novoanel[r] = novoanel[s];
		novoanel[s] = tmp;
		// calcula custo do novoanel
		novocusto = FuncaoCusto(N, novoanel, MatrizTrafego);
		if (novocusto < menorcusto)
		{
			menorcusto = novocusto;
			TK_CopiaVetor_int(N, novoanel, AnelBasico);
		};
	}
	TK_LiberaVetor(novoanel);
}

// Adiciona Enlace a Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeAdicionarEnlace for TRUE
// Esta funcao atualiza GrauLogicoSaida e GrauLogicoEntrada
void OPT_AdicionaEnlace(int Lin, int Col, TOptNet *OptNet)
{

	char enlace;
	enlace = OptNet->TopologiaVirtual[Lin][Col];
	OptNet->TopologiaVirtual[Lin][Col] =  1;
	// Incrementa apenas se nao havia enlace antes (XOR)
	OptNet->GrauLogicoSaida[Lin]   += (int) (enlace ^ 1);
	OptNet->GrauLogicoEntrada[Col] += (int) (enlace ^ 1);
}

// Adiciona Enlace em uma Linha da Matriz de Topologia Virtual
// varrendo a linha em busca de uma coluna valida para adicionar enlace.
// Retorna TRUE se algum enlace foi adicionado, e FALSE caso contrario
int OPT_AdicionaEnlaceLinha(int Linha, int GrauLimite, int TamRede, TOptNet *OptNet)
{
	int k, coluna, sair = FALSE;
	int ordemcolunas[MAX_TAM_REDE];
	TK_PermutacaoVetorAleatorio_int(TamRede, ordemcolunas);
	// percorre ordemcolunas ate encontrar coluna valida p/ ADICAO de enlace
	for (k=0; k<TamRede && sair==FALSE; k++)
	{
		coluna = ordemcolunas[k];
		if (OPT_PodeAdicionarEnlace(Linha, coluna, GrauLimite, OptNet))
		{
			OPT_AdicionaEnlace(Linha, coluna, OptNet);
			sair = TRUE;
		};
	}
	return sair;
}

// Funcao que checa se um enlace pode ser removido em uma posicao
// Essa funcao eh usada na funcao CorrigeGrauLogico
int OPT_PodeRemoverEnlace(int Lin, int Col, int TamRede, int *Anel, TOptNet *OptNet)
{
	int i=0, saida, entrada;
	// procura, no anel, indice da linha(saida)
	while (Lin!=Anel[i] && i<TamRede)
	{
		i++;
	}
	saida   = i;
	entrada = (i+1)%TamRede;
	return
	(
		(Col!=Anel[entrada]) && // se nao pertence ao anel basico
		(OptNet->TopologiaVirtual[Lin][Col] == 1) // e se nao-nulo
	);	// Retorna 1 caso seja possivel remover enlace
}

// Remove Enlace da Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeRemoverEnlace for TRUE
void OPT_RemoveEnlace(int Lin, int Col, TOptNet *OptNet)
{
	char enlace;
	enlace = OptNet->TopologiaVirtual[Lin][Col];
	OptNet->TopologiaVirtual[Lin][Col]=0;
	// Decrementa apenas se havia enlace antes
	OptNet->GrauLogicoSaida[Lin]   -= (int) (enlace);
	OptNet->GrauLogicoEntrada[Col] -= (int) (enlace);
}

// Remove um Enlace de uma Linha da Matriz de Topologia Virtual
// varrendo a Linha em busca de coluna valida para remover enlace
void OPT_RemoveEnlaceLinha(int Linha, int TamRede, int *Anel, TOptNet *OptNet)
{
	int k, coluna, sair = FALSE;
	int ordemcolunas[MAX_TAM_REDE];
	TK_PermutacaoVetorAleatorio_int(TamRede, ordemcolunas);
	// percorre ordemcolunas ate encontrar coluna valida p/ REMOCAO de enlace
	for (k=0; k<TamRede && sair==FALSE; k++)
	{
		coluna = ordemcolunas[k];
		if (OPT_PodeRemoverEnlace(Linha, coluna, TamRede, Anel, OptNet))
		{
			OPT_RemoveEnlace(Linha, coluna, OptNet);
			sair = TRUE;
		};
	}
}

// Remove um Enlace de uma Coluna da Matriz de Topologia Virtual
// varrendo a Coluna em busca de linha valida para remover enlace
void OPT_RemoveEnlaceColuna(int Coluna, int TamRede, int *Anel, TOptNet *OptNet)
{
	int k, linha, sair = FALSE;
	int ordemlinhas[MAX_TAM_REDE];
	TK_PermutacaoVetorAleatorio_int(TamRede, ordemlinhas);
	// percorre vetorlinhas ate encontrar linha valida p/ REMOCAO de enlace
	for (k=0; k<TamRede && sair==FALSE; k++)
	{
		linha = ordemlinhas[k];
		if (OPT_PodeRemoverEnlace(linha, Coluna, TamRede, Anel, OptNet))
		{
			OPT_RemoveEnlace(linha, Coluna, OptNet);
			sair = TRUE;
		};
	}
}

// Funcao que atualiza os vetores GrauLogicoSaida e GrauLogicoEntrada
// Grau Logico de saida eh definido pelas linhas
// Grau Logico de entrada eh definido pelas colunas
void OPT_AtualizaGrauLogico(int TamRede, TOptNet *OptNet)
{
	int i, j, cont_saida, cont_entrada;
	for (i=0; i<TamRede; i++)
	{
		cont_saida   = 0;
		cont_entrada = 0;
		for(j=0; j<TamRede; j++)
		{
			cont_saida   += (int) OptNet->TopologiaVirtual[i][j];
			cont_entrada += (int) OptNet->TopologiaVirtual[j][i];
		}	  
		OptNet->GrauLogicoSaida[i]   = cont_saida;
		OptNet->GrauLogicoEntrada[i] = cont_entrada;
	}
}

//-----------------------------------------------------------------------------

//Corrige o grau logico das linhas (saida) removendo enlaces de um individuo
void OPT_CorrigeGrauLogicoSaida(int TamRede, int *Anel, int GrauLimite, TOptNet *OptNet)
{
	int linha;
	for(linha=0; linha<TamRede; linha++)
	{			
		while(OptNet->GrauLogicoSaida[linha] > GrauLimite)

			OPT_RemoveEnlaceLinha(linha, TamRede, Anel, OptNet);
	}
}

//Corrige o grau logico das colunas (entrada) removendo enlaces de um individuo
void OPT_CorrigeGrauLogicoEntrada(int TamRede, int *Anel, int GrauLimite, TOptNet *OptNet)
{
	int coluna;
	for(coluna=0; coluna<TamRede; coluna++)
	{			
		while((OptNet->GrauLogicoEntrada[coluna]) > GrauLimite)

			OPT_RemoveEnlaceColuna(coluna, TamRede, Anel, OptNet);
	}
}

// Completa a Matriz de Topologia Virtual
// adicionando enlaces ate atingir Grau Limite
void OPT_CompletaMatrizTopologiaVirtual(int GrauLimite, int TamRede, TOptNet *OptNet)
{
	int k, linha, adicionou;
	int ordemlinhas[MAX_TAM_REDE];
	do
	{
		adicionou = 0; // numero de enlaces adicionados
		TK_PermutacaoVetorAleatorio_int(TamRede, ordemlinhas);
		for(k=0; k<TamRede; k++)
		{
			linha  = ordemlinhas[k];
			adicionou += OPT_AdicionaEnlaceLinha(linha, GrauLimite, TamRede, OptNet);
		}
	} while (adicionou > 0);
}

// Checa se um enlace pode ser adicionado em uma posicao
// Esta funcao eh usada na funcao CompletaMatrizTopologiaVirtual
int OPT_PodeAdicionarEnlace(int Lin, int Col, int Grau, TOptNet *OptNet)
{
	return
	(
		(Lin != Col) &&
		(OptNet->TopologiaVirtual[Lin][Col] == 0) &&
		(OptNet->GrauLogicoSaida[Lin] < Grau) &&
		(OptNet->GrauLogicoEntrada[Col] < Grau)
	);	// Retorna 1 caso seja possivel adicionar enlace
}

// Checa se um enlace pode ser adicionado em uma posicao
// Admite-se multiplos enlaces entre um mesmo par de nos
// (Multiplicidade)
int OPT_PodeAdicionarEnlaceMultiplo(int Lin, int Col, int Grau, TOptNet *OptNet)
{
	return
	(
		(Lin != Col) &&
		(OptNet->GrauLogicoSaida[Lin] < Grau) &&
		(OptNet->GrauLogicoEntrada[Col] < Grau)
	);	// Retorna 1 caso seja possivel adicionar enlace com multiplicidade
}

// Adiciona Enlace Multiplo a Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeAdicionarEnlaceMultiplo for TRUE
// Esta funcao atualiza GrauLogicoSaida e GrauLogicoEntrada
void OPT_AdicionaEnlaceMultiplo(int Lin, int Col, TOptNet *OptNet)
{
	OptNet->TopologiaVirtual[Lin][Col]++;
	OptNet->GrauLogicoSaida[Lin]++;
	OptNet->GrauLogicoEntrada[Col]++;
}

// Mapeia Anel Basico na Matriz Topologia Virtual do TOptNet
void OPT_MapeiaAnelMatrizTopologiaVirtual(int TamRede, int *Anel, TOptNet *OptNet)
{
	int k, saida, entrada, N=TamRede;
	// Mapeia anel na matriz de topologia virtual
	for(k=0; k<N; k++)
	{
		saida   = Anel[k];
		entrada = Anel[(k+1)%N];
		OPT_AdicionaEnlace(saida, entrada, OptNet);
	}
}

//-----------------------------------------------------------------------------

//Retorna o tamanho Contiguo da estrutura TOptNet
int OPT_TamanhoContiguoOptNet(int TamRede)
{
	int NLin, NCol, tam, tam_ponteiro, alinhamento;
	tam_ponteiro = sizeof(void*);
	NLin = NCol = TamRede;
	tam =
		(
			// Estrutura TOptNet
			sizeof(TOptNet) + 
			// Matriz Topologia Virtual
			TK_TamanhoContiguoMatriz(NLin, NCol, sizeof(char)) +
			// Vetor Grau Logico Saida
			(NLin * sizeof(int)) +
			// Vetor Grau Logico Entrada
			(NCol * sizeof(int))
		);
	// checa alinhamento:
	// (se tamanho da estrutura eh multiplo do tamanho de ponteiros)
	// se nao estiver alinhado, alinha para aumentar desempenho.
	alinhamento = tam % tam_ponteiro;
	if (alinhamento != 0)
		tam += (tam_ponteiro-alinhamento);
	return tam;
}

//Aloca uma estrutura TOptNet
TOptNet *OPT_AlocaOptNet(int TamRede)
{
	TOptNet *OptNet;
	OptNet = (TOptNet*) malloc
			( 
				OPT_TamanhoContiguoOptNet(TamRede) 
			);
	return OptNet;
}

//Faz o mapeamento da estrutura TOptNet
void OPT_MapeiaContiguoOptNet(int TamRede, TOptNet *OptNet)
{
	int NLin, NCol;
	void *posicao = (void*) OptNet;
	posicao += sizeof(TOptNet);
	NLin = NCol = TamRede;
	OptNet->TopologiaVirtual  = (char**) posicao;

	// Mapeia Matriz TopologiaVirtual
	TK_MapeiaContiguoMatriz(NLin, NCol,
		sizeof(char), (void**) OptNet->TopologiaVirtual);

	// Posicao de GrauLogicoSaida: apos TopologiaVirtual
	posicao += TK_TamanhoContiguoMatriz(NLin, NCol, sizeof(char));
	// Mapeia GrauLogicoSaida
	OptNet->GrauLogicoSaida   = (int*) posicao;
	// Posicao de GrauLogicoEntrada: apos GrauLogicoSaida
	posicao += NLin * sizeof(int);
	// Mapeia GrauLogicoEntrada
	OptNet->GrauLogicoEntrada = (int*) posicao;
}

//Inicializa uma estrutura OptNet com zeros
//utilizada apenas para testes
void OPT_InicializaOptNet(int N, TOptNet *OptNet)
{
	TK_InicializaMatriz_char(N, N, OptNet->TopologiaVirtual);
	TK_InicializaVetor_int(N, OptNet->GrauLogicoSaida);
	TK_InicializaVetor_int(N, OptNet->GrauLogicoEntrada);
}

//Aloca e mapeia uma estrutura OptNet
TOptNet *OPT_CriaOptNet(int TamRede)
{
	TOptNet *OptNet;
	OptNet = OPT_AlocaOptNet(TamRede);
	OPT_MapeiaContiguoOptNet(TamRede, OptNet);
	return OptNet;
}

//Libera a área de memória alocada por uma estrutura TOptNet
void OPT_DestroiOptNet(TOptNet *OptNet)
{
	free(OptNet);
}

//Imprime a matriz de topologia virtual e os graus logicos de saida e entrada
void OPT_ImprimeTOptNet(int TamRede, TOptNet *OptNet)
{
	int i, j;
	for (i=0; i<TamRede; i++)
	{
		for (j=0; j<TamRede; j++)
			printf("%4d", (int)OptNet->TopologiaVirtual[i][j]);
		printf("\t%d\n", OptNet->GrauLogicoSaida[i]);
		
	}
	for (j=0; j<TamRede; j++)
		printf("%4d", OptNet->GrauLogicoEntrada[j]);
	printf("\n");
	printf("\n");
}

// Copia Linha completa da Matriz de Topologia Virtual
void OPT_CopiaLinhaTopologia(int Lin, int TamRede, TOptNet *OptNet1, TOptNet *OptNet2)
{
	int j;
	for (j=0; j<TamRede; j++)
		OptNet2->TopologiaVirtual[Lin][j] = OptNet1->TopologiaVirtual[Lin][j];
}


// Copia Coluna completa da Matriz de Topologia Virtual
void OPT_CopiaColunaTopologia(int Col, int TamRede,
	TOptNet *OptNet1, TOptNet *OptNet2)
{
	int i;
	for (i=0; i<TamRede; i++)
		OptNet2->TopologiaVirtual[i][Col] = OptNet1->TopologiaVirtual[i][Col];
}

// Faz a comparação da quantidades de enlaces equivalentes em
// uma matriz e retorna esse valor em percentual
float OPT_Diversidade(int TamRede, TOptNet *OptNet1, TOptNet *OptNet2)
{
	int i, j;
	int SomaDiferenca = 0;
	for (i=0; i<TamRede; i++)
	{
		for (j=i; j<TamRede; j++)
		{
			SomaDiferenca +=
				(OptNet2->TopologiaVirtual[i][j] ^ OptNet1->TopologiaVirtual[i][j]) +
				(OptNet2->TopologiaVirtual[j][i] ^ OptNet1->TopologiaVirtual[j][i]);
		}
	}
	return ((float)SomaDiferenca/(TamRede*(TamRede-1)));
}

