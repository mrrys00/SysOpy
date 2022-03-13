SRC=santa.c
MAIN=./santa

all:
	make build
	make run

build:
	gcc -o $(MAIN) $(SRC) -g -Wall -pthread

run:
	$(MAIN)

clean:
	rm -f $(FILEOUT)
	rm -f $(MAIN)
