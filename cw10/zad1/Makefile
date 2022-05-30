MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -Ofast -pthread
MAIN = ./zad1

.PHONY: build
build:
	$(GCCF) -o $(MAIN) $(MAIN).c 

.PHONY: run
run:
	$(MAIN)

.PHONY: all
all:
	make build
	make run

.PHONY: clean
clean:
	rm -r $(MAIN)
