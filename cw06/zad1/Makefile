MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
CLIENT = ./client
SERVER = ./server

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
	$(SERVER)

.PHONY: run_client
run_client:
	make build_client
	$(CLIENT)

.PHONY: clean
clean:
	rm *.o $(CLIENT) $(SERVER)
