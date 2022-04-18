#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <linux/limits.h>

void process_line(char* l_buff) 
{
    int pos = 0, 
    while(l_buff)
    {

    }
    return;
}

void interpreter(FILE *fp)
{
    char buf[_SC_LINE_MAX];
    while (fgets(buf, _SC_LINE_MAX, fp) != NULL)
    {

    }
}

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(args[1], "r");

    interpreter(fp);
    fclose(fp);
    exit(EXIT_SUCCESS);
}
