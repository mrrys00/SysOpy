#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

#define TIME_MEASUREMENTS_FILENAME "pomiar_zad_1lib.txt"
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
        if (!fread(buffer + buffer_position, sizeof(char), 1, file)) {
            buffer[buffer_position] = '\n';
            buffer[buffer_position + 1] = '\0';
            return buffer;
        };
        buffer_position++;
    }
    buffer[buffer_position] = '\0';
    return buffer;
}

void print_lines(FILE* file_a, FILE* file_b) {
    char buffer[257];
    while (get_line(buffer, file_a) != NULL) {
        printf("%s", buffer);
        if (get_line(buffer, file_b) != NULL) {
            printf("%s", buffer);
        }
    }
    while (get_line(buffer, file_b) != NULL) {
        printf("%s", buffer);
    }
}

int main(int argc, char** argv) {
    char* filename_a;
    char* filename_b;
    if (argc < 3) {
        // ask for file paths if they are not in program args
        char buffer[1000];
        if (argc == 1) {
            printf("first file:\n");
            scanf("%s", (char *) &buffer);
            filename_a = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(filename_a, buffer);
        } else {
            filename_a = argv[1];
        }
        printf("second file:\n");
        scanf("%s", (char *) &buffer);
        filename_b = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(filename_b, buffer);
    } else {
        filename_a = argv[1];
        filename_b = argv[2];
    }

    FILE* file_a = fopen(filename_a, "r");
    FILE* file_b = fopen(filename_b, "r");
    FILE* results_file = fopen(TIME_MEASUREMENTS_FILENAME, "w");
    HANDLE_ERROR(file_a, NULL, "failed to open file %s\n", filename_a);
    HANDLE_ERROR(file_b, NULL, "failed to open file %s\n", filename_b);
    HANDLE_ERROR(results_file, NULL, "failed to open file %s\n", TIME_MEASUREMENTS_FILENAME);

    WITH_TIME_MEASURED(print_lines(file_a, file_b), results_file);
    fclose(file_a);
    fclose(file_b);
    fclose(results_file);
}
