CC = gcc -Wall -ggdb
CATCHER_PID = 0
SENDING_MODE = SIGRT
SIGNALS_COUNT = 10000


run_catcher:
	@$(CC) catcher.c -o catcher.o
	@./catcher.o ${SENDING_MODE}

run_sender:
	@$(CC) sender.c -o sender.o
	@./sender.o ${CATCHER_PID} $(SIGNALS_COUNT) ${SENDING_MODE}

all: run

clean:
	rm *.o
