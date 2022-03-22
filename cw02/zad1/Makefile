GCCF = gcc -Wall -ggdb
OUTPUT = ./zad1s
OUTFILE = ./big_new.txt

build:
	$(GCCF) -c zad1s.c
	$(GCCF) zad1s.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) ./big.txt $(OUTFILE)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT) $(OUTFILE)
