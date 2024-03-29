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
		
testa_ga: testa_ga.c ga.c optical_network.c toolkit.c
	gcc testa_ga.c ga.c optical_network.c toolkit.c glpkwrapper.c -o testa_ga -Wall -lglpk -g

ivtd_hmax_esp:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_hmax_esp.c -o ivtd_hmax_esp.bin -Wall -lglpk -lm

ivtd_hmax_original:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_hmax_original.c -o ivtd_hmax_original.bin -Wall -lglpk -lm
	
ivtd_ftnet_original:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_ftnet_original.c -o ivtd_ftnet_original.bin -Wall -lglpk -lm

rhdla:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c rhlda.c -o rhlda.bin -Wall -lglpk -lm

rdla:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c rlda.c -o rlda.bin -Wall -lglpk -lm

ivtd_ftnet_esp:
	gcc matsol.c glpkwrapper.c ivtd.c toolkit.c mattraf.c ivtd_ftnet_esp.c -o ivtd_ftnet_esp.bin -Wall -lglpk -lm

segatto:
	gcc segatto.c glpkwrapper.c toolkit.c matsol.c mattraf.c -o segatto.bin -Wall -lglpk

lbi_hmax_glass:
	gcc lbi_hmax_glass.c mattraf.c toolkit.c -o lbi_hmax_glass -lglpk -Wall
	
milp_ftnet_glass:
	gcc milp_ftnet_glass.c mattraf.c toolkit.c -o milp_ftnet_glass -lglpk -Wall
