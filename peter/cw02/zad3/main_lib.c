#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/times.h>

#define TIME_MEASUREMENTS_FILENAME "pomiar_zad_3lib.txt"
#define HANDLE_ERROR(expression, error_value, ...)           \
    if (expression == error_value) {                         \
        printf(__VA_ARGS__);                                 \
        exit(1);                                             \
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

int main(int argc, char** argv) {
    FILE* file_data = fopen("dane.txt", "r");
    FILE* file_a = fopen("a.txt", "w");
    FILE* file_b = fopen("b.txt", "w");
    FILE* file_c = fopen("c.txt", "w");
    FILE* results_file = fopen(TIME_MEASUREMENTS_FILENAME, "w");
    HANDLE_ERROR(file_data, NULL, "failed to open file dane.txt\n");
    HANDLE_ERROR(file_a, NULL, "failed to open file a.txt\n");
    HANDLE_ERROR(file_b, NULL, "failed to open file b.txt\n");
    HANDLE_ERROR(file_c, NULL, "failed to open file c.txt\n");
    HANDLE_ERROR(results_file, NULL, "failed to open file %s\n", TIME_MEASUREMENTS_FILENAME);
    char buffer[257];
    int even_numbers_count = 0;
    WITH_TIME_MEASURED(
    while (get_line(buffer, file_data) != NULL) {
        int number = atoi(buffer);
        if (number % 2 == 0) {
            even_numbers_count++;
            printf("%d\n", number);
        }
        if ((number / 10) % 10 == 7 || (number / 10) % 10 == 0) {
            char* file_b_content = calloc((int)log10(number) + 1, sizeof(char));
            sprintf(file_b_content, "%d\n", number);
            fwrite(file_b_content, sizeof(char), strlen(file_b_content), file_b);
            free(file_b_content);
        }
        if (sqrt(number) == (int) sqrt(number)) {
            char* file_c_content = calloc((int)log10(number) + 1, sizeof(char));
            sprintf(file_c_content, "%d\n", number);
            fwrite(file_c_content, sizeof(char), strlen(file_c_content), file_c);
            free(file_c_content);
        }
    }
    char* file_a_text = calloc(strlen("Liczb parzystych jest ") + 1 + (int)log10(even_numbers_count), sizeof(char));
    sprintf(file_a_text, "Liczb parzystych jest %d", even_numbers_count);
    fwrite(file_a_text, sizeof(char), strlen(file_a_text), file_a);
    free(file_a_text);
    , results_file);
    fclose(file_data);
    fclose(file_a);
    fclose(file_b);
    fclose(file_c);
    fclose(results_file);
}
