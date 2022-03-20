CC = gcc -Wall -std=gnu11 -ggdb

run: main
	@./main.o

main:
	@$(CC) main.c -o main.o -pthread

clean:
	rm *.o
