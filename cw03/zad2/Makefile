GCCF = gcc -Wall -ggdb -O0
OUTPUT = ./zad2
REPORT = report.txt

build:
	$(GCCF) -c zad2.c
	$(GCCF) zad2.o -o $(OUTPUT)

run:
	echo "##################### Riemman Integration report #####################" > $(REPORT)
	$(OUTPUT) 0.1 16		>> $(REPORT)
	$(OUTPUT) 0.1 128		>> $(REPORT)
	$(OUTPUT) 0.1 1024		>> $(REPORT)
	$(OUTPUT) 0.1 8192		>> $(REPORT)

	$(OUTPUT) 0.01 16		>> $(REPORT)
	$(OUTPUT) 0.01 128		>> $(REPORT)
	$(OUTPUT) 0.01 1024		>> $(REPORT)
	$(OUTPUT) 0.01 8192		>> $(REPORT)

	$(OUTPUT) 0.001 16		>> $(REPORT)
	$(OUTPUT) 0.001 128		>> $(REPORT)
	$(OUTPUT) 0.001 1024	>> $(REPORT)
	$(OUTPUT) 0.001 8192	>> $(REPORT)

	$(OUTPUT) 0.0001 16		>> $(REPORT)
	$(OUTPUT) 0.0001 128	>> $(REPORT)
	$(OUTPUT) 0.0001 1024	>> $(REPORT)
	$(OUTPUT) 0.0001 8192	>> $(REPORT)

	$(OUTPUT) 0.00001 16	>> $(REPORT)
	$(OUTPUT) 0.00001 128	>> $(REPORT)
	$(OUTPUT) 0.00001 1024	>> $(REPORT)
	$(OUTPUT) 0.00001 8192	>> $(REPORT)

.PHONY: all
all:
	make build
	# $(OUTPUT) 0.001 10
	make run

.PHONY: clean
clean:
	rm -f *.o $(OUTPUT)
