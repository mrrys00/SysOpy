PROG=main
LDLIBS=-pthread
CC = gcc

main: main.c
	$(GCCF) $^ -o $@ $(CFLAGS) -lpthread

test: ${PROG}
	@echo -e "\n\nTESTING\n\n"
	@./${PROG}

clean:
	rm ${PROG}

.PHONY: clean test

all: main