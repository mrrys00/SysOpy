//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

char* get_line(char* buffer, int file) {
    int buffer_position = 1;
    if (!read(file, buffer, 1)) {
        return NULL;
    }
    while (buffer[buffer_position - 1] != '\n') {
        if (!read(file, buffer + buffer_position, 1)) {
            buffer[buffer_position + 1] = '\0';
            return buffer;
        };
        buffer_position++;
    }
    buffer[buffer_position] = '\0';
    return buffer;
}

int main(int argc, char *args[]) {
    char* fn1;
    char* fn2;
    if (argc < 3) {
        char buffer[1000];

        printf("first file:\n");
        scanf("%s", (char *) &buffer);
        fn1 = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(fn1, buffer);

        printf("second file:\n");
        scanf("%s", (char *) &buffer);
        fn2 = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(fn2, buffer);
    } else {
        fn1 = args[1];
        fn2 = args[2];
    }

    char blok[1024];
    int we, wy;
    we = open(fn1, O_RDONLY);
    wy = open(fn2, O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR);
    printf("%d, %d", we, wy);

    if (we == -1) {
        printf("cannot open source file");
        exit(1);
    }
    if (wy == -1) {
        printf("cannot open destination file");
        exit(1);
    }

    int jj = 0;
    while(get_line(blok, we) != NULL) {
        char* save_buff = calloc(strlen(blok) + 1, sizeof(char));
        sprintf(save_buff, "%s", blok);
        if (save_buff[0] != '\n') {
            write(wy, save_buff, strlen(save_buff));
        }
        free(save_buff);
        jj ++;
    }
    close(we);
    close(wy);

    return 0;

}
