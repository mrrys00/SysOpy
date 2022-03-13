#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096

char* check(int fin, char *goal, char *replacement, char* buf, char inchar) {
    char c;
    buf[0] = inchar;
    for(int i=1; i<strlen(goal); i++) {
        if(read(fin, &c, 1) == 0) {
            buf[i] = '\0';
            return buf;
        }
        else if(c != goal[i]) {
            buf[i] = c;
            buf[i+1] = '\0';
            return buf;
        } 
        else {
            buf[i] = c;
        }
    }
    return replacement;
}

int main(int argc, char * argv[]) {
    int fin =  open(argv[1], O_RDONLY, 0666);
    int fout = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0666);
    int rd;
    char c = '\n';

    char *s1 = argv[3];
    char *s2 = argv[4];
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char trigger = s1[0];

    char *buf = (char*) calloc((len1>len2 ? len1+1 : len2+1), sizeof(char));
    char *swap;

    while((rd = read(fin, &c, 1))) {
        if(c == trigger) {
            swap = check(fin, s1, s2, buf, c);
            write(fout, swap, strlen(swap));
        }
        else {
            write(fout, &c, 1);
        }
    }

    close(fin);
    close(fout);
    free(buf);
}
