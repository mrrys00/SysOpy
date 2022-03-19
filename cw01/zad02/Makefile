	GCCF = gcc -O0 -Wall -ggdb
LIBS = ../zad01/
OUTPUT = ./zad02
TIMESTART = ts
TIMEFINIS = tf
REPORTNAM = report2.txt

.PHONY: build
build:
	$(GCCF) -c $(LIBS)counter.c
	$(GCCF) -c zad02.c
	$(GCCF) zad02.o counter.o -o $(OUTPUT)

.PHONY: all
all:
	make build
	$(OUTPUT) create_table 1 $(TIMESTART) wc_files small.txt all_operation_name 1small $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_1small $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_1small $(TIMEFINIS)
	$(OUTPUT) create_table 5 $(TIMESTART) wc_files small.txt small.txt small.txt small.txt small.txt all_operation_name 5small $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_5small $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_5small $(TIMEFINIS)
	$(OUTPUT) create_table 10 $(TIMESTART) wc_files small.txt small.txt small.txt small.txt small.txt small.txt small.txt small.txt small.txt small.txt all_operation_name 10small $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_10small $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_10small $(TIMEFINIS)
	$(OUTPUT) create_table 1 $(TIMESTART) wc_files medium.txt all_operation_name 1medium $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_1medium $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_1medium $(TIMEFINIS)
	$(OUTPUT) create_table 5 $(TIMESTART) wc_files medium.txt medium.txt medium.txt medium.txt medium.txt all_operation_name 5medium $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_5medium $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_5medium $(TIMEFINIS)
	$(OUTPUT) create_table 10 $(TIMESTART) wc_files medium.txt medium.txt medium.txt medium.txt medium.txt medium.txt medium.txt medium.txt medium.txt medium.txt all_operation_name 10medium $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_10medium $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_10medium $(TIMEFINIS)
	$(OUTPUT) create_table 1 $(TIMESTART) wc_files big.txt all_operation_name 1big $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_1big $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_1big $(TIMEFINIS)
	$(OUTPUT) create_table 5 $(TIMESTART) wc_files big.txt big.txt big.txt big.txt big.txt all_operation_name 5big $(	TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_5big $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_5big $(TIMEFINIS)
	$(OUTPUT) create_table 10 $(TIMESTART) wc_files big.txt big.txt big.txt big.txt big.txt big.txt big.txt big.txt big.txt big.txt all_operation_name 10big $(TIMEFINIS) $(TIMESTART) create_block all_operation_name create_block_10big $(TIMEFINIS) $(TIMESTART) remove_block 1 all_operation_name remove_block_10big $(TIMEFINIS)

	$(OUTPUT) create_table 5 $(TIMESTART) create_block remove_block 1 create_block create_block remove_block 1 remove_block 2 create_block create_block create_block remove_block 1 remove_block 2 remove_block 3 all_operation_name create_remove_blocks $(TIMEFINIS)

.PHONY: clean
clean:
	rm -f *.o *.a *.so $(OUTPUT) zad02
