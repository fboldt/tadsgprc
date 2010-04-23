compile: 
	make geradorDeMatrizes
	
clean:
	@rm *.bin *~

testaToolkit:
	gcc toolkit.c testaToolkit.c -o testaToolkit.bin -Wall
	
testeSimilaridade:
	gcc toolkit.c matsol.c testeSimilaridade.c -o testeSimilaridade.bin -Wall -lm

geradorDeMatrizes:
	gcc toolkit.c mattraf.c gera_mattraf.c geradorDeMatrizes.c -o geradorDeMatrizes.bin -Wall

solucoesAleatorias:
	gcc glpkwrapper.c matsol.c toolkit.c mattraf.c solucoesAleatorias.c -o solucoesAleatorias.bin -Wall -lglpk
	
ivtd_hmax:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_hmax.c -o ivtd_hmax.bin -Wall -lglpk -lm
	
testa_ga: testa_ga.c ga.c optical_network.c toolkit.c
	gcc testa_ga.c ga.c optical_network.c toolkit.c glpkwrapper.c -o testa_ga -Wall -lglpk -g

ivtd_hmax_esp:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_hmax_esp.c -o ivtd_hmax_esp.bin -Wall -lglpk -lm

ivtd_hmax_original:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_hmax_original.c -o ivtd_hmax_original.bin -Wall -lglpk -lm
	
segatto:
	gcc segatto.c glpkwrapper.c toolkit.c matsol.c mattraf.c -o segatto.bin -Wall -lglpk
