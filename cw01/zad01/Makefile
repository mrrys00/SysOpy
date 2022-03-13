CC = gcc -Wall -std=c11 -ggdb

static:
	$(CC) -c lib.c
	ar rcs libmerge.a lib.o

shared:
	$(CC) -c -fPIC lib.c
	$(CC) -shared -fPIC -o libmerge_shared.so lib.o

all: static shared 

clean:
	rm -f *.o *.a *.so
