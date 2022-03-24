#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <dlfcn.h>

char*** (*create_table)(int table_size);
int (*set_merge_sequence)(char** filenames, int size);
void (*remove_main_table_entry)(char*** main_table, int index);
void (*remove_row)(char*** main_table, int block_index, int row_index);
int (*get_merged_block_size)(int main_table_index);
int (*merge)(char*** main_table, int sequence_index);
void (*print_merged_files)(char*** main_table);
int (*save_merge_in_memory)(char*** main_table);

#define WITH_TIME_MEASURED(operation, start_time_variable_name, start_buffer_name)    \
    struct tms start_buffer_name;                                                     \
    clock_t start_time_variable_name = start_time_measurement(&start_buffer_name);    \
    operation;                                                                        \
    end_time_measurment(&start_buffer_name, start_time_variable_name);


clock_t start_time_measurement(struct tms* start_buffer) {
    clock_t start_time = times(start_buffer);
    return start_time;
}

void end_time_measurment(struct tms* start_buffer, clock_t start_time) {
    struct tms end_buffer;
    clock_t end_time = times(&end_buffer);
    printf("User: %jd\n", end_buffer.tms_utime - start_buffer->tms_utime);
    printf("System: %jd\n", end_buffer.tms_stime - start_buffer->tms_stime);
    printf("Real: %jd\n\n", end_time - start_time);
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
                printf("merging files pair %s time:\n\n", merge_sequence[merge_sequence_size - 1]);
                WITH_TIME_MEASURED(merge(main_table, merge_sequence_size - 1), merge_start_time, merge_start_buffer);
                printf("saving %s merge in memory time:\n\n", merge_sequence[merge_sequence_size - 1]);
                WITH_TIME_MEASURED(save_merge_in_memory(main_table), memory_save_start_time, save_start_buffer);
            }
        }
        if (strcmp(args[i], "remove_block") == 0) {
            i++;
            int block_index = atoi(args[i]);
            printf("removing block %d time:\n\n", block_index);
            WITH_TIME_MEASURED(remove_main_table_entry(main_table, block_index), start_time, start_buffer);
        }
        if (strcmp(args[i], "remove_row") == 0) {
            i++;
            int block_index = atoi(args[i]);
            i++;
            int row_index = atoi(args[i]);
            printf("removing row %d %d time:\n\n", block_index, row_index);
            WITH_TIME_MEASURED(remove_row(main_table, block_index, row_index), start_time, start_buffer);
        }
    }
    return main_table;
}

int main(int argc, char** args) {
    void *library_handle = dlopen("../zad1/libmerge_shared.so", RTLD_LAZY);

    if (library_handle == NULL) {
        printf("error openening library");
        exit(1);
    }

    create_table = dlsym(library_handle, "create_table");
    set_merge_sequence = dlsym(library_handle, "set_merge_sequence");
    remove_main_table_entry = dlsym(library_handle, "remove_main_table_entry");
    remove_row = dlsym(library_handle, "remove_row");
    get_merged_block_size = dlsym(library_handle, "get_merged_block_size");
    merge = dlsym(library_handle, "merge");
    print_merged_files = dlsym(library_handle, "print_merged_files");
    save_merge_in_memory = dlsym(library_handle, "save_merge_in_memory");

    WITH_TIME_MEASURED(
        char*** main_table = parse_arguments(argc, args); 
        printf("All operations time:\n\n"), 
        start_time,
        start_buffer
    );
    // print_merged_files(main_table);
    dlclose(library_handle);
    free(main_table);
}
