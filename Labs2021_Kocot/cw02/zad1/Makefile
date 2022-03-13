timer = pomiar_zad_1.txt

time_all:
	make build
	make clear_file
	make otime
	make ftime

build: open_merger.c fopen_merger.c
	gcc open_merger.c -o open_merger -Wall
	gcc fopen_merger.c -o fopen_merger -Wall

clear_file:
	> $(timer)

otime:
	echo "OPEN():" >> $(timer)
	(time -p ./open_merger large1.txt large2.txt) 2>> $(timer)
	echo >> $(timer)

ftime:
	echo "FOPEN():" >> $(timer)
	(time -p ./fopen_merger large1.txt large2.txt) 2>> $(timer)
	echo >> $(timer)
