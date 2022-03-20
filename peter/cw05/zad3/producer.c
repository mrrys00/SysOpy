#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    srand(time(NULL));
    if (argc != 5) {
        perror("Invalid number of arguments");
        return EXIT_FAILURE;
    } 
    FILE* consumer_pipe = fopen(argv[1], "w");
    int numer_of_bytes_to_read = atoi(argv[2]);
    char* producer_id = argv[3];
    FILE* read_file = fopen(argv[4], "r");
    if (consumer_pipe == NULL) {
        printf("open pipe");
        return EXIT_FAILURE;
    }
    if (read_file == NULL) {
        printf("opening %s", argv[4]);
        return EXIT_FAILURE;
    }
    char buffer[10000];
    char written_data[10000];
    int read_bytes_count;
    while ((read_bytes_count = fread(buffer, sizeof(char), numer_of_bytes_to_read, read_file)) != 0) {
        buffer[read_bytes_count] = '\0';
        sprintf(written_data, "%s %s\n", producer_id, buffer);
        fwrite(written_data, strlen(written_data), sizeof(char), consumer_pipe);
        fflush(consumer_pipe);
        sleep(1 + rand() % 2);
    }
    fclose(consumer_pipe);
}
