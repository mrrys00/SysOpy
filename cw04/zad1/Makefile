MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUTEXEC = ./zad1exec
OUTPUTFORK = ./zad1fork
OUTPUTTEST = ./zad1test
REPORT = report2.txt

build_exec:
	$(GCCF) -c $(OUTPUTEXEC).c
	$(GCCF) $(OUTPUTEXEC).o -o $(OUTPUTEXEC)
build_fork:
	$(GCCF) -c $(OUTPUTFORK).c
	$(GCCF) $(OUTPUTFORK).o -o $(OUTPUTFORK)
build_test:
	$(GCCF) -c $(OUTPUTTEST).c
	$(GCCF) $(OUTPUTTEST).o -o $(OUTPUTTEST)
build_all:
	$(MAKE) build_exec
	$(MAKE) build_fork
	$(MAKE) build_test

test_exec:
	$(OUTPUTEXEC) ignore
	$(OUTPUTEXEC) mask
	$(OUTPUTEXEC) pending

test_fork:
	$(OUTPUTFORK) ignore
	$(OUTPUTFORK) handler
	$(OUTPUTFORK) mask
	$(OUTPUTFORK) pending

test_all:
	$(MAKE) test_exec
	$(MAKE) test_fork

.PHONY: all
all:
	$(MAKE) build_all
	$(MAKE) test_all > $(REPORT)

.PHONY: clean
clean:
	rm *.o $(OUTPUTFORK) $(OUTPUTEXEC) $(OUTPUTTEST)
