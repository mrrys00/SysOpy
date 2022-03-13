#include <stdio.h>

#ifndef counter
#define counter

	extern char*** create_table(int size);
	extern void count_all(char *fn);
	extern int create_block(FILE *fp, char ***main, int id);
	extern int block_size(char ***main, int id);
//	extern void remove_line(char ***main, int id, int line);
	extern void remove_block(char ***main, int id);
//	extern void print(char ***main, int size);
	extern void remove_all(char ***main, int size);

#endif
