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
	make build_exec
	make build_fork
	make build_test

test_exec:
	$(OUTPUTEXEC) ignore
	$(OUTPUTEXEC) handler
	$(OUTPUTEXEC) mask
	$(OUTPUTEXEC) pending

test_fork:
	$(OUTPUTFORK) ignore
	$(OUTPUTFORK) handler
	$(OUTPUTFORK) mask
	$(OUTPUTFORK) pending

test_all:
	echo "################ report 2 ################" >  $(REPORT)
	echo "################ exec test ################" >> $(REPORT)
	make test_exec
	echo "################ fork test ################" >> $(REPORT)
	make test_fork
	echo "################ report\0 ################" >> $(REPORT)

.PHONY: all
all:
	make build_all
	make test_all

.PHONY: clean
clean:
	rm *.o $(OUTPUTFORK) $(OUTPUTEXEC) $(OUTPUTTEST)
