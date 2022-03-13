MAIN = ./interpreter

all:
	make build
	make testall

build: interpreter.c
	gcc -o $(MAIN) interpreter.c -Wall

testall: interpreter
	$(MAIN) example.txt
	$(MAIN) input.txt

clean:
	rm -f $(MAIN)
