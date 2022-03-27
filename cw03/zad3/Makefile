GCCF = gcc -Wall -Wint-to-pointer-cast -ggdb -O0
OUTPUT = ./zad3
ARGS = . expression 3

build:
	$(GCCF) -c zad3.c
	$(GCCF) zad3.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) $(ARGS)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT)
