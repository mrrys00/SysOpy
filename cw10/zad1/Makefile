MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -Ofast -pthread
CLIENT = ./client
SERVER = ./server
PORT = 2137 
SOCKET = socket
NAME = client

.PHONY: build_server
build_server:
	$(GCCF) -c $(SERVER).c
	$(GCCF) $(SERVER).o -o $(SERVER)

.PHONY: build_client
build_client:
	$(GCCF) -c $(CLIENT).c
	$(GCCF) $(CLIENT).o -o $(CLIENT)

.PHONY: run_server
run_server:
	make build_server
	$(SERVER) $(PORT) ./$(SOCKET)

.PHONY: run_client1
run_client1:
	make build_client
	$(CLIENT) $(NAME) $(SOCKET) ./$(SOCKET)

.PHONY: run_client2
run_client2:
	make build_client
	$(CLIENT) $(NAME)2 $(SOCKET) ./$(SOCKET)

.PHONY: clean
clean:
	rm *.o $(CLIENT) $(SERVER) $(SOCKET)
