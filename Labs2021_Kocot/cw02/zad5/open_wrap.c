#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096

int main(int argc, char * argv[]) {
    int fin =  open(argv[1], O_RDONLY, 0666);
    int fout = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0666);
    int rd, line = 0;
    char c = '\n';
    const char endl = '\n';

    while((rd = read(fin, &c, 1))) {
        line++;
        if(c == '\n') {
            line = 0;
        }
        else if(line > 50) {
            line -= 50;
            write(fout, &endl, 1);
        }
        write(fout, &c, 1);
    }

    close(fin);
    close(fout);
}
