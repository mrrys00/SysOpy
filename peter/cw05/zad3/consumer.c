#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/file.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Invalid argument number");
        return EXIT_FAILURE;
    }
    FILE* named_pipe = fopen(argv[1], "r");
    if (named_pipe == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    const int MAX_LINES = 100;
    char* lines[MAX_LINES];
    for (int i = 0; i < MAX_LINES; ++i) {
        lines[i] = calloc(100000, sizeof(char));
        lines[i][0] = '\0';
    }
    
    char buffer[100000];
    while (fgets(buffer, 100000, named_pipe) != NULL) {
        for (int i = 0; i < MAX_LINES; ++i) {
            lines[i][0] = '\0';
        }
        int max_used_line = 0;
        char line[100000];
        strcpy(line, strchr(buffer, ' ') + 1);
        int line_number = atoi(strtok(buffer, " "));
        line[strlen(line) - 1] = '\0';
        int fd = open(argv[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            perror("file opening error");
        }
        if (flock(fd, LOCK_EX) != 0) {
            perror("locking file error");
        }
        int read_bytes_count;
        while ((read_bytes_count = read(fd, buffer, 100000)) > 0) {
            int line_offset = strlen(lines[max_used_line]);
            for (int i = 0; i < read_bytes_count; ++i) {
                if (buffer[i] == '\n') {
                    lines[max_used_line][line_offset] = '\0';
                    ++max_used_line;
                    line_offset = 0;
                    continue;
                }
                lines[max_used_line][line_offset] = buffer[i];
                ++line_offset;
            }
        }
        strcat(lines[line_number - 1], line);
        if (line_number > max_used_line) {
            max_used_line = line_number;
        }
        lseek(fd, 0, SEEK_SET);
        for (int i = 0; i < max_used_line; ++i) {
            strcat(lines[i], "\n");
            write(fd, lines[i], strlen(lines[i]));
        }
        if (flock(fd, LOCK_UN) != 0) {
            perror("release lock error");
        };
        close(fd);
    }
}