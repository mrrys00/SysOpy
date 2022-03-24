GCCF = gcc -Wall -ggdb
OUTPUTN = ./zad3nftw
OUTPUTD = ./zad3dir
PATH1 = "."
PATH2 = "/dev/input"

build_nftw:
	$(GCCF) -c zad3nftw.c
	$(GCCF) zad3nftw.o -o $(OUTPUTN)

build_dir:
	$(GCCF) -c zad3dir.c
	$(GCCF) zad3dir.o -o $(OUTPUTD)

.PHONY: all
all:
	make build_nftw
	$(OUTPUTN) $(PATH2)
	make build_dir
	$(OUTPUTD) $(PATH2)

.PHONY: clean
clean:
	rm -f *.o $(OUTPUTN) $(OUTPUTD)
