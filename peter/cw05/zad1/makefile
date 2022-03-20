CC = gcc -Wall -std=c11 -ggdb
RUNARGS = ./interpreted.txt

run:
	@$(CC) main.c -o main.o
	@./main.o ${RUNARGS}

clean:
	rm *.o
