GCCF = gcc -Wall -ggdb
OUTPUT = ./zad3
PATH1 = "/home/mrrys00/Studia/4.Semestr/Systemy Operacyjne/SysOpy/cw02"
PATH2 = "/dev/input"

build:
	$(GCCF) -c zad3nftw.c
	$(GCCF) zad3nftw.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) $(PATH2)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT)
