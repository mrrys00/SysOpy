SENDER = ./sender
CATCHER = ./catcher
SAVE = ./pid_save.bin

build:
	gcc sender.c -o $(SENDER) -Wall
	gcc catcher.c -o $(CATCHER) -Wall

catch1:
	$(CATCHER) KILL
send1:
	$(SENDER) 0 1000 KILL

catch2:
	$(CATCHER) SIGQUEUE
send2:
	$(SENDER) 0 1000 SIGQUEUE

catch3:
	$(CATCHER) SIGRT
send3:
	$(SENDER) 0 1000 SIGRT

clean:
	rm -f $(SENDER)
	rm -f $(CATCHER)
	rm -f $(SAVE)
