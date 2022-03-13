MAIN = ./main
OPTIMAL = -O0

main_test:
	make static_build
	make createall

merger_term: merger.c
	gcc -c merger.c
	gcc -c term.c
merger_shared:
	gcc -fPIC -c merger.c
	gcc -o libmerger.so merger.o -fPIC -shared

# build with static library
static_build:
	make merger_term
	gcc term.o merger.o -o $(MAIN) $(OPTIMAL) 

# build with shared library
shared_build:
	make merger_shared
	gcc term.c -L. -Wl,-rpath=. -lmerger -o $(MAIN) $(OPTIMAL)

# build with dynamic library
dynamic_build:
	make merger_shared
	gcc -o $(MAIN) $(OPTIMAL) -fPIC -L. -Wl,-rpath=. dynamic.c -ldl -D DYNAMIC

# fulltest - test of different ways to link libraries
fulltest:
	make static_build
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	make shared_build
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	make dynamic_build
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	
# test_optimal - test of different optimalisations
test_optimal:
	make merger_term
	gcc term.o merger.o -o $(MAIN) -O
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	gcc term.o merger.o -o $(MAIN) -O0
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	gcc term.o merger.o -o $(MAIN) -O1
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	gcc term.o merger.o -o $(MAIN) -O2
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	gcc term.o merger.o -o $(MAIN) -O3 
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	gcc term.o merger.o -o $(MAIN) -Os 
	$(MAIN) tstart create_table 1000 merge_files large1.txt:large2.txt repeat_merge repeat 1000 remove_next end_repeat tstop
	
create5mini:
	$(MAIN) create_table 5 tstart merge_files mini1.txt:mini2.txt repeat_merge tstop
create50mini:
	$(MAIN) create_table 50 tstart merge_files mini1.txt:mini2.txt repeat_merge tstop
create500mini:
	$(MAIN) create_table 500 tstart merge_files mini1.txt:mini2.txt repeat_merge tstop

create5small:
	$(MAIN) create_table 5 tstart merge_files small1.txt:small2.txt repeat_merge tstop
create50small:
	$(MAIN) create_table 50 tstart merge_files small1.txt:small2.txt repeat_merge tstop
create500small:
	$(MAIN) create_table 500 tstart merge_files small1.txt:small2.txt repeat_merge tstop
	
create5medium:
	$(MAIN) create_table 5 tstart merge_files medium1.txt:medium2.txt repeat_merge tstop
create50medium:
	$(MAIN) create_table 50 tstart merge_files medium1.txt:medium2.txt repeat_merge tstop
create500medium:
	$(MAIN) create_table 500 tstart merge_files medium1.txt:medium2.txt repeat_merge tstop
	
create5big:
	$(MAIN) create_table 5 tstart merge_files big1.txt:big2.txt repeat_merge tstop
create50big:
	$(MAIN) create_table 50 tstart merge_files big1.txt:big2.txt repeat_merge tstop
create500big:
	$(MAIN) create_table 500 tstart merge_files big1.txt:big2.txt repeat_merge tstop
	
create5large:
	$(MAIN) create_table 5 tstart merge_files large1.txt:large2.txt repeat_merge tstop
create50large:
	$(MAIN) create_table 50 tstart merge_files large1.txt:large2.txt repeat_merge tstop
create500large:
	$(MAIN) create_table 500 tstart merge_files large1.txt:large2.txt repeat_merge tstop
	
createall:
	make create5mini
	make create50mini
	make create500mini
	make create5small
	make create50small
	make create500small
	make create5medium
	make create50medium
	make create500medium
	make create5big
	make create50big
	make create500big
	make create5large
	make create50large
	make create500large
	
remove_mini:
	$(MAIN) create_table 110 merge_files mini1.txt:mini2.txt repeat_merge tstart repeat 100 remove_next end_repeat tstop
remove_small:
	$(MAIN) create_table 110 merge_files small1.txt:small2.txt repeat_merge tstart repeat 100 remove_next end_repeat tstop
remove_medium:
	$(MAIN) create_table 110 merge_files medium1.txt:medium2.txt repeat_merge tstart repeat 100 remove_next end_repeat tstop
remove_big:
	$(MAIN) create_table 110 merge_files big1.txt:big2.txt repeat_merge tstart repeat 100 remove_next end_repeat tstop
remove_large:
	$(MAIN) create_table 110 merge_files large1.txt:large2.txt repeat_merge tstart repeat 100 remove_next end_repeat tstop
	
removeall:
	make remove_mini
	make remove_small
	make remove_medium
	make remove_big
	make remove_large

testall:
	make createall
	make removeall
	
