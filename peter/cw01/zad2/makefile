CC = gcc -Wall -std=c11 -ggdb
TEST_RUN_ARGS = create_table 10 \
	merge_files                                                   \
		test_text_files/small_a.txt:test_text_files/small_b.txt   \
		test_text_files/medium_a.txt:test_text_files/medium_b.txt \
		test_text_files/large_a.txt:test_text_files/large_b.txt   \

test_run:
	@$(CC) main.c -o main_test.o
	@./main_test.o $(TEST_RUN_ARGS)
	@echo "\n"

all: test_run

clean:
	rm main_test.o temp_merge_file.txt
