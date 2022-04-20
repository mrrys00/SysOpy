#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/file.h>

#define READ "r"
#define FILELINES 128

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        printf("Not enough args");
        exit(EXIT_FAILURE);
    }

    FILE *fifo = fopen(args[1], READ);
    if (fifo == NULL)
        exit(EXIT_FAILURE);

    char *lin_arr[FILELINES], buf[_SC_LINE_MAX];
    for (int i = 0; i < FILELINES; i++)
    {
        lin_arr[i] = calloc(_SC_LINE_MAX, sizeof(char));
        lin_arr[i][0] = '\0';
    }

    
    while (fgets(buf, _SC_LINE_MAX, fifo) != NULL)
    {
        for (int i = 0; i < FILELINES; i++)
            lin_arr[i][0] = '\0';
            
        int idx_last = 0;
        char line[_SC_LINE_MAX];
        strcpy(line, strchr(buf, ' ') + 1);
        int ln = atoi(strtok(buf, " "));
        line[strlen(line) - 1] = '\0';

        int fd = open(args[3], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0)
            perror("file opening error");
        if (flock(fd, LOCK_EX) != 0)
            perror("locking file error");

        int read_cnt;
        while ((read_cnt = read(fd, buf, _SC_LINE_MAX)) > 0)
        {
            int offs = strlen(lin_arr[idx_last]);
            for (int i = 0; i < read_cnt; i++)
            {
                if (buf[i] == '\n')
                {
                    lin_arr[idx_last][offs] = '\0';
                    idx_last++;
                    offs = 0;
                    continue;
                }
                lin_arr[idx_last][offs] = buf[i];
                offs++;
            }
        }

        strcat(lin_arr[ln - 1], line);
        if (ln > idx_last)
            idx_last = ln;

        lseek(fd, 0, SEEK_SET);
        for (int i = 0; i < idx_last; i++)
        {
            strcat(lin_arr[i], "\n");
            write(fd, lin_arr[i], strlen(lin_arr[i]));
        }
        if (flock(fd, LOCK_UN) != 0)
            perror("error: lock release");
        close(fd);
    }

    exit(EXIT_SUCCESS);
}