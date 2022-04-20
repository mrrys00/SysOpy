#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define READ "r"
#define MAXBUF 262144

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        printf("Not enough args");
        exit(EXIT_FAILURE);
    }
    FILE *_if = fopen(args[1], READ), *_of = fopen(args[2], READ);
    if (_if == NULL || _of == NULL)
        exit(EXIT_FAILURE);

    int ln = atoi(args[3]), lid = 0;
    char line[MAXBUF], buf[MAXBUF];
    fgets(line, MAXBUF, _if);
    
    while (lid != ln)
    {
        fgets(buf, MAXBUF, _of);
        lid++;
    }

    buf[strlen(buf) - 1] = '\0';

    if (strcmp(buf, line) != 0)
        printf("FAIL file: %s, lnum: %d\n", args[1], ln);
    else
        printf("OK file: %s, lnum: %d\n", args[1], ln);

    exit(EXIT_SUCCESS);
}
