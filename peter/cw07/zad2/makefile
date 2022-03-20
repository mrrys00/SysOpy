CC = gcc -Wall -std=gnu11 -ggdb
MAIN_ARGS = 3 3

run: main

main: cook deliverer
	@$(CC) main.c -o main.o -lrt -pthread 
	@./main.o ${MAIN_ARGS}

cook:
	@$(CC) cook.c -o cook.o -lrt -pthread

deliverer:
	@$(CC) deliverer.c -o deliverer.o -lrt -pthread

clean:
	rm *.o
