CC = gcc -Wall -std=gnu11 -ggdb
NAME = client

server:
	@$(CC) server.c -o server.o -pthread
	@./server.o 5000 ~/unix_socket

client:
	@$(CC) client.c -o client.o -pthread
	@./client.o $(NAME) unix_socket ~/unix_socket

clean:
	rm *.o
