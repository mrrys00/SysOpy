MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad2

DATE = "data"
SENDER = "nadawca"

EMAIL = "ala@ma.kota"
TITLE = "AlaMaKota"
BODY = "ALA niema Kota"

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
	$(OUTPUT) $(EMAIL) $(TITLE) $(BODY)
	$(OUTPUT) $(DATE)
	$(OUTPUT) $(SENDER)

.PHONY: clean
clean:
	rm *.o $(OUTPUT)
