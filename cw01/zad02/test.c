//
// Created by mrrys00 on 3/13/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../zad01/counter.h"

//#define FILENAME "test.c"
//#define CNTFILE "cnt_file"
//#define AWKUTIL "| awk '{print $1}'"

// 1 - true
int is_filename(char* fn) {
    // https://www.delftstack.com/howto/c/string-contains-in-c/
    char * ret;
    ret = strstr(fn, ".txt");
    if (ret)
        return 1;
    return 0;
}

int is_create_table(char* arg) {
    return strcmp(arg, "create_table") == 0 ? 1 : 0;
}

int is_wc_lines(char* arg) {
    return strcmp(arg, "wc_lines") == 0 ? 1 : 0;
}

int is_remove_block(char* arg) {
    return strcmp(arg, "remove_block") == 0 ? 1 : 0;
}

int is_clean_all(char* arg) {
    return strcmp(arg, "clean_all") == 0 ? 1 : 0;
}

int main(int argc, char ** args) {
    printf("argc %d\n", argc);
    void ** arr = NULL;
    int size = 0;
    for (int i = 1; i < argc; i++) {
//        printf("%s\n",args[i]);
        if (i+1 < argc && is_create_table(args[i])) {     // create table
            i++;
            size = atoi(args[i]);
            arr = create_table(size);
            printf("table created\n");
        } else if (i+1 < argc && is_wc_lines(args[i])) {
            while (i+1 < argc && is_filename(args[i+1])) {
                i++;
                wc_files(args[i]);
                printf("saved to tmp file\n");
            }

        } else if (i+1 < argc && is_remove_block(args[i])) {
            printf("block removed\n");
        } else if (is_clean_all(args[i])) {
//            clean_all(arr, )
            printf("all cleaned\n");
        }

    }
    printf("%s\n", temp_cnt_file);
    free(arr);
    return 0;
}
