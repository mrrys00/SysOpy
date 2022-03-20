CC = gcc -Wall -std=gnu11 -ggdb
RUNARGS = ./interpreted.txt

server:
	@$(CC) server.c -o server.o -lrt
	@./server.o ${RUNARGS}

client:
	@$(CC) client.c -o client.o -lrt
	@./client.o ${RUNARGS}

clean:
	rm *.o
