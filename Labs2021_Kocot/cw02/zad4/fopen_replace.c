#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096

char* check(FILE *fin, char *goal, char *replacement, char* buf, char inchar) {
    char c;
    buf[0] = inchar;
    for(int i=1; i<strlen(goal); i++) {
        if((c = fgetc(fin)) == EOF) {
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
    FILE *fin =  fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    char c = '\n';

    char *s1 = argv[3];
    char *s2 = argv[4];
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char trigger = s1[0];

    char *buf = (char*) calloc((len1>len2 ? len1+1 : len2+1), sizeof(char));
    char *swap;

    while((c = fgetc(fin)) != EOF) {
        if(c == trigger) {
            swap = check(fin, s1, s2, buf, c);
            fprintf(fout, "%s", swap);
        }
        else {
            fputc(c, fout);
        }
    }

    fclose(fin);
    fclose(fout);
    free(buf);
}
