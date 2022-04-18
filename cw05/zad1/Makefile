MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad1
COMMAN = commands

.PHONY: build
build:
	$(GCCF) -c $(OUTPUT).c
	$(GCCF) $(OUTPUT).o -o $(OUTPUT)

.PHONY: all
all:
	$(MAKE) build
	$(OUTPUT) $(COMMAN)

.PHONY: clean
clean:
	rm *.o $(OUTPUT)
