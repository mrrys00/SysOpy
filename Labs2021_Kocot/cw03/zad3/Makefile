MAIN = ./finder

default:
	make build
	make default_run

build: finder.c
	gcc finder.c -o $(MAIN) -Wall

default_run: finder
	$(MAIN) data banana 3