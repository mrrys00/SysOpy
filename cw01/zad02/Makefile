GCCF = gcc -O0 -Wall -std=c11 -ggdb
LIBS = ../zad01/

.PHONY: clean
clean:
	rm -f *.o *.a *.so

.PHONY: test
test:
	$(GCCF) -c $(LIBS)counter.c
	$(GCCF) -c test.c
	$(GCCF) test.o counter.o -o ./test