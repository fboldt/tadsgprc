#ifndef __MATTRAF_H_
#define __MATTRAF_H_

#define UNI 0
#define SD 1
#define SN 2
#define MAXDIST 3

#define mtioGravaVetMatTraf mtGravaVetMatTraf
#define mtioGravaMatTrafMathProg mtGravaMatTrafMathProg
#define mtioMaxElem mtMaxElem
#define mtioAlign mtAlign
#define mtioLocalizaParametroInt mtLocalizaParametroInt
#define mtioCarregaMatTraf mtCarregaMatTraf

typedef struct matTraf
{
	float **h;
	int ordem;
} *MatTraf;

typedef struct configMatTraf
{
	int tipoDistribuicao, ordem;
	float porcentagemDistribuicao, minP, maxP, minG, maxG;
} *ConfigMatTraf;

/* ===================================================== */



/* Construtor ConfigMatTraf */
ConfigMatTraf mtNewConfigMatTraf()
;

/* Destrutor ConfigMatTraf */
void mtDelConfigMatTraf(ConfigMatTraf cm)
;

/* Construtor MatTraf */
MatTraf mtNewMatTraf(int ordem)
;

/* Destrutor MatTraf */
void mtDelMatTraf(MatTraf mt)
;

/* Cria um vetor de instancias de MatTraf */
MatTraf* mtCriaInstancias(ConfigMatTraf confMat, int quantInst)
;

/* Desaloca um vetor de MatTrafs */
void mtLiberaVetorMatTraf(MatTraf* vetMT, int quantInst)
;

/* Cria uma instancia de MatTraf */
MatTraf mtCriaInstancia(ConfigMatTraf confMat)
;

/* Cria uma matriz uniforme */
float** mtUniform(ConfigMatTraf confMat)
;

/* Cria um matriz de super demanda */
float** mtSuperd(ConfigMatTraf confMat)
;

/* Cria um matriz de super nos */
float** mtSupern(ConfigMatTraf confMat)
;

/* Retorna um valor do tipo float entre nim e max */
float mtBetween(float min, float max)
;

/* Retorna o intervalo linear entre a e b */
float mtIntervaloLinear(float a, float b, int intervalo, int posicao)
;

/* Retorna a matriz posicao de um intervalo entre duas matrizes */
MatTraf mtMatTrafIntervalar(MatTraf m1, MatTraf m2, int intervalo, int posicao)
;

/* Cria vetor de MatTrafs intervalares */
MatTraf* mtVetMatTrafIntervalar(MatTraf m1, MatTraf m2, int intervalo)
;

/* Imprime no terminal de texto uma MatTraf */
void mtImprimeMatTraf(MatTraf mt)
;

/* Grava vetor de MatTraf no formato MatProg */
void mtGravaVetMatTraf(char *nomeBase, int tamVet, MatTraf *vetMT, char *param)
;

/* Grava matriz no formato de dados em MathProg */
void mtGravaMatTrafMathProg(char *fileName, MatTraf mt, char *param)
;

/* Retorna o maior elemento da matriz */
float mtMaxElem(int tam, float **m)
;
/* Retorna a quantidade de casas antes da vírgula do número n */
int mtAlign(float n)
;

/* Localiza parametro em arquivo de dados MathProg */
int mtLocalizaParametroInt(char *dados, char *parametro)
;

/* Realiza a leitura de um arquivo de dados do problema LP
Retorna: Estrutura MatTraf preenchida 
AVISO: FUNCAO EM TESTE!!! FUNCIONAMENTO IMPERFEITO!!! */
MatTraf mtCarregaMatTraf(char *NomeArqDados)
;

#endif


