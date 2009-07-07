#ifndef __OPTICAL_NETWORK_H_
#define __OPTICAL_NETWORK_H_

#define TRUE  1
#define FALSE 0
#define MAX_TAM_REDE 64

//-----------------------------------------------------------------------------

typedef struct 
{
	int *GrauLogicoEntrada;
	int *GrauLogicoSaida;
	char **TopologiaVirtual;

} TOptNet;

//-----------------------------------------------------------------------------

// Calcula o FTj (trafico restransmitido eletronicamente) de cada no em um anel
float OPT_FT_J(int Indice, int TamRede, int *Anel, float **Matriz)
;

// Calcula o FTnet somando todos os FTj's
float OPT_FTnet(int TamRede, int *Anel, float **Matriz)
;

// Calcula o carregamento de trafego no enlace I->J de um Anel
float OPT_Demanda_IJ(int I, int J, int TamRede, int *Anel, float **Matriz)
;

// Retorna o Congestionamento: quantidade de trafego designada ao enlace logico mais carregado
float OPT_Congestionamento(int TamRede, int *Anel, float **Matriz)
;

//-----------------------------------------------------------------------------

// Funcao que seleciona o melhor anel basico dentre
// diversos aneis aleatorios gerados
// FuncaoCusto pode ser Congestionamento ou FTnet
void OPT_SelecionaAnelBasico(float (*FuncaoCusto)(),
	int Iteracoes, int N, float **MatrizTrafego, int *AnelBasico)
;

// Adiciona Enlace a Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeAdicionarEnlace for TRUE
// Esta funcao atualiza GrauLogicoSaida e GrauLogicoEntrada
void OPT_AdicionaEnlace(int Lin, int Col, TOptNet *OptNet)
;

// Adiciona Enlace em uma Linha da Matriz de Topologia Virtual
// varrendo a linha em busca de uma coluna valida para adicionar enlace.
// Retorna TRUE se algum enlace foi adicionado, e FALSE caso contrario
int OPT_AdicionaEnlaceLinha(int Linha, int GrauLimite, int TamRede, TOptNet *OptNet)
;

// Funcao que checa se um enlace pode ser removido em uma posicao
// Essa funcao eh usada na funcao CorrigeGrauLogico
int OPT_PodeRemoverEnlace(int Lin, int Col, int TamRede, int *Anel, TOptNet *OptNet)
;

// Remove Enlace da Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeRemoverEnlace for TRUE
void OPT_RemoveEnlace(int Lin, int Col, TOptNet *OptNet)
;

// Remove um Enlace de uma Linha da Matriz de Topologia Virtual
// varrendo a Linha em busca de coluna valida para remover enlace
void OPT_RemoveEnlaceLinha(int Linha, int TamRede, int *Anel, TOptNet *OptNet)
;

// Remove um Enlace de uma Coluna da Matriz de Topologia Virtual
// varrendo a Coluna em busca de linha valida para remover enlace
void OPT_RemoveEnlaceColuna(int Coluna, int TamRede, int *Anel, TOptNet *OptNet)
;

// Funcao que atualiza os vetores GrauLogicoSaida e GrauLogicoEntrada
// Grau Logico de saida eh definido pelas linhas
// Grau Logico de entrada eh definido pelas colunas
void OPT_AtualizaGrauLogico(int TamRede, TOptNet *OptNet)
;

//-----------------------------------------------------------------------------

//Corrige o grau logico das linhas (saida) removendo enlaces de um individuo
void OPT_CorrigeGrauLogicoSaida(int TamRede, int *Anel, int GrauLimite, TOptNet *OptNet)
;

//Corrige o grau logico das colunas (entrada) removendo enlaces de um individuo
void OPT_CorrigeGrauLogicoEntrada(int TamRede, int *Anel, int GrauLimite, TOptNet *OptNet)
;

// Completa a Matriz de Topologia Virtual
// adicionando enlaces ate atingir Grau Limite
void OPT_CompletaMatrizTopologiaVirtual(int GrauLimite, int TamRede, TOptNet *OptNet)
;

// Checa se um enlace pode ser adicionado em uma posicao
// Esta funcao eh usada na funcao CompletaMatrizTopologiaVirtual
int OPT_PodeAdicionarEnlace(int Lin, int Col, int Grau, TOptNet *OptNet)
;

// Checa se um enlace pode ser adicionado em uma posicao
// Admite-se multiplos enlaces entre um mesmo par de nos
// (Multiplicidade)
int OPT_PodeAdicionarEnlaceMultiplo(int Lin, int Col, int Grau, TOptNet *OptNet)
;

// Adiciona Enlace Multiplo a Matriz Topologia Virtual do individuo
// Deve ser usada apenas se PodeAdicionarEnlaceMultiplo for TRUE
// Esta funcao atualiza GrauLogicoSaida e GrauLogicoEntrada
void OPT_AdicionaEnlaceMultiplo(int Lin, int Col, TOptNet *OptNet)
;

// Mapeia Anel Basico na Matriz Topologia Virtual do TOptNet
void OPT_MapeiaAnelMatrizTopologiaVirtual(int TamRede, int *Anel, TOptNet *OptNet)
;

//-----------------------------------------------------------------------------

//Retorna o tamanho Contiguo da estrutura TOptNet
int OPT_TamanhoContiguoOptNet(int TamRede)
;

//Aloca uma estrutura TOptNet
TOptNet *OPT_AlocaOptNet(int TamRede)
;

//Faz o mapeamento da estrutura TOptNet
void OPT_MapeiaContiguoOptNet(int TamRede, TOptNet *OptNet)
;

//Inicializa uma estrutura OptNet com zeros
//utilizada apenas para testes
void OPT_InicializaOptNet(int N, TOptNet *OptNet)
;

//Aloca e mapeia uma estrutura OptNet
TOptNet *OPT_CriaOptNet(int TamRede)
;

//Libera a área de memória alocada por uma estrutura TOptNet
void OPT_DestroiOptNet(TOptNet *OptNet)
;

//Imprime a matriz de topologia virtual e os graus logicos de saida e entrada
void OPT_ImprimeTOptNet(int TamRede, TOptNet *OptNet)
;

// Copia Linha completa da Matriz de Topologia Virtual
void OPT_CopiaLinhaTopologia(int Lin, int TamRede, TOptNet *OptNet1, TOptNet *OptNet2)
;


// Copia Coluna completa da Matriz de Topologia Virtual
void OPT_CopiaColunaTopologia(int Col, int TamRede, TOptNet *OptNet1, TOptNet *OptNet2)
;

// Faz a comparação da quantidades de enlaces equivalentes em
// uma matriz e retorna esse valor em percentual
float OPT_Diversidade(int TamRede, TOptNet *OptNet1, TOptNet *OptNet2)
;

#endif


