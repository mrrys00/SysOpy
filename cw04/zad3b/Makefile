MAKEFLAGS += --no-print-directory 
MAKEFLAGS += --ignore-errors

GCCF = gcc -Wall -ggdb
SENDER = ./sender
CATCHER = ./catcher

CATCHER_PID = 0
SENDING_MODE_KILL = KILL
SENDING_MODE_SIGRT = SIGRT
SENDING_MODE_SIGQUEUE = SIGQUEUE
SIGNALS_COUNT = 2048

build:
	$(GCCF) -c $(SENDER).c
	$(GCCF) $(SENDER).o -o $(SENDER)
	$(GCCF) -c $(CATCHER).c
	$(GCCF) $(CATCHER).o -o $(CATCHER)

run_kill_cartcher:
	$(CATCHER) $(SENDING_MODE_KILL)
	echo "sendera z ręki odpalać!!!"
# run_kill_sender:
# 	$(SENDER) $(CATCHER_PID) $(SIGNALS_COUNT) $(SENDING_MODE_KILL)
run_sigrt_catcher:
	$(CATCHER) $(SENDING_MODE_SIGRT)
	echo "sendera z ręki odpalać!!!"
# run_sigrt_sender:
# 	$(SENDER) $(CATCHER_PID) $(SIGNALS_COUNT) $(SENDING_MODE_SIGRT)
run_sigqueue_catcher:
	$(CATCHER) $(SENDING_MODE_SIGQUEUE)
	echo "sendera z ręki odpalać!!!"
# run_sigqueue_sender:
# 	$(SENDER) $(CATCHER_PID) $(SIGNALS_COUNT) $(SENDING_MODE_SIGQUEUE)

.PHONY: clean
clean:
	rm *.o $(SENDER) $(CATCHER)