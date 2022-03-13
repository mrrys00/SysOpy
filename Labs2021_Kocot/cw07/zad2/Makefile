MAIN = ./main
PIZZ = ./pizzaiolo
DELIV = ./delivery

all:
	make build
	$(MAIN)

build:
	gcc -o $(MAIN) main.c -Wall -g -pthread -lrt
	gcc -o $(PIZZ) pizzaiolo.c -Wall -g -pthread -lrt
	gcc -o $(DELIV) delivery.c -Wall -g -pthread -lrt

clean:
	rm -f $(MAIN)
	rm -f $(PIZZ)
	rm -f $(DELIV)
