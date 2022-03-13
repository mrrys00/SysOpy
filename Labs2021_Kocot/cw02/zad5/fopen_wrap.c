#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096

int main(int argc, char * argv[]) {
    FILE *fin =  fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    int line = 0;
    char c = '\n';

    while((c = fgetc(fin)) != EOF) {
        line++;
        if(c == '\n') {
            line = 0;
        }
        else if(line > 50) {
            line -= 50;
            fputc('\n', fout);
        }
        fputc(c, fout);
    }

    fclose(fin);
    fclose(fout);
}
