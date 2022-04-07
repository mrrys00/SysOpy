MAKEFLAGS += --no-print-directory 
MAKEFLAGS += --ignore-errors

GCCF = gcc -Wall -ggdb
OUTPUT = ./zad2

build:
	$(GCCF) -c $(OUTPUT).c
	$(GCCF) $(OUTPUT).o -o $(OUTPUT)

tests:
	$(OUTPUT) 0
	$(OUTPUT) 1
	$(OUTPUT) 2

.PHONY: all
all:
	make build
	make tests 

.PHONY: clean
clean:
	rm *.o $(OUTPUT)
