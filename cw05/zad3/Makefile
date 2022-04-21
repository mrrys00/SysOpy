MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad3
CONS = ./cons
PROD = ./prod
CHECK = ./check
OUTCONSUMER = out_cons.txt
TEMP = ./temp/
REPORT = "report.txt"

RUNARGS = 4 1 10 $(TEMP)"producer%d_read.txt"

build_zad3:
	$(GCCF) -c $(OUTPUT).c 
	$(GCCF) $(OUTPUT).o -o $(OUTPUT)

build_cons:
	$(GCCF) -c $(CONS).c 
	$(GCCF) $(CONS).o -o $(CONS)

build_prod:
	$(GCCF) -c $(PROD).c -o $(PROD).o
	$(GCCF) $(PROD).o -o $(PROD)

build_check:
	$(GCCF) -c $(CHECK).c -o $(CHECK).o
	$(GCCF) $(CHECK).o -o $(CHECK)

.PHONY: build_all
build_all:
	make build_zad3
	make build_cons
	make build_prod
	make build_check

.PHONY: test_all
test_all:
	$(OUTPUT) 3 1 10 	$(TEMP)"file%d.txt"		 >  $(REPORT)
	$(OUTPUT) 1 3 10 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 3 10 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 1 100 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 1 3 100 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 3 100 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 1 1000 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 1 3 1000 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 3 1000 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 1 1225100 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 1 3 1225100 	$(TEMP)"file%d.txt"		 >> $(REPORT)
	$(OUTPUT) 3 3 1225100 	$(TEMP)"file%d.txt"		 >> $(REPORT)

.PHONY: all
all:
	make build_all
	make test_all

.PHONY: test
test:
	make all

.PHONY: clean
clean:
	rm *.o $(OUTPUT).o $(CONS).o $(PROD).o $(CHECK).o $(OUTPUT) $(CONS) $(PROD) $(CHECK) $(OUTCONSUMER) fifo
