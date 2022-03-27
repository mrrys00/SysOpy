GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad1

build:
	$(GCCF) -c zad1.c
	$(GCCF) zad1.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) 15

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT)
