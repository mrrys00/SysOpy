//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <fcntl.h>

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
    char* fn1 = args[1];
    char chr_to_find = args[2][0];
    clock_t fs1;
    struct tms fs2;

    start_time_measure(&fs1, &fs2);
    int fp1 = open(fn1, O_RDONLY);
    if (fp1 == -1) {
        printf("cannot open source file\n");
        exit(1);
    }

    int cnt = 0, max_line_len = 256, lines_with_char_cnt = 0, all_chars = 0;
    char block[max_line_len];

    while ((cnt = read(fp1, block, max_line_len)) > 0) {
        int i = 0, line_char_cnt = 0, char_n_l_i = -1;

        while (i < cnt && char_n_l_i == -1) {
            if (block[i] == '\n') {
                char_n_l_i = i;
            } else if (block[i] == chr_to_find) {
                line_char_cnt++;
            }
            i++;
        }
        lseek(fp1, -cnt + char_n_l_i + 1, SEEK_CUR);

        if (line_char_cnt != 0) {
            lines_with_char_cnt++;
        }
        if (line_char_cnt >= 0){
            all_chars += line_char_cnt;
        }

    }
    close(fp1);
    printf("characters in file: %d\nline with character: %d\n", all_chars, lines_with_char_cnt);
    finish_time_measure(fs1, fs2, "System");

    return 0;
}
