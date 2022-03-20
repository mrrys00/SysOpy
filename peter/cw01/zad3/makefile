RUN_ARGS = create_table 9                                          \
	merge_files                                                    \
		test_text_files/small_a.txt:test_text_files/small_b.txt    \
		test_text_files/small_a.txt:test_text_files/small_b.txt    \
		test_text_files/small_a.txt:test_text_files/small_b.txt
OPTIMIZATION = O0
CC = gcc -$(OPTIMIZATION) -Wall -std=c11 -ggdb 

build_static:
	@$(CC) main.c -o main_static.o  -L../zad1 -lmerge

run_static: build_static
	@./main_static.o $(RUN_ARGS)
	@echo "\n"

build_dynamic:
	@$(CC) main.c -o main_dynamic.o  -L../zad1 -lmerge_shared

run_dynamic: build_dynamic
	@LD_LIBRARY_PATH="../zad1" ./main_dynamic.o $(RUN_ARGS)
	@echo "\n"

build_dynamically_loaded:
	@$(CC) main_dynamic.c -o main_dynamically_loaded.o -ldl

run_dynamically_loaded: build_dynamically_loaded
	@./main_dynamically_loaded.o $(RUN_ARGS)
	@echo "\n"

all: build_static build_dynamic build_dynamically_loaded

clean:
	rm *.o temp_merge_file.txt
