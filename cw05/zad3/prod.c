#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ "r"
#define WRITE "w"

int main(int argc, char *args[])
{
    srand(time(NULL));
    if (argc < 5)
    {
        printf("Not enough args");
        exit(EXIT_FAILURE);
    }
    FILE *pipe_ptr = fopen(args[1], WRITE), *fp = fopen(args[4], READ);
    int to_read_cnt = atoi(args[2]);
    char *_id = args[3];
    
    if (pipe_ptr == NULL || fp == NULL)
        exit(EXIT_FAILURE);

    int read_cnt;
    char buf[_SC_LINE_MAX], products[_SC_LINE_MAX];
    while ((read_cnt = fread(buf, sizeof(char), to_read_cnt, fp)) != 0)
    {
        sleep(rand() % 2 + 1);
        buf[read_cnt] = '\0';
        sprintf(products, "%s %s\n", _id, buf);
        fwrite(products, strlen(products), sizeof(char), pipe_ptr);
        fflush(pipe_ptr);
    }
    fclose(pipe_ptr);

    exit(EXIT_SUCCESS);
}
