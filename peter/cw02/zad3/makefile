CC = gcc -Wall -std=c11 -ggdb
RUN_ARGS = a example.txt

run_lib:
	@$(CC) main_lib.c -o main_lib.o -lm
	@./main_lib.o $(RUN_ARGS)

run_sys:
	@$(CC) main_sys.c -o main_sys.o -lm
	@./main_sys.o $(RUN_ARGS)

all: run_lib run_sys

clean:
	rm *.o pomiar_zad*.txt
