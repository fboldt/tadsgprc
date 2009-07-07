#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "toolkit.h"

// Compilar:
// gcc testa_toolkit.c toolkit.c -o testa_toolkit.bin -Wall -g
// Executar:
// ./testa_toolkit.bin <tamanho>

// Variaveis Globais de Teste
int TAM, *vetor, *vetor2;
char **matriz;
float t;
struct timeval t1, t2;


void TestaVetor()
{
	#define VEZES 100
	int k;
	printf("\nTestando funções que manipulam VETORES...\n\n");

	vetor  = (int *) TK_AlocaVetor(TAM, sizeof(int));
	vetor2 = (int *) TK_AlocaVetor(TAM, sizeof(int));

	printf("\nInicializando VETOR (%d)...\n", TAM);
	TK_InicializaVetor_int(TAM, vetor);
	TK_InicializaVetor_int(TAM, vetor2);

	// Arranjo Aleatorio de vetor
	printf("\nArranjando VETOR aleatorio (3,N)...\t");
	gettimeofday(&t1, NULL);
	for (k=0; k<VEZES; k++)
	{
		TK_ArranjoVetorAleatorio_int(3, TAM, vetor);
	}
	gettimeofday(&t2, NULL);
	t = (float) (t2.tv_sec-t1.tv_sec) + 1E-6 * (float) (t2.tv_usec-t1.tv_usec);
	printf("%.3f (microsegundos)\n", t*1E6 / (float) VEZES);
	TK_ImprimeVetor_int(TAM, vetor);

	// Permutacao Aleatoria de vetor
	printf("\nPermutando VETOR aleatorio...\t\t");
	gettimeofday(&t1, NULL);
	for (k=0; k<VEZES; k++)
	{
		TK_PermutacaoVetorAleatorio_int(TAM, vetor);
	}
	gettimeofday(&t2, NULL);
	t = (float) (t2.tv_sec-t1.tv_sec) + 1E-6 * (float) (t2.tv_usec-t1.tv_usec);
	printf("%.3f (microsegundos)\n", t*1E6 / (float) VEZES);
	TK_ImprimeVetor_int(TAM, vetor);

	TK_PermutacaoVetorAleatorio_int(TAM, vetor);
	TK_CopiaVetor_int(TAM, vetor, vetor2);

	printf("\nOrdenando VETOR (Selecao)...\t\t");
	gettimeofday(&t1, NULL);
	TK_OrdenaVetor_int(TAM, vetor);	// Selecao
	gettimeofday(&t2, NULL);
	t = (float) (t2.tv_sec-t1.tv_sec) + 1E-6 * (float) (t2.tv_usec-t1.tv_usec);
	printf("%.0f (microsegundos)\n", t*1E6);
	TK_ImprimeVetor_int(TAM, vetor);

	printf("\nOrdenando VETOR (Heapsort)...\t\t");
	gettimeofday(&t1, NULL);
	TK_Heapsort(Maior, TAM, vetor2);		// Heapsort
	gettimeofday(&t2, NULL);
	t = (float) (t2.tv_sec-t1.tv_sec) + 1E-6 * (float) (t2.tv_usec-t1.tv_usec);
	printf("%.0f (microsegundos)\n", t*1E6);
	TK_ImprimeVetor_int(TAM, vetor2);

	printf("Liberando VETOR...\n");
	TK_LiberaVetor(vetor);
}

void TestaMatriz()
{
	int i, j;
	printf("\nTestando funções que manipulam MATRIZES...\n\n");

	matriz = (char**) TK_AlocaMatriz(TAM, TAM, sizeof(char));

	printf("Inicializando MATRIZ (%dx%d)...\n", TAM, TAM);
	TK_InicializaMatriz_char(TAM, TAM, matriz);

	TK_ImprimeMatriz_char(TAM, TAM, matriz);

	printf("Escreve na MATRIZ...\n");
	// escreve na matriz usando notacao matricial
	for(i=0; i<TAM; i++)
	{
		for(j=0; j<TAM; j++)
			matriz[i][j] = i*10+j;
	}

	TK_ImprimeMatriz_char(TAM, TAM, matriz);

	printf("Liberando MATRIZ...\n");
	TK_LiberaMatriz((void**) matriz);
}


int main(int argc, char **argv)
{
	gettimeofday(&t1, NULL);
	srand(t1.tv_sec + t1.tv_usec);
	
	if (argc > 1)
		TAM = atoi(argv[1]);
	else
	{
		printf("Uso: %s <Tamanho>\n", argv[0]);
		exit(-1);
	}

	TestaVetor();
	TestaMatriz();
	
	return 0;
}

