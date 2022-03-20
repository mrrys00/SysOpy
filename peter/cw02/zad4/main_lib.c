#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

#define TIME_MEASUREMENTS_FILENAME "pomiar_zad_4lib.txt"
#define HANDLE_ERROR(expression, error_value, message_template_str, ...)           \
    if (expression == error_value) {                                               \
        printf(message_template_str, __VA_ARGS__);                                 \
        exit(1);                                                                   \
    }

#define WITH_TIME_MEASURED(operation, results_file) {                                             \
    struct tms WITH_TIME_MEASURED_tms_buffer;                                                     \
    clock_t WITH_TIME_MEASURED_start = times(&WITH_TIME_MEASURED_tms_buffer);                     \
    operation;                                                                                    \
    end_time_measurment(&WITH_TIME_MEASURED_tms_buffer, WITH_TIME_MEASURED_start, results_file);  \
}                                                                                                 \

void end_time_measurment(struct tms* start_buffer, clock_t start_time, FILE* results_file) {
    struct tms end_buffer;
    clock_t end_time = times(&end_buffer);
    clock_t user_time = end_buffer.tms_utime - start_buffer->tms_utime;
    clock_t sys_time = end_buffer.tms_stime - start_buffer->tms_stime;
    clock_t real_time = end_time - start_time;
    char results_file_content[1000];
    sprintf(results_file_content, "User: %jd\nSystem: %jd\nReal: %jd\n\n", user_time, sys_time, real_time);
    fwrite(results_file_content, sizeof(char), strlen(results_file_content), results_file);
}

char* get_line(char* buffer, FILE* file) {
    int buffer_position = 1;
    if (!fread(buffer, 1, 1, file)) {
        return NULL;
    }    
    while (buffer[buffer_position - 1] != '\n') {
        if (!fread(buffer + buffer_position, 1, 1, file)) {
            buffer[buffer_position] = '\n';
            buffer[buffer_position + 1] = '\0';
            return buffer;
        };
        buffer_position++;
    }
    buffer[buffer_position] = '\0';
    return buffer;
}

void replace_string(FILE* read_file, FILE* write_file, char* replaced, char* replace_to) {
    char buffer[257];
    char replacing_buffer[1000];
    int pattern_length = strlen(replaced);
    int replacing_word_length = strlen(replace_to);
    while (get_line(buffer, read_file) != NULL) {
        char* replacing_buffer_positon = replacing_buffer;
        char* buffer_position = buffer;
        char* pattern_occurence = strstr(buffer_position, replaced);
        while (pattern_occurence != NULL) {
            int moved_fragment_length = pattern_occurence - buffer_position;
            strncpy(replacing_buffer_positon, buffer_position, moved_fragment_length);
            replacing_buffer_positon += moved_fragment_length;
            strncpy(replacing_buffer_positon, replace_to, replacing_word_length);
            replacing_buffer_positon += replacing_word_length;
            buffer_position += moved_fragment_length + pattern_length;
            pattern_occurence = strstr(buffer_position, replaced);
        };
        strcpy(replacing_buffer_positon, buffer_position);
        fwrite(replacing_buffer, sizeof(char), strlen(replacing_buffer), write_file);
    }
}

int main(int argc, char** argv) {
    char* replaced;
    char* replaced_to;
    char* read_filename;
    char* write_filename;
    if (argc < 5) {
        // ask for arguments if they were not passed thorugh terminal
        char buffer[1000];
        if (argc == 1) {
            printf("read filename:\n");
            scanf("%s", (char *) &buffer);
            read_filename = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(read_filename, buffer);
        } else {
            read_filename = argv[1];
        }
        if (argc < 3) {
            printf("write filename:\n");
            scanf("%s", (char *) &buffer);
            write_filename = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(write_filename, buffer);
        } else {
            write_filename = argv[2];
        }
        if (argc < 4) {
            printf("first word:\n");
            scanf("%s", (char *) &buffer);
            replaced = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(replaced, buffer);
        } else {
            replaced = argv[3];
        }
        printf("second word:\n");
        scanf("%s", (char *) &buffer);
        replaced_to = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(replaced_to, buffer);
    } else {
        read_filename = argv[1];
        write_filename = argv[2];
        replaced = argv[3];
        replaced_to = argv[4];
    }

    FILE* read_file_ptr = fopen(read_filename, "r");
    FILE* write_file_ptr = fopen(write_filename, "w");
    FILE* results_file = fopen(TIME_MEASUREMENTS_FILENAME, "w");
    HANDLE_ERROR(read_file_ptr, NULL, "failed to open file %s\n", read_filename);
    HANDLE_ERROR(write_file_ptr, NULL, "failed to open file %s\n", write_filename);
    HANDLE_ERROR(results_file, NULL, "failed to open file %s\n", TIME_MEASUREMENTS_FILENAME);
    WITH_TIME_MEASURED(replace_string(read_file_ptr, write_file_ptr, replaced, replaced_to), results_file);
    fclose(read_file_ptr);
    fclose(write_file_ptr);
    fclose(results_file);
}
