CC = gcc -Wall -std=gnu11 -ggdb
MAIN_ARGS = 3 3

run: main

main: cook deliverer
	@$(CC) main.c -o main.o
	@./main.o ${MAIN_ARGS}

cook:
	@$(CC) cook.c -o cook.o

deliverer:
	@$(CC) deliverer.c -o deliverer.o

clean:
	rm *.o
