#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TEMP_MERGE_FILE_NAME "temp_merge_file.txt"

static char** filenames_sequence = NULL;
static int filenames_sequence_size = 0;
static int table_first_free_index = 0;
static int* block_sizes = NULL;

char*** create_table(int table_size) {
    block_sizes = calloc(table_size, sizeof(int));
    return calloc(table_size, sizeof(char**));
}

static int check_filenames_pair_format(char* files_pair) {
    char* copy = calloc(strlen(files_pair) + 1, sizeof(char));
    strcpy(copy, files_pair);
    char* filename_a = strtok(copy, ":");
    if (filename_a == NULL) {
        printf("Invalid filenames pair provided");
        return -1;
    }
    char* filename_b = strtok(NULL, ":");
    if (filename_b == NULL) {
        printf("Invalid filenames pair provided");
        return -1;
    }
    return 1;
}

int set_merge_sequence(char** filenames, int size) {
    filenames_sequence = filenames;
    filenames_sequence_size = size;
    for (int i = 0; i < size; i++) {
        if (check_filenames_pair_format(filenames[i]) == -1) {
            return -1;
        }
    }
    return 1;
}

void remove_main_table_entry(char*** main_table, int index) {
    if (index >= table_first_free_index) {
        return;
    }
    for (int row_index = 0; row_index < block_sizes[index]; row_index++) {
        free(main_table[index][row_index]);
    }
    free(main_table[index]);
    while (index < table_first_free_index - 1) {
        main_table[index] = main_table[index + 1];
        index++;
    }
    table_first_free_index--;
    main_table[table_first_free_index] = NULL;
    block_sizes[table_first_free_index] = 0;
}

void remove_row(char*** main_table, int block_index, int row_index) {
    char** rows = main_table[block_index];
    free(rows[row_index]);
    while (row_index < block_sizes[block_index] - 1) {
        rows[row_index] = rows[row_index + 1];
        row_index++;
    }
    block_sizes[block_index]--;
    rows[block_sizes[block_index]] = NULL;
}

static int merge_files(char* filename_a, char* filename_b) {
    FILE* file_a = fopen(filename_a, "r");
    FILE* file_b = fopen(filename_b, "r");
    FILE* temp_merge_file = fopen(TEMP_MERGE_FILE_NAME, "w");

    int created_file_size_counter = 0;

    if (file_a == NULL || file_b == NULL || temp_merge_file == NULL) {
        printf("Failed to open one of files: %s, %s, %s", filename_a, filename_b, TEMP_MERGE_FILE_NAME);
        return -1;
    }

    char file_a_line[1000];
    char file_b_line[1000];
    while (fgets(file_a_line, sizeof(file_a_line), file_a) != NULL) {
        fputs(file_a_line, temp_merge_file);
        created_file_size_counter++;
        if (fgets(file_b_line, sizeof(file_b_line), file_b) != NULL) {
            fputs(file_b_line, temp_merge_file);
            created_file_size_counter++;
        }
    }
    while (fgets(file_b_line, sizeof(file_b_line), file_b) != NULL) {
        fputs(file_b_line, temp_merge_file);
        created_file_size_counter++;
    }
    fclose(file_a);
    fclose(file_b);
    fclose(temp_merge_file);
    return created_file_size_counter;
}

int get_merged_block_size(int main_table_index) {
    return block_sizes[main_table_index];
}

int merge(char*** main_table, int sequence_index) {
    if (filenames_sequence == NULL || sequence_index >= filenames_sequence_size) {
        printf("merge sequence not defined or index out of range\n");
        return -1;
    }
    char* files_pair = calloc(strlen(filenames_sequence[sequence_index]) + 1, sizeof(char));
    strcpy(files_pair, filenames_sequence[sequence_index]);
    if (check_filenames_pair_format(files_pair) == -1) {
        return -1;
    }
    char* filename_a = strtok(files_pair, ":");
    char* filename_b = strtok(NULL, ":");
    int merged_file_size = merge_files(filename_a, filename_b);
    if (merged_file_size == -1) {
        return -1;
    }
    block_sizes[table_first_free_index] = merged_file_size;
    return 1;
}

int save_merge_in_memory(char*** main_table) {
    FILE* merged_file = fopen(TEMP_MERGE_FILE_NAME, "r");
    main_table[table_first_free_index] = calloc(block_sizes[table_first_free_index], sizeof(char*));
    char merged_file_line[1000];
    int row_counter = 0;
    while (fgets(merged_file_line, sizeof(merged_file_line), merged_file) != NULL) {
        char * row = calloc(strlen(merged_file_line) + 1, sizeof(char));
        strcpy(row, merged_file_line);
        main_table[table_first_free_index][row_counter] = row;
        row_counter++;
    }
    table_first_free_index++;
    return table_first_free_index - 1;
}

void print_merged_files(char*** main_table) {
    printf("\n");
    for (int i = 0; i < table_first_free_index; i++) {
        printf("MERGED FILE NO %d\n", i + 1);
        for (int row_index = 0; row_index < block_sizes[i]; row_index++) {
            printf("%s", main_table[i][row_index]);
        }
        printf("\n");
    }
}
