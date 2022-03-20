#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TIME_MEASUREMENTS_FILENAME "pomiar_zad_2sys.txt"

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
}

void end_time_measurment(struct tms* start_buffer, clock_t start_time, int results_file) {
    struct tms end_buffer;
    clock_t end_time = times(&end_buffer);
    clock_t user_time = end_buffer.tms_utime - start_buffer->tms_utime;
    clock_t sys_time = end_buffer.tms_stime - start_buffer->tms_stime;
    clock_t real_time = end_time - start_time;
    char results_file_content[1000];
    sprintf(results_file_content, "User: %jd\nSystem: %jd\nReal: %jd\n\n", user_time, sys_time, real_time);
    write(results_file, results_file_content, strlen(results_file_content));
}

char* get_line(char* buffer, int file, char character, int* has_character) {
    int buffer_position = 1;
    *has_character = 0;
    if (!read(file, buffer, 1)) {
        return NULL;
    }    
    while (buffer[buffer_position - 1] != '\n') {
        if (!read(file, buffer + buffer_position, 1)) {
            buffer[buffer_position] = '\n';
            buffer[buffer_position + 1] = '\0';
            return buffer;
        };
        if (buffer[buffer_position] == character) {
            *has_character = 1;
        }
        buffer_position++;
    }
    buffer[buffer_position] = '\0';
    return buffer;
}

void print_lines(int file_a, char character) {
    char buffer[257];
    int has_character;
    while (get_line(buffer, file_a, character, &has_character) != NULL) {
        if (has_character) {
            printf("%s", buffer);
        }
    }
}

int main(int argc, char** argv) {
    char* character;
    char* filename;
    if (argc < 3) {
        // ask for arguments if they were not passed thorugh terminal
        char buffer[1000];
        if (argc == 1) {
            printf("character:\n");
            scanf("%c", (char *) &buffer);
            character = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(character, buffer);
        } else {
            character = argv[1];
        }
        printf("second file:\n");
        scanf("%s", (char *) &buffer);
        filename = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(filename, buffer);
    } else {
        character = argv[1];
        filename = argv[2];
    }

    int file_ptr = open(filename, O_RDONLY);
    int results_file = open(TIME_MEASUREMENTS_FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    HANDLE_ERROR(file_ptr, -1, "failed to open file %s\n", filename);
    HANDLE_ERROR(results_file, -1, "failed to open file %s\n", TIME_MEASUREMENTS_FILENAME);
    WITH_TIME_MEASURED(print_lines(file_ptr, *character), results_file);
    close(file_ptr);
    close(results_file);
}
