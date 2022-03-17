GCCF = gcc -O0 -Wall -std=c17 -ggdb

.PHONY: all
all: static_lib shared_lib

static_lib:
	$(GCCF) -c counter.c
	ar rcs cntlib.a counter.o

shared_lib:
	$(GCCF) -c -fPIC counter.c
	$(GCCF) -shared -fPIC -o cntlib_shd.so counter.o

.PHONY: clean
clean:
	rm -f *.o *.a *.so
