#include "scip/scip.h"

// Inicializa estruturas de dados do MILP para o SCIP,
void swInicializaSCIP(SCIP *scip) {
   SCIP_CALL(SCIPcreate(&scip)); 			//inicializar SCIP
   SCIP_CALL(SCIPincludeDefaultPlugins(scip)); 		//incluir os plugins padrão
}

// Faz a leitura do problema de um arquivo em formato FreeMPS e gera o modelo linear do SCIP na memória
void swCarregaMILPFreeMPS(SCIP *scip, char *freemps){
   SCIP_CALL(SCIPreadProb(scip,freemps,NULL)); 		//Le problema de um arquivo freeMPS
}

// Resolve o problema MILP
double swResolveLP(SCIP *scip){

   SCIP_SOL* solMILP;

   SCIP_CALL(SCIPsolve(scip));				//Resolve o problema
   solMILP = SCIPgetBestSol(scip);
   return solMILP->obj;
}

// Libera memoria
void swFinalizaMILP(SCIP *scip)
{
   SCIP_CALL(SCIPfree(scip));				//Limpar a memoria usada pelo scip e sair
}












