#include "merger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char*** create_table(int size) {
	return (char***) calloc(size, sizeof(char**));
}

void merge_two(FILE *fin1, FILE *fin2, FILE *fout) {
	char c1='\n', c2='\n';
	int state = 1;
	while (c1 != EOF && c2 != EOF) {
		if (state == 1) {
			while ((c1 = fgetc(fin1)) != '\n' && c1 != EOF) {
				fputc(c1, fout);
				//printf("%c", c1);
			}
			fputc('\n', fout);
			//printf("\n");
			state = 2;
		}
		else {
			while ((c2 = fgetc(fin2)) != '\n' && c2 != EOF) {
				fputc(c2, fout);
				//printf("%c", c2);
			}
			fputc('\n', fout);
			//printf("\n");
			state = 1;
		}
	}
	if(c1 == EOF && c2 != EOF) {
		while ((c2 = fgetc(fin2)) != EOF) {
			fputc(c2, fout);
			//printf("%c", c2);
		}
	}
	else if(c1 != EOF && c2 == EOF) {
		while ((c1 = fgetc(fin1)) != EOF) {
			fputc(c1, fout);
			//printf("%c", c1);
		}
	}
}

int create_block(FILE *fp, char ***main, int id) {
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
		main[id][line++] = (char*) calloc(llen+1, sizeof(char));
	}
	rewind(fp);
	c = '\n';
	line = 0;
	while(c != EOF) {
		llen = 0;
		while((c = fgetc(fp)) != '\n' && c != EOF) {
			main[id][line][llen++] = c;
		}
		line++;
	}
	main[id][line] = NULL;  // last pointer is made NULL, to indicate array length
	return id;
}

int block_size(char ***main, int id) {
	int lines = 0;
	if(main[id] != NULL) {
		while(main[id][lines] != NULL) {
			lines++;
		}
	}
	return lines;
}

void remove_line(char ***main, int id, int line) {
	int bsize = block_size(main, id);
	for(int i=line; i < bsize; i++) {
		main[id][i] = main[id][i+1];
	}
	free(main[id][bsize]);
	main[id] = (char**) realloc(main[id], (bsize)*sizeof(char*));
}

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

void print(char ***main, int size) {
	for(int b=0; b<size; b++) {
		if(main[b] != NULL) {
			for(int i=0; main[b][i] != NULL; i++) {
				printf("%s\n", main[b][i]);
			}
			printf("\n");
		}
	}
}
