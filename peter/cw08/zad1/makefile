CC = gcc -Wall -std=gnu11 -ggdb
MAIN_ARGS = 3 numbers lena.ascii.pgm result.pgm

run: main
	@./main.o ${MAIN_ARGS}

main:
	@$(CC) main.c -o main.o -pthread

test: main
	@-rm Times.txt
	@./main.o 1 numbers lena.ascii.pgm result.pgm
	@./main.o 1 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 1 block lena.ascii.pgm result.pgm
	@./main.o 1 block apollonian_gasket.ascii.pgm result.pgm

	@./main.o 2 numbers lena.ascii.pgm result.pgm
	@./main.o 2 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 2 block lena.ascii.pgm result.pgm
	@./main.o 2 block apollonian_gasket.ascii.pgm result.pgm

	@./main.o 4 numbers lena.ascii.pgm result.pgm
	@./main.o 4 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 4 block lena.ascii.pgm result.pgm
	@./main.o 4 block apollonian_gasket.ascii.pgm result.pgm

	@./main.o 8 numbers lena.ascii.pgm result.pgm
	@./main.o 8 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 8 block lena.ascii.pgm result.pgm
	@./main.o 8 block apollonian_gasket.ascii.pgm result.pgm

	@./main.o 16 numbers lena.ascii.pgm result.pgm
	@./main.o 16 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 16 block lena.ascii.pgm result.pgm
	@./main.o 16 block apollonian_gasket.ascii.pgm result.pgm

	@./main.o 32 numbers lena.ascii.pgm result.pgm
	@./main.o 32 numbers apollonian_gasket.ascii.pgm result.pgm
	@./main.o 32 block lena.ascii.pgm result.pgm
	@./main.o 32 block apollonian_gasket.ascii.pgm result.pgm


clean:
	rm *.o
