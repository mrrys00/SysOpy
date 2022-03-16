#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define AWKUTIL "| awk '{print $1}'"
#define TEMPFILENAME "cnt_file.temp"

void** create_table(int size) {
	return calloc(sizeof(void*), size);
}

void wc_files(char *fn) {
    char *command = (char*)malloc(200 * sizeof(char));      // assume that command will be shorter than 200 characters
    // count and dump to tmp file using default bash command wc with proper flags
    sprintf(command, "wc -l %s %s > %s && wc -w %s %s >> %s && wc -m %s %s >> %s", fn, AWKUTIL, TEMPFILENAME, fn, AWKUTIL, TEMPFILENAME, fn, AWKUTIL, TEMPFILENAME);
    system(command);
    return;
}

int create_block(void **main_arr, int arr_size) {
    int fp = open(TEMPFILENAME, O_RDONLY);
    // lseek - find bytes length
    long size = lseek(fp, 0, SEEK_END);
    // return to the beginning
    lseek(fp, 0, SEEK_SET);
    main_arr[arr_size] = calloc(sizeof(char), size + 1);
    read(fp, main_arr[arr_size], size);
    // end of data sign \0
    ((char **)main_arr)[arr_size][size] = '\0';
    close(fp);
    remove(TEMPFILENAME);
    return arr_size+1;
}

void remove_block(void **main_arr, int id) {
    free(main_arr[id]);
    main_arr[id] = NULL;
    return;
}

void clean_all(void **main_arr, int size) {
    for (int i = 0; i < size; i++) {
        remove_block(main_arr, i);
    }
    free(main_arr);
    main_arr = NULL;
    return;
}
