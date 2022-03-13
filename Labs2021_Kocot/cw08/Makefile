SRC="thread.c"
MAIN="./thread"
THREADS=16
MODE="block"
FILEIN="casablanca.ascii.pgm"
FILEOUT="casablanca_inv.pgm"

all:
	make build
	make run

build:
	gcc -o $(MAIN) $(SRC) -g -Wall -pthread

run:
	$(MAIN) $(THREADS) $(MODE) $(FILEIN) $(FILEOUT)

clean:
	rm -f $(FILEOUT)
	rm -f $(MAIN)
