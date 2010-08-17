#ifndef __SCIPWRAPPER_H_
#define __SCIPWRAPPER_H_
#include "scip/scip.h"

// Inicializa estruturas de dados do MILP para o SCIP,
void swInicializaSCIP(SCIP *scip);

// Faz a leitura do problema de um arquivo em formato FreeMPS e gera o modelo linear do SCIP na memória
void swCarregaMILPFreeMPS(SCIP *scip, char *freemps);

// Resolve o problema MILP
double swResolveLP(SCIP *scip);

// Libera memoria
void swFinalizaMILP(SCIP *scip);

#endif












