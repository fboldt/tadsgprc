compile: 
	make geradorDeMatrizes
	
clean:
	@rm *.bin *.o *~

testaToolkit:
	gcc toolkit.c testaToolkit.c -o testaToolkit.bin -Wall
	
testaMatsol:
	gcc toolkit.c matsol.c testaMatsol.c -o testaMatsol.bin -Wall -lm

geradorDeMatrizes:
	gcc toolkit.c mattraf.c mattraf_io.c gera_mattraf.c geradorDeMatrizes.c -o geradorDeMatrizes.bin -Wall

solucoesAleatorias:
	gcc glpkwrapper.c matsol.c toolkit.c mattraf.c mattraf_io.c solucoesAleatorias.c -o solucoesAleatorias.bin -Wall -lglpk
	
ivtd_hmax:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c mattraf_io.c ivtd_hmax.c -o ivtd_hmax.bin -Wall -lglpk -lm
	
testa_ga: testa_ga.c ga.c optical_network.c toolkit.c
	gcc testa_ga.c ga.c optical_network.c toolkit.c glpkwrapper.c -o testa_ga -Wall -lglpk -g


