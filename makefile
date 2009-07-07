compile: 
	make geradorDeMatrizes
	
clean:
	@rm *.bin *.o *~

toolkit:
	gcc -c toolkit.c -Wall
	
testaToolkit:
	gcc toolkit.c testaToolkit.c -o testaToolkit.bin -Wall
	
glpkwrapper:
	gcc -c glpkwrapper.c -Wall

matsol:
	@make toolkit
	gcc -c matsol.c -Wall

testaMatsol:
	@make matsol
	gcc testaMatsol.c -o testaMatsol.bin -Wall "toolkit.o" "matsol.o" -lm

mattraf:
	@make toolkit
	gcc -c mattraf.c -Wall

mattraf_io:
	@make mattraf
	gcc -c mattraf_io.c -Wall

gera_mattraf:
	@make mattraf_io
	gcc -c gera_mattraf.c -Wall

geradorDeMatrizes:
	@make gera_mattraf
	gcc geradorDeMatrizes.c -o geradorDeMatrizes.bin -Wall "toolkit.o" "mattraf.o" "mattraf_io.o" "gera_mattraf.o"

solucoesAleatorias:
	@make matsol
	@make glpkwrapper
	@make mattraf_io
	gcc solucoesAleatorias.c -o solucoesAleatorias.bin -Wall -lglpk "toolkit.o" "matsol.o" "glpkwrapper.o" "mattraf_io.o"
	
ivtd_hmax:
	@make ivtd
	@make matsol
	@make mattraf_io
	gcc ivtd_hmax.c -o ivtd_hmax.bin -Wall -lglpk "toolkit.o" "matsol.o" "glpkwrapper.o" "mattraf_io.o" "ivtd.o"
	
ivtd:
	@make glpkwrapper
	gcc -c ivtd.c -Wall
	
testa_ga: testa_ga.c ga.c optical_network.c toolkit.c
	gcc testa_ga.c ga.c optical_network.c toolkit.c glpkwrapper.c -o testa_ga -Wall -lglpk -g


