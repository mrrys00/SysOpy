CC = gcc -Wall -std=gnu11 -ggdb
RUNARGS = ./interpreted.txt

server:
	@$(CC) server.c -o server.o
	@./server.o ${RUNARGS}

client:
	@$(CC) client.c -o client.o
	@./client.o ${RUNARGS}

clean:
	rm *.o
