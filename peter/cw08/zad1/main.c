#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

int** pixels;
int** negated_pixels;
int width;
int height;
int threads_count;
FILE* times_file;

int** parse_input_file(char* filename, int* width, int* height) {
    FILE* input_file = fopen(filename, "r");
    if (input_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char buffer[10000];
    fgets(buffer, sizeof(buffer), input_file); // skip format line
    fgets(buffer, sizeof(buffer), input_file); // parse width and height
    *width = atoi(strtok(buffer, " "));
    *height = atoi(strtok(NULL, " "));
    fgets(buffer, sizeof(buffer), input_file); // skip max pixel value info as we assume 255
    int** pixels = calloc(*height, sizeof(int*)); 
    for (int row = 0; row < *height; ++row) {
        pixels[row] = calloc(*width, sizeof(int));
    }
    int x = 0;
    int y = 0;
    while (fgets(buffer, sizeof(buffer), input_file) != NULL) {
        pixels[y][x] = atoi(strtok(buffer, " "));
        ++x;
        if (x == *width) {
            x = 0;
            ++y;
        }
        char* pixel_value;
        while ((pixel_value = strtok(NULL, " ")) != NULL) {
            if (pixel_value[0] == '\n') {
                continue;
            }
            pixels[y][x] = atoi(pixel_value);
            ++x;
            if (x == *width) {
                x = 0;
                ++y;
            }
        }
    }
    fclose(input_file);
    return pixels;
}

void* numbers_thread_routine(void* thread_number_ptr) {
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
    int thread_number = *(int *)thread_number_ptr;
    int lowest_pixel = thread_number * 256 / threads_count;
    int highest_pixel = (thread_number + 1) * 256 / threads_count;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (lowest_pixel <= pixels[i][j] && pixels[i][j] < highest_pixel) {
                negated_pixels[i][j] = 255 - pixels[i][j];
            }
        }
    }
    struct timeval end_tv, result;
    gettimeofday(&end_tv, NULL);
    timersub(&end_tv, &start_tv, &result);
    char line[1000];
    sprintf(line, "Thread %d has spent %ld microsconds\n", thread_number, result.tv_sec * 1000000 + result.tv_usec);
    fputs(line, times_file);
    return NULL;
}

void* blocks_thread_routine(void* thread_number_ptr) {
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
    int thread_number = *(int *)thread_number_ptr;
    int min_column = thread_number * width / threads_count;
    int max_column = (thread_number + 1) * width / threads_count;
    for (int i = 0; i < height; ++i) {
        for (int j = min_column; j < max_column; ++j) {
            negated_pixels[i][j] = 255 - pixels[i][j];
        }
    }
    struct timeval end_tv, result;
    gettimeofday(&end_tv, NULL);
    timersub(&end_tv, &start_tv, &result);
    char line[1000];
    sprintf(line, "Thread %d has spent %ld microsconds\n", thread_number, result.tv_sec * 1000000 + result.tv_usec);
    fputs(line, times_file);
    return NULL;
}

void negate_picture(int** pixels, int threads_count, int width, int height, int use_blocks) {
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
    pthread_t* tid = calloc(threads_count, sizeof(pthread_t));
    pthread_attr_t* attr = calloc(threads_count, sizeof(pthread_attr_t));
    int* thread_numbers = calloc(threads_count, sizeof(int));
    void* (*thread_routine) (void*) = use_blocks ? blocks_thread_routine : numbers_thread_routine;
    for (int i = 0; i < threads_count; ++i) {
        thread_numbers[i] = i;
        pthread_create(tid + i, attr + i, thread_routine, thread_numbers + i);
    }
    for (int i = 0; i < threads_count; ++i) {
        if (pthread_join(tid[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    struct timeval end_tv, result;
    gettimeofday(&end_tv, NULL);
    timersub(&end_tv, &start_tv, &result);
    char line[1000];
    sprintf(line, "Whole operations took %ld microsconds\n\n", result.tv_sec * 1000000 + result.tv_usec);
    fputs(line, times_file);
    free(tid);
    free(attr);
}

void write_results(int** results, char* results_filename, int width, int height) {
    FILE* results_file = fopen(results_filename, "w");
    if (results_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fputs("P2\n", results_file);
    char line[71]; // pgm line shouldn't exceed 70 characters
    sprintf(line, "%d %d\n", width, height);
    fputs(line, results_file);
    fputs("255\n", results_file);
    int x = 0;
    int y = 0;
    while (y != height) {
        line[0] = '\0';
        for (int i = 0; i < 17 && y != height; ++i) {
            char pixel[5];
            sprintf(pixel, "%d ", results[y][x]);
            ++x;
            if (x == width) {
                x = 0;
                ++y;
            }
            strcat(line, pixel);
        }
        strcat(line, "\n");
        fputs(line, results_file);
    }
    fclose(results_file);
}

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Invalid number of arguments\n");
        return EXIT_FAILURE;
    }
    times_file = fopen("Times.txt", "a");
    if (times_file == NULL) {
        perror("fopen Times.txt");
        return EXIT_FAILURE;
    }

    threads_count = atoi(argv[1]);
    char* thread_division_mode = argv[2];
    char* input_filename = argv[3];
    char* result_filename = argv[4];

    pixels = parse_input_file(input_filename, &width, &height);
    negated_pixels = calloc(height, sizeof(int*)); 

    char line[1000];
    sprintf(line, "Testing with %d threads on image %dx%d, using division type %s\n\n", threads_count, width, height, thread_division_mode);
    fputs(line, times_file);

    for (int row = 0; row < height; ++row) {
        negated_pixels[row] = calloc(width, sizeof(int));
    }
    if (strcmp(thread_division_mode, "numbers") == 0) {
        negate_picture(pixels, threads_count, width, height, 0);
    } else if (strcmp(thread_division_mode, "block") == 0) {
        negate_picture(pixels, threads_count, width, height, 1);
    } else {
        printf("Invalid division mode\n");
        return EXIT_FAILURE;
    }
    write_results(negated_pixels, result_filename, width, height);
    for (int i = 0; i < height; ++i) {
        free(pixels[i]);
        free(negated_pixels[i]);
    }
    free(pixels);
    free(negated_pixels);
    fclose(times_file);
    return EXIT_SUCCESS;
}
