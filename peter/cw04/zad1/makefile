CC = gcc -Wall -ggdb
RUN_ARGS = ignore


run_fork:
	@$(CC) fork_test.c -o fork_test.o
	@./fork_test.o $(RUN_ARGS)

build_excuted: 
	@$(CC) executed.c -o executed.o

run_exec: build_excuted
	@$(CC) exec_test.c -o exec_test.o
	@./exec_test.o $(RUN_ARGS)

all: run

clean:
	rm *.o
