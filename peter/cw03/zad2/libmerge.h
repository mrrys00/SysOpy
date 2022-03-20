#ifndef __LIB_H__
#define __LIB_H__

char*** create_table(int table_size);

int set_merge_sequence(char** filenames, int size);

void remove_main_table_entry(char*** main_table, int index);

void remove_row(char*** main_table, int block_index, int row_index);

int get_merged_block_size(int main_table_index);

int merge(char*** main_table, int sequence_index);

void print_merged_files(char*** main_table);

int save_merge_in_memory(char*** main_table);

#endif
