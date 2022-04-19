MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad2
COMMAN = commands

DATE = "data"
SENDER = "nadawca"


.PHONY: prepare
prepare:
	sudo apt install mailutils

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
