MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad3
CONS = ./cons
PROD = ./prod
CHECK = ./check
OUTCONSUMER = out_cons.txt
TEMP = ./temp/

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
	$(OUTPUT) 5 1 10 	$(TEMP)"file%d_read.txt"         	# 5 producers, 1 consumer, N = 10
	$(OUTPUT) 1 3 10 	$(TEMP)"file%d_read.txt"         	# 1 producer, 3 consumers, N = 10
	$(OUTPUT) 5 3 10 	$(TEMP)"file%d_read.txt"         	# 5 producers, 3 consumers, N = 10
	$(OUTPUT) 5 1 20 	$(TEMP)"file%d_read.txt"         	# 5 producers, 1 consumer, N = 20
	$(OUTPUT) 1 3 20 	$(TEMP)"file%d_read.txt"         	# 1 producer, 3 consumers, N = 20
	$(OUTPUT) 5 3 20 	$(TEMP)"file%d_read.txt"         	# 5 producers, 3 consumers, N = 20
	$(OUTPUT) 5 1 4500 	$(TEMP)"file%d_read_large.txt" 		# 5 producers, 1 consumer, N = 4500
	$(OUTPUT) 1 3 4500 	$(TEMP)"file%d_read_large.txt" 		# 1 producer, 3 consumers, N = 4500
	$(OUTPUT) 5 3 4500 	$(TEMP)"file%d_read_large.txt" 		# 5 producers, 3 consumers, N = 4500

.PHONY: all
all:
	make build_all
	make test_all

.PHONY: clean
clean:
	rm *.o $(OUTPUT).o $(CONS).o $(PROD).o $(CHECK).o $(OUTPUT) $(CONS) $(PROD) $(CHECK) $(OUTCONSUMER)
