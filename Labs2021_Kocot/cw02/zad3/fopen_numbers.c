#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PATH_MAX 4096
#define MAX_DIGITS 10 
#define FDATA "data.txt"
#define FA "a.txt"
#define FB "b.txt"
#define FC "c.txt"

int digit_even(char d) {
    return (d == '0' || d == '2' || d == '4' || d == '6' || d == '8');
}
int digit0or7(char d) {
    return (d == '0' || d == '7');
}
int is_square(char *s) {
    int num = atoi(s);
    double root = sqrt((double) num);
    int f = floor(root);
    int c = ceil(root);
    return (num == f*f || num == c*c);
}

int main() {
    int i, len, even = 0;
    char c = '\n', buf[MAX_DIGITS + 1];
    FILE *fdat, *fa, *fb, *fc;
    
    fdat = fopen(FDATA, "r");
    fa   = fopen(FA, "w");
    fb   = fopen(FB, "w");
    fc   = fopen(FC, "w");
    
    while(c != EOF) {
        i = 0;
        len = 0;
        while((c = fgetc(fdat)) != '\n' && c != EOF) {
            if(c != '\n') {
                buf[i++] = c;
            } else break;
        }
        buf[i] = '\0';
        len = i;
        if(digit_even(buf[len-1])) even++;
        if(digit0or7(buf[len-2])) {
            fprintf(fb, "%s\n", buf);
        }
        if(is_square(buf)) {
            fprintf(fc, "%s\n", buf);
        }
    }
    fprintf(fa, "Liczb parzystych jest: %d", even);

    fclose(fdat);
    fclose(fa);
    fclose(fb);
    fclose(fc);
}
