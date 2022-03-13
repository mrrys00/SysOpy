timer = pomiar_zad_2.txt

time_all:
	make build
	make clear_file
	make otime
	make ftime

build: open_filter.c fopen_filter.c
	gcc open_filter.c -o open_filter -Wall
	gcc fopen_filter.c -o fopen_filter -Wall

clear_file:
	> $(timer)

otime:
	echo "OPEN():" >> $(timer)
	(time -p ./open_filter h large1.txt) 2>> $(timer)
	echo >> $(timer)

ftime:
	echo "FOPEN():" >> $(timer)
	(time -p ./fopen_filter h large1.txt) 2>> $(timer)
	echo >> $(timer)
