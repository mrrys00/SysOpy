GCCF = gcc -Wall -ggdb
LIBS = ../zad01/
CODE = ../zad02/
OUTPUT = ./zad03
TIMESTART = ts
TIMEFINIS = tf
REPORT3A = report2.txt
REPORT3B = report2.txt
TESTARGS = create_table 1100 ts wc_files $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt \
			$(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt $(CODE)big.txt tf

static_lib:
	$(GCCF) -c $(LIBS)counter.c
	$(GCCF) -c $(CODE)zad02.c
	ar rcs counter.a counter.o

static_build:
	make static_lib
	$(GCCF) zad02.o counter.a -o $(OUTPUT)

shared_lib:
	$(GCCF) -fPIC -c $(LIBS)counter.c
	$(GCCF) -o libcounter.so counter.o -fPIC -shared

shared_build:
	make shared_lib
	$(GCCF) -c $(CODE)zad02.c
	$(GCCF) zad02.o -L. -Wl,-rpath=. -lcounter -o $(OUTPUT)

dynamic_build:
	make shared_lib
	$(GCCF) dynamic.c -o $(OUTPUT) -ldl

.PHONY: test_libs
test_libs:
	echo "=========== STATIC LIBRARY TEST ===========" > $(REPORT3A)
	make static_build
	$(OUTPUT) $(TESTARGS)
	echo "=========== SHARED LIBRARY TEST ===========" >> $(REPORT3A)
	make shared_build
	$(OUTPUT) $(TESTARGS)
	echo "=========== DYNAMIC LIBRARY TEST ===========" >> $(REPORT3A)
	make dynamic_build
	$(OUTPUT) $(TESTARGS)
	mv "report2.txt" results3a.txt
	make clean

.PHONY: test_optim
test_optim:
	make static_lib
	echo "=========== STATIC LIBRARY TEST O  ===========" > $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -O
	$(OUTPUT) $(TESTARGS)
	echo "=========== STATIC LIBRARY TEST O0 ===========" >> $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -O0
	$(OUTPUT) $(TESTARGS)
	echo "=========== STATIC LIBRARY TEST O1 ===========" >> $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -O1
	$(OUTPUT) $(TESTARGS)
	echo "=========== STATIC LIBRARY TEST O2 ===========" >> $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -O2
	$(OUTPUT) $(TESTARGS)
	echo "=========== STATIC LIBRARY TEST O3 ===========" >> $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -O3
	$(OUTPUT) $(TESTARGS)
	echo "=========== STATIC LIBRARY TEST Os ===========" >> $(REPORT3A)
	$(GCCF) zad02.o counter.a -o $(OUTPUT) -Os
	$(OUTPUT) $(TESTARGS)
	mv "report2.txt" results3b.txt
	make clean

.PHONY: all
all:
	make test_libs
	make test_optim

.PHONY: clean
clean:
	rm -f *.o *.a *.so *.temp $(OUTPUT)
