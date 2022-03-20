CC = gcc -Wall -std=gnu11 -ggdb
RUNARGS = 4 1 10 "./test_texts/producer%d_read.txt"

run: main producer consumer test_program
	@./main.o ${RUNARGS}

main:
	@$(CC) main.c -o main.o

producer:
	@$(CC) producer.c -o producer.o

consumer:
	@$(CC) consumer.c -o consumer.o

test_program:
	@$(CC) test.c -o test.o

test: main producer consumer test_program
	@./main.o 5 1 10 "./test_texts/producer%d_read.txt"         # 5 producers, 1 consumer, N = 10
	@./main.o 1 3 10 "./test_texts/producer%d_read.txt"         # 1 producer, 3 consumers, N = 10
	@./main.o 5 3 10 "./test_texts/producer%d_read.txt"         # 5 producers, 3 consumers, N = 10
	@./main.o 5 1 20 "./test_texts/producer%d_read.txt"         # 5 producers, 1 consumer, N = 20
	@./main.o 1 3 20 "./test_texts/producer%d_read.txt"         # 1 producer, 3 consumers, N = 20
	@./main.o 5 3 20 "./test_texts/producer%d_read.txt"         # 5 producers, 3 consumers, N = 20
	@./main.o 5 1 4500 "./test_texts/producer%d_read_large.txt" # 5 producers, 1 consumer, N = 4500
	@./main.o 1 3 4500 "./test_texts/producer%d_read_large.txt" # 1 producer, 3 consumers, N = 4500
	@./main.o 5 3 4500 "./test_texts/producer%d_read_large.txt" # 5 producers, 3 consumers, N = 4500
	

clean:
	rm *.o consumer_write.txt
