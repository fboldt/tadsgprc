#include "solucoesAleatorias.c"

int segatto(int argc, char **argv)
{
	int testes, gl, tamRede;
	LPX *lp;
	testes = 100000;
	if(argc < 4)
	{
		printf("Uso correto:\n %s modelo.mod dados.dat graulogico [testes]\n", argv[0]);
		exit(0);
	}
	if(argc > 4)
		testes = atoi(argv[4]);
	gl = atoi(argv[3]);
	tamRede = mtLocalizaParametroInt(argv[2], "N");
	lp = gwCarregaModeloLP(argv[1], argv[2], "foo.txt");
	gwAtribuiParametrosLP(lp);
	gwCriaIndiceLP(lp);
	
	srand(time(NULL));
	testeGL(lp, tamRede, gl, testes);
	
	gwFinalizaLP(lp);
	
	return 0;
}

int main(int argc, char **argv)
{
	//return solucoesAleatorias(argc, argv);
	return segatto(argc, argv);
}


