GCCF = gcc -Wall -ggdb
OUTPUTS = ./zad1s
OUTPUTL = ./zad1l
INFILE = ./big.txt
OUTFILE = ./big_new.txt

build_sys:
	$(GCCF) -c zad1s.c
	$(GCCF) zad1s.o -o $(OUTPUTS)
build_lib:
	$(GCCF) -c zad1l.c
	$(GCCF) zad1l.o -o $(OUTPUTL)

.PHONY: all
all:
	make build_sys
	make build_lib
	$(OUTPUTS) $(INFILE) $(OUTFILE) >  pomiar_zad_1.txt
	$(OUTPUTL) $(INFILE) $(OUTFILE) >> pomiar_zad_1.txt

.PHONY: clean
clean:
	rm -f *.o $(OUTPUTS) $(OUTPUTL) $(OUTFILE)
