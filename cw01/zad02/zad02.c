//
// Created by mrrys00 on 3/13/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

#include "../zad01/counter.h"
#define REPORT2 "report2.txt"

// https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-times-get-process-child-process-times
clock_t times(struct tms *buffer);

void start_time_measure(clock_t *s1, struct tms *s2) {
    *s1 = times(s2);
}

void finish_time_measure(clock_t s1, struct tms s2, char* operation_name) {
    FILE *fp;
    // https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
    if( access( REPORT2, F_OK ) == 0 ) {
        fp = fopen(REPORT2, "a");
    } else {
        fp = fopen(REPORT2, "w");
    }
    char *command = (char*)malloc(200 * sizeof(char));
    int tics_per_second = sysconf(_SC_CLK_TCK);
    clock_t e1;
    struct tms e2;
    double treal, tuser, tsys;

    e1 = times(&e2);
    treal = ((double) (e1 - s1)) / tics_per_second;
    tuser = ((double) (e2.tms_utime - s2.tms_utime)) / tics_per_second;
    tsys  = ((double) (e2.tms_stime - s2.tms_stime)) / tics_per_second;

    sprintf(command,"User time: %f\nSystem time: %f\nReal time: %f\n\n", tuser, tsys, treal);
    fprintf(fp, "Operation %s execution times:\n", operation_name);
    fprintf(fp, command);

    fclose(fp);
}

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
    return strcmp(arg, SCREATETABLE) == 0 ? 1 : 0;
}

int is_wc_lines(char* arg) {
    return strcmp(arg, SWCFILES) == 0 ? 1 : 0;
}

int is_create_block(char* arg) {
    return strcmp(arg, SCREATEBLOCK) == 0 ? 1 : 0;
}

int is_remove_block(char* arg) {
    return strcmp(arg, SREMOVEBLOCK) == 0 ? 1 : 0;
}

int is_clean_all(char* arg) {
    return strcmp(arg, SCLEANALL) == 0 ? 1 : 0;
}

int main(int argc, char ** args) {
    printf("argc %d\n", argc);
    void ** arr = NULL;
    int size = 0, used_size = 0;
    char all_operation_name[50];
    int every_log = 0;              // change flag to catch all times

    clock_t s1, fs1;
    struct tms s2, fs2;

    for (int i = 1; i < argc; i++) {
        if (i+1 < argc && is_create_table(args[i])) {     // create table
            i++;
            size = atoi(args[i]);
            if (every_log) start_time_measure(&s1, &s2);
            arr = create_table(size);
            if (every_log) finish_time_measure(s1, s2, SCREATETABLE);
            printf("table created\n");
        } else if (i+1 < argc && is_wc_lines(args[i])) {
            while (i+1 < argc && is_filename(args[i+1])) {
                i++;
                if (every_log) start_time_measure(&s1, &s2);
                wc_files(args[i]);
                if (every_log) finish_time_measure(s1, s2, SWCFILES);
                printf("file %s saved to tmp file\n", args[i]);
            }
        } else if (i+1 < argc && is_create_block(args[i])) {
            if (every_log) start_time_measure(&s1, &s2);
            size = create_block(arr, used_size);
            if (every_log) finish_time_measure(s1, s2, SCREATEBLOCK);
            printf("block created\n");
        } else if (i+1 < argc && is_remove_block(args[i])) {
            i++;
            if (every_log) start_time_measure(&s1, &s2);
            remove_block(arr, atoi(args[i]));
            if (every_log) finish_time_measure(s1, s2, SREMOVEBLOCK);
            printf("block %s removed\n", args[i]);
        } else if (is_clean_all(args[i])) {
            if (every_log) start_time_measure(&s1, &s2);
            clean_all(arr, size);
            if (every_log) finish_time_measure(s1, s2, SCLEANALL);
            printf("all cleaned\n");
        } else if (i+1 < argc && strcmp(args[i], SOPERNAME) == 0) {
            (void)strncpy(all_operation_name, args[i+1], sizeof(all_operation_name));
            printf("get operation name\n");
        } else if (strcmp(args[i], "ts") == 0) {
            start_time_measure(&fs1, &fs2);
        } else if (strcmp(args[i], "tf") == 0) {
            finish_time_measure(fs1, fs2, all_operation_name);
        } else if (strcmp(args[i], "every_log") == 0) {
            if (every_log == 0) every_log = 1;
            else every_log = 0;
        }
    }
    return 0;
}
