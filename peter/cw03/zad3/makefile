RUN_ARGS = ./ pattern 3
ARTEFACT = main.o
CC = gcc -Wall -std=c11 -ggdb

run:
	@$(CC) main.c -o $(ARTEFACT)
	@./$(ARTEFACT) $(RUN_ARGS)

all: run

clean:
	rm *.o
