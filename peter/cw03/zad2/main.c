#include "libmerge.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define HANDLE_ERROR(expression, error_value, ...)           \
    if (expression == error_value) {                         \
        printf(__VA_ARGS__);                                 \
        exit(1);                                             \
    }

#define WITH_TIME_MEASURED(operation) {                                                      \
    struct tms WITH_TIME_MEASURED_tms_buffer;                                                \
    clock_t WITH_TIME_MEASURED_start = times(&WITH_TIME_MEASURED_tms_buffer);                \
    operation;                                                                               \
    end_time_measurment(&WITH_TIME_MEASURED_tms_buffer, WITH_TIME_MEASURED_start);           \
}                                                                                            \

void end_time_measurment(struct tms* start_buffer, clock_t start_time) {
    struct tms end_buffer;
    clock_t end_time = times(&end_buffer);
    clock_t user_time = end_buffer.tms_utime - start_buffer->tms_utime;
    clock_t sys_time = end_buffer.tms_stime - start_buffer->tms_stime;
    clock_t real_time = end_time - start_time;
    printf("User: %jd\nSystem: %jd\nReal: %jd\n\n", user_time, sys_time, real_time);
}

int is_operation_name(char* name) {
    if (
        strcmp(name, "remove_block") != 0
        && strcmp(name, "remove_row") != 0
        && strcmp(name, "merge_files") != 0
        && strcmp(name, "create_table") != 0
    ) {
        return 0;
    }
    return 1;
} 

char*** parse_arguments(int argc, char** args) {
    char*** main_table = NULL;
    char** merge_sequence = NULL;
    int merge_sequence_size = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(args[i], "create_table") == 0) {
            i++;
            int table_size = atoi(args[i]);
            main_table = create_table(table_size);
            merge_sequence = calloc(table_size, sizeof(char*));
        }
        if (strcmp(args[i], "merge_files") == 0) {
            while (i + 1 != argc && !is_operation_name(args[i + 1])) {
                i++;
                merge_sequence[merge_sequence_size] = args[i];
                merge_sequence_size++;
                set_merge_sequence(merge_sequence, merge_sequence_size);
                pid_t pid = fork();
                if (pid == 0) {
                    merge(main_table, merge_sequence_size - 1);
                    exit(0);
                }
            }
        }
    }
    while(wait(NULL) > 0);
    return main_table;
}

int main(int argc, char** args) {
    char*** main_table;
    WITH_TIME_MEASURED(
        main_table = parse_arguments(argc, args); 
        printf("All operations time:\n\n")
    );
    free(main_table);
}

