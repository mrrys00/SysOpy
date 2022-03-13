timer = pomiar_zad_5.txt

time_all:
	make build
	make clear_file
	make otime
	make ftime

build: open_wrap.c fopen_wrap.c
	gcc open_wrap.c -o open_wrap -Wall
	gcc fopen_wrap.c -o fopen_wrap -Wall

clear_file:
	> $(timer)

otime:
	echo "OPEN():" >> $(timer)
	(time -p ./open_wrap large1.txt out.txt) 2>> $(timer)
	echo >> $(timer)

ftime:
	echo "FOPEN():" >> $(timer)
	(time -p ./fopen_wrap large1.txt out.txt) 2>> $(timer)
	echo >> $(timer)

	
