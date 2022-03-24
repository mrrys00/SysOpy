GCCF = gcc -Wall -ggdb
OUTPUTS = ./zad2s
OUTPUTL = ./zad2l
INFILE = ./big.txt
CHAR = c

build_sys:
	$(GCCF) -c zad2s.c
	$(GCCF) zad2s.o -o $(OUTPUTS)
build_lib:
	$(GCCF) -c zad2l.c
	$(GCCF) zad2l.o -o $(OUTPUTL)

.PHONY: all
all:
	make build_sys
	make build_lib
	$(OUTPUTS) $(INFILE) $(CHAR) >  pomiar_zad_2.txt
	$(OUTPUTL) $(INFILE) $(CHAR) >> pomiar_zad_2.txt

.PHONY: clean
clean:
	rm -f *.o $(OUTPUTS) $(OUTPUTL) $(CHAR)
