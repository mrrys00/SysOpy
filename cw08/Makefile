MAKEFLAGS += --no-print-directory

GCCF = gcc -Wall -ggdb -Ofast -pthread
MAIN = ./zad1
IN = "fractal_tree.ascii.pgm"
OUT = "fractal_tree_inv.ascii.pgm"
BLOCKMODE = "block"
NUMBEMODE = "numbers"
REPORT = "times.txt"

.PHONY: build
build:
	$(GCCF) -o $(MAIN) $(MAIN).c 

.PHONY: test
test:
	$(MAIN) 1 $(BLOCKMODE) $(IN) $(OUT)

	# echo -e "mode \t time [µs] \t thread_num \t single_thread_time [µs]" > $(REPORT)
	# $(MAIN) 1 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 2 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 4 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 8 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 16 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 32 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 64 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 128 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 256 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 512 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 1024 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 2048 $(BLOCKMODE) $(IN) $(OUT) >> $(REPORT)

	# $(MAIN) 1 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 2 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 4 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 8 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 16 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 32 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 64 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 128 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 256 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 512 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 1024 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)
	# $(MAIN) 2048 $(NUMBEMODE) $(IN) $(OUT) >> $(REPORT)

.PHONY: show
show:
	eog fractal_tree_inv.ascii.pgm &
	eog fractal_tree.ascii.pgm &

.PHONY: all
all:
	make build
	make test

.PHONY: clean
clean:
	rm -r $(MAIN) $(OUT)
