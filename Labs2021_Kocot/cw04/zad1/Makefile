MAIN = ./signal
EXEC = ./signal_exec

all:
	make build
	make test-all

build:
	gcc signal.c -o $(MAIN) -Wall
	gcc signal_exec.c -o $(EXEC) -Wall

test-all:
	$(MAIN) handler
	$(MAIN) mask
	$(MAIN) ignore
	$(MAIN) pending

clean:
	rm -f $(MAIN)
