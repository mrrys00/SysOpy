timer = pomiar_zad_4.txt

time_all:
	make build
	make clear_file
	make otime
	make ftime

build: open_replace.c fopen_replace.c
	gcc open_replace.c -o open_replace -lm -Wall
	gcc fopen_replace.c -o fopen_replace -lm -Wall

clear_file:
	> $(timer)

otime:
	echo "OPEN():" >> $(timer)
	(time -p ./open_replace large1.txt out.txt sit SIIT) 2>> $(timer)
	echo >> $(timer)

ftime:
	echo "FOPEN():" >> $(timer)
	(time -p ./fopen_replace large1.txt out.txt sit SIIT) 2>> $(timer)
	echo >> $(timer)

	
