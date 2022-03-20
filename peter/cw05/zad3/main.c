#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>

#define PIPE_PATH "./producer_consumer_pipe"

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Invalid argument count");
        return EXIT_FAILURE;
    }
    mode_t pipe_mode = S_IFIFO | S_IRUSR | S_IWUSR;
    if (mkfifo(PIPE_PATH, pipe_mode) != 0) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < atoi(argv[1]); ++i) {
        if (fork() == 0) {
            char producer_id[10];
            char read_file_path[100];
            sprintf(producer_id, "%d", i + 1);
            sprintf(read_file_path, argv[4], i + 1);
            execl("./producer.o", "./producer.o", PIPE_PATH, argv[3], producer_id, read_file_path, NULL);
        }
    }
    remove("./consumer_write.txt");
    for (int i = 0; i < atoi(argv[2]); ++i) {
        if (fork() == 0) {
            execl("./consumer.o", "./consumer.o", PIPE_PATH, argv[3], "./consumer_write.txt", NULL);
        }
    }
    while(wait(NULL) > 0);
    printf("TESTS for %d producers, %d consumers, N = %d\n", atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    for (int i = 0; i < atoi(argv[1]); ++i) {
        if (fork() == 0) {
            char producer_id[10];
            char read_file_path[100];
            sprintf(producer_id, "%d", i + 1);
            sprintf(read_file_path, argv[4], i + 1);
            execl("./test.o", "./test.o", read_file_path, "./consumer_write.txt", producer_id, NULL);
        }
    }
    while(wait(NULL) > 0);
    remove(PIPE_PATH);
}
