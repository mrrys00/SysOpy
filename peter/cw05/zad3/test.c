#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Invalid number of arguments\n");
        return EXIT_FAILURE;
    }
    FILE* input_file = fopen(argv[1], "r");
    FILE* output_file = fopen(argv[2], "r");
    int line_number = atoi(argv[3]);
    if (input_file == NULL) {
        printf("failed to open input file\n");
        return EXIT_FAILURE;
    }
    if (output_file == NULL) {
        printf("failed to open output file\n");
        return EXIT_FAILURE;
    }
    char line[100000];
    fgets(line, 100000, input_file);
    int line_idx = 0;
    char buffer[100000];
    while (line_idx != line_number) {
        fgets(buffer, 100000, output_file);
        ++line_idx;
    }
    
    buffer[strlen(buffer) - 1] = '\0';
    if (strcmp(buffer, line) == 0) {
        printf("PASSED test for files: %s, %s, line: %d\n", argv[1], argv[2], line_number);
    } else {
        printf("FAILED test for files: %s, %s, line: %d\n", argv[1], argv[2], line_number);
        // printf("expeced: '%s'\n", buffer);
        // printf("received: '%s'\n", line);
    }
    return EXIT_SUCCESS;
}
