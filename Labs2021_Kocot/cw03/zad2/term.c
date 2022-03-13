#include "merger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEMPFILE "temp.txt"
#define REPORTFILE "generated_raport.txt"
#define PATH_MAX 4096 

int main(int argc, char * argv[]) {
	int pairs=0;
	char cbuf[2*PATH_MAX + 2];
	char *pair, *path1, *path2, *pch;
	FILE *fa, *fb;
	FILE *ftemp;
	FILE *freport = fopen(REPORTFILE, "a");
	char ***blox;
	clock_t s1, e1;  // clock start and end
	struct tms s2, e2;  // clock start and end
	double treal, tuser, tsys, treal2;
	struct timespec s3, e3;
	int repleft=0, repn;
	int lastremoved = -1;
	pid_t pid;
	
	for(int n=1; n<argc; n++) {
		if(strcmp(argv[n], "create_table") == 0) {  // create_table [number of pairs]
			pairs = atoi(argv[++n]);
			blox = create_table(pairs);
			printf("<table created>\n");
		}
		else if(strcmp(argv[n], "merge_files") == 0) {  // merge_files [file1]:[file2] [file3]:[file4] ...
			char *pair;
			int posn = n;
			for(int i=0; i<pairs; i++) {
				pair = argv[++n];
				if(strcmp(pair, "repeat_merge") == 0) {  // "repeat_merge" is necessary only when not enough files are given.
					n = posn;
					pair = argv[++n];
				}
				pid = vfork();
				if(pid == 0) {
					strcpy(cbuf, pair);
					pch = strtok(cbuf, ":");
					path1 = pch;
					pch = strtok(NULL, ":");
					path2 = pch;
					//printf("<#%d: merging %s and %s>\n", i, path1, path2);
					FILE *fa = fopen(path1, "r");
					FILE *fb = fopen(path2, "r");
					ftemp = fopen(TEMPFILE, "w+");
					merge_two(fa, fb, ftemp);
					create_block(ftemp, blox, i);
					fclose(fa);
					fclose(fb);
					fclose(ftemp);
					exit(0);
				}
				//wait(NULL);
			}
			/*for(int i=0; i<pairs; i++) {
				wait(NULL);
			}*/
			printf("<merged>\n");
		}
		else if(strcmp(argv[n], "print") == 0) {  // print
			print(blox, pairs);
		}
		else if(strcmp(argv[n], "remove_block") == 0) {  // remove_block [block id]
			int id = atoi(argv[++n]);
			remove_block(blox, id);
			printf("<block removed>\n");
		}
		else if(strcmp(argv[n], "remove_next") == 0) {  // remove_next - used for tests
			remove_block(blox, ++lastremoved);
			printf("<block #%d removed>\n", lastremoved);
		}
		else if(strcmp(argv[n], "remove_row") == 0) {  // remove_line [block id] [line id]
			int id = atoi(argv[++n]);
			int line = atoi(argv[++n]);
			remove_line(blox, id, line);
			printf("<row removed>\n");
		}
		else if(strcmp(argv[n], "tstart") == 0) {  // tstart - starts the timer
			s1 = times(&s2);
			clock_gettime(CLOCK_REALTIME, &s3);
		}
		else if(strcmp(argv[n], "tstop") == 0) {  // tstop - stops the timer and saves the results
			e1 = times(&e2);
			clock_gettime(CLOCK_REALTIME, &e3);
			treal2 = (e3.tv_nsec - s3.tv_nsec) / 1e9;
			treal = ((double) (e1 - s1)) / 100;  // READ INFO BELOW
			tuser = ((double) (e2.tms_utime - s2.tms_utime)) / 100;  // READ INFO BELOW
			tsys  = ((double) (e2.tms_stime - s2.tms_stime)) / 100;  // READ INFO BELOW
			/* Generally, the CLOCKS_PER_SEC constant would be used as a divisor here. However,
			testing the code with Linux-builtin method "time" it has been shown, that times divided
			by CLOCKS_PER_SEC were off 10000 times, thus implying need to use "CLOCKS_PER_SEC/1e4",
			which according to clock's man page ("CLOCKS_PER_SEC equals 1000000 independent of the
			actual resolution.") is always equal 100.*/
			for(int r=1; r<argc; r++) {
				fprintf(freport, "%s ", argv[r]);
			}
			printf("\n");
			printf("Real time: %.2f sec\n", treal);
			printf("User time: %.2f sec\n", tuser);
			printf("System time: %.2f sec\n\n", tsys);
			fprintf(freport, "\n");
			fprintf(freport, "Real time: %.2f sec\n", treal);
			fprintf(freport, "User time: %.2f sec\n", tuser);
			fprintf(freport, "System time: %.2f sec\n\n", tsys);
		}
		else if(strcmp(argv[n], "repeat") == 0) {  // repeat [number of repetitions]
			repleft = atoi(argv[++n]) - 1;
			repn = n;
		}
		else if(strcmp(argv[n], "end_repeat") == 0) {  // end_repeat - closes the repeated block of parameters (multiple repeats cannot be nested)
			if(repleft > 0) {
				repleft--;
				n = repn;
			}
		}
	}
	if(pairs > 0) {
		remove_all(blox, pairs);
		free(blox);
		printf("<memory freed>\n");
	}
	fclose(freport);
}
