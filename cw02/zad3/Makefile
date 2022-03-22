GCCF = gcc -Wall -ggdb
OUTPUT = ./zad3
#OUTFILE = ./big_new.txt
PATH1 = "/home/mrrys00/Studia/4.Semestr/Systemy Operacyjne/SysOpy/cw02"
PATH2 = "/dev/input/"

build:
	$(GCCF) -c zad3nftw.c
	$(GCCF) zad3nftw.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) $(PATH1)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT) $(OUTFILE)

test:
	$(GCCF) -c test.c
	$(GCCF) test.o -o ./testt
	./testt
