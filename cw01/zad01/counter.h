#ifndef counter
#define counter

extern char* SCREATETABLE;
extern char* SWCFILES;
extern char* SCREATEBLOCK;
extern char* SREMOVEBLOCK;
extern char* SCLEANALL;
extern char* SOPERNAME;

void** create_table(int size);
void wc_files(char *fn);
int create_block(void **main_arr, int size);
void remove_block(void **main_arr, int id);
void clean_all(void **main_arr, int size);

#endif
