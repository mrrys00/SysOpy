#include <stdio.h>

#ifndef merger
#define merger

	extern char*** create_table(int size);
	extern void merge_two(FILE *fin1, FILE *fin2, FILE *fout);
	extern int create_block(FILE *fp, char ***main, int id);
	extern int block_size(char ***main, int id);
	extern void remove_line(char ***main, int id, int line);
	extern void remove_block(char ***main, int id);
	extern void print(char ***main, int size);

#endif
