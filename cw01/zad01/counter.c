#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TEMPCNTFILE "cnt_file"
#define AWKUTIL "| awk '{print $1}'"

char*** create_table(int size) {        // chyba działa
	return (char***) calloc(size, sizeof(char**));
}

void count_all(char *fn) {              // chyba działa
    char *command = (char*)malloc(200 * sizeof(char));
    // count and dump to tmp file using default bash command wc with proper flags
    sprintf(command, "wc -l %s %s > %s && wc -w %s %s >> %s && wc -m %s %s >> %s", fn, AWKUTIL, TEMPCNTFILE, fn, AWKUTIL, TEMPCNTFILE, fn, AWKUTIL, TEMPCNTFILE);
    system(command);
    return;
}

int create_block(FILE *fp, char ***main, int id) {      // chyba NIE działa
	main[id] = (char**) calloc(1, sizeof(char*));
	char c = '\n';
	int line = 0;  // line id inside the block
	int llen;  // present line length
	rewind(fp);
	while(c != EOF) {
		llen = 0;
		while((c = fgetc(fp)) != '\n' && c != EOF) {
			llen++;
		}
		main[id] = (char**) realloc(main[id], (line+2)*sizeof(char*));
//		main[id][line++] = (char*) calloc(llen+1, sizeof(char));
	}
//	rewind(fp);
//	c = '\n';
//	line = 0;
//	while(c != EOF) {
//		llen = 0;
//		while((c = fgetc(fp)) != '\n' && c != EOF) {
//			main[id][line][llen++] = c;
//		}
//		line++;
//	}
//	main[id][line] = NULL;  // last pointer is made NULL, to indicate array length
	return id;
}

int block_size(char ***main, int id) {      // de facto to mi nie potrzebne chyba
	int lines = 0;
	if(main[id] != NULL) {
		while(main[id][lines] != NULL) {
			lines++;
		}
	}
	return lines;
}

//void remove_line(char ***main, int id, int line) {
//	int bsize = block_size(main, id);
//	for(int i=line; i < bsize; i++) {
//		main[id][i] = main[id][i+1];
//	}
//	free(main[id][bsize]);
//	main[id] = (char**) realloc(main[id], (bsize)*sizeof(char*));
//}

void remove_block(char ***main, int id) {
	int bsize = block_size(main, id);
	for(int i=0; i<bsize; i++) {
		free(main[id][i]);
	}
	free(main[id]);
	main[id] = NULL;
}

void remove_all(char ***main, int size) {
	for(int i=0; i<size; i++) {
		remove_block(main, i);
	}
}

//void print(char ***main, int size) {
//	for(int b=0; b<size; b++) {
//		if(main[b] != NULL) {
//			for(int i=0; main[b][i] != NULL; i++) {
//				printf("%s\n", main[b][i]);
//			}
//			printf("\n");
//		}
//	}
//}
