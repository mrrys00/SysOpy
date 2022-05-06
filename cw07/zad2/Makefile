MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0 -g -pthread -lrt
COOK = ./cook
DELIVERER = ./deliverer
MAIN = ./zad2
ARGS = 9 7 

.PHONY: build_deliverer
build_deliverer:
	$(GCCF) -o $(DELIVERER) $(DELIVERER).c 

.PHONY: build_cook
build_cook:
	$(GCCF) -o $(COOK) $(COOK).c 

.PHONY: build_main
build_main:
	$(GCCF) -o $(MAIN) $(MAIN).c

.PHONY: build_all
build_all:
	make build_deliverer
	make build_cook
	make build_main

.PHONY: run
run:
	make build_all
	$(MAIN) $(ARGS)

.PHONY: clean
clean:
	rm *.o $(COOK) $(DELIVERER) $(MAIN)