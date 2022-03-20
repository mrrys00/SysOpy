CC = gcc -Wall -ggdb
RUN_ARGS = ignore


run:
	@$(CC) main.c -o main.o
	@./main.o $(RUN_ARGS)

all: run

clean:
	rm *.o
