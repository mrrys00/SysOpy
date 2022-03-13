test:
	make build
	./test3b buffer.pipe 5
	bash check.sh

build:
	gcc -o producer producer.c -Wall
	gcc -o consumer consumer.c -Wall
	gcc -o test3b test3b.c -Wall

clean:
	rm -f consumer
	rm -f producer
	rm -f shelf.txt
	rm -f test3b
	rm -f .checker_temp
