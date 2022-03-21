GCCF = gcc -Wall -ggdb
OUTPUT = ./zad1
OUTFILE = ./big_new.txt

build:
	$(GCCF) -c zad1.c
	$(GCCF) zad1.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) ./big.txt $(OUTFILE)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT) $(OUTFILE)
