CFLAGS = -Wall -lrt
CC = gcc

sender: sender.c
	$(GCCF) $^ -o $@ $(CFLAGS)

receiver: receiver.c
	$(GCCF) $^ -o $@ $(CFLAGS)

clean:
	rm -f sender.o sender receiver.o receiver

test:  sender receiver 
	@echo -e "\nTESTING\n"
	./receiver &
	./sender 22 
	./receiver &
	./sender 9 
	./receiver &
	./sender -13 
	./receiver &
	./sender cos 
all: sender receiver
