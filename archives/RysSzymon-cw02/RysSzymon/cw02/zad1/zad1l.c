//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

// https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-times-get-process-child-process-times
clock_t times(struct tms *buffer);

void start_time_measure(clock_t *s1, struct tms *s2) {
    *s1 = times(s2);
}

void finish_time_measure(clock_t s1, struct tms s2, char* operation_name) {
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
    printf("Operation %s execution times:\n", operation_name);
    printf(command);
}

int main(int argc, char *args[]) {
    char* fn1;
    char* fn2;
    clock_t fs1;
    struct tms fs2;
    if (argc < 3) {
         char buffer[4095];

         scanf("input file name:\n %s", (char *) &buffer);
         fn1 = calloc(strlen(buffer) + 1, sizeof(char));
         strcpy(fn1, buffer);

         scanf("output file name:\n %s", (char *) &buffer);
         fn2 = calloc(strlen(buffer) + 1, sizeof(char));
         strcpy(fn2, buffer);
     } else {
         fn1 = args[1];
         fn2 = args[2];
     }

    start_time_measure(&fs1, &fs2);
    FILE *fp1 = fopen(fn1, "r");
    if (fp1 == NULL) {
        printf("cannot open source file\n");
        exit(1);
    }
    FILE *fp2 = fopen(fn2, "w");
    if (fp2 == NULL) {
        printf("cannot open destination file\n");
        exit(1);
    }
    int cnt = 0, max_line_len = 1500, qualified_to_copy = 0, offset = 0;
    char block[max_line_len];

    while ((cnt = fread(block, 1, max_line_len, fp1)) > 0) {
        int i = 0, char_n_l_i = -1;

        while (i < cnt) {
            if (block[i] == '\n') {
                char_n_l_i = i;
                break;
            } else if (isspace((int) block[i]) == 0) {
                qualified_to_copy = 1;
            }
            i++;
        }

        if (char_n_l_i == -1 && cnt < max_line_len && qualified_to_copy) {
            char_n_l_i = cnt - 1;
        }

        if (char_n_l_i == -1) {
            offset += max_line_len;
        }

        if (char_n_l_i != -1) {
            if (qualified_to_copy) {
                char *new_buffer = calloc(sizeof(char), char_n_l_i + offset +1);
                fseek(fp1, -cnt - offset, SEEK_CUR);
                fread(new_buffer, 1, char_n_l_i + offset + 1, fp1);
                fwrite(new_buffer, 1, char_n_l_i + offset + 1, fp2);
                free(new_buffer);
            } else {
                fseek(fp1, -cnt + char_n_l_i + 1, SEEK_CUR);
            }
            offset = 0; qualified_to_copy = 0;
        }
    }
    fclose(fp1);
    fclose(fp2);
    finish_time_measure(fs1, fs2, "Library");

    return 0;
}
