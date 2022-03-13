timer = pomiar_zad_3.txt

time_all:
	make build
	make clear_file
	make otime
	make ftime

build: open_numbers.c fopen_numbers.c
	gcc open_numbers.c -o open_numbers -lm -Wall
	gcc fopen_numbers.c -o fopen_numbers -lm -Wall

clear_file:
	> $(timer)

otime:
	echo "OPEN():" >> $(timer)
	(time -p ./open_numbers) 2>> $(timer)
	echo >> $(timer)

ftime:
	echo "FOPEN():" >> $(timer)
	(time -p ./fopen_numbers) 2>> $(timer)
	echo >> $(timer)

	
