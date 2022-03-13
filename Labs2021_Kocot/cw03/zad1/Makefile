MAIN = ./replicant

default:
	make build
	make test

build: replicant.c
	gcc replicant.c -o $(MAIN) -Wall

test: replicant
	$(MAIN) 10
