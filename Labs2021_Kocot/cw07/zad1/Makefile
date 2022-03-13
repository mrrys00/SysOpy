MAIN = ./main
PIZZ = ./pizzaiolo
DELIV = ./delivery

all:
	make build
	$(MAIN)

build:
	gcc -o $(MAIN) main.c -Wall -g
	gcc -o $(PIZZ) pizzaiolo.c -Wall -g
	gcc -o $(DELIV) delivery.c -Wall -g

clean:
	rm -f $(MAIN)
	rm -f $(PIZZ)
	rm -f $(DELIV)
