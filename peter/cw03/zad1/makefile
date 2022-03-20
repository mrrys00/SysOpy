CC = gcc -Wall -std=c11 -ggdb
RUN_ARGS = 10


run:
	@$(CC) main.c -o main.o
	@./main.o $(RUN_ARGS)

all: run

clean:
	rm *.o
