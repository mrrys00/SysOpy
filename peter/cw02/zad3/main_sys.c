#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TIME_MEASUREMENTS_FILENAME "pomiar_zad_3sys.txt"
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

char* get_line(char* buffer, int file) {
    int buffer_position = 1;
    if (!read(file, buffer, 1)) {
        return NULL;
    }    
    while (buffer[buffer_position - 1] != '\n') {
        if (!read(file, buffer + buffer_position, 1)) {
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
    int file_data = open("dane.txt", O_RDONLY);
    int file_a = open("a.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int file_b = open("b.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int file_c = open("c.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int results_file = open(TIME_MEASUREMENTS_FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    HANDLE_ERROR(file_data, -1, "failed to open file dane.txt\n");
    HANDLE_ERROR(file_a, -1, "failed to open file a.txt\n");
    HANDLE_ERROR(file_b, -1, "failed to open file b.txt\n");
    HANDLE_ERROR(file_c, -1, "failed to open file c.txt\n");
    HANDLE_ERROR(results_file, -1, "failed to open file %s\n", TIME_MEASUREMENTS_FILENAME);
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
            write(file_b, file_b_content, strlen(file_b_content));
            free(file_b_content);
        }
        if (sqrt(number) == (int) sqrt(number)) {
            char* file_c_content = calloc((int)log10(number) + 1, sizeof(char));
            sprintf(file_c_content, "%d\n", number);
            write(file_c, file_c_content, strlen(file_c_content));
            free(file_c_content);
        }
    }
    char* file_a_text = calloc(strlen("Liczb parzystych jest ") + 1 + (int)log10(even_numbers_count), sizeof(char));
    sprintf(file_a_text, "Liczb parzystych jest %d", even_numbers_count);
    write(file_a, file_a_text, strlen(file_a_text));
    free(file_a_text);
    , results_file);
    close(file_data);
    close(file_a);
    close(file_b);
    close(file_c);
    close(results_file);
}
