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
void int2string(int x, char *buf, int bufsize) {
    int d = 0;
    if(x < 0) {
        buf[0] = '-';
        d++;
        bufsize--;
    }
    else if(x == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    else {
        int pow = 1;
        for(int i=1; i<bufsize-1 && x >= pow*10; i++) {
            pow *= 10;
        }
        while(pow >= 1) {
            buf[d++] = '0' + (x / pow) % 10;
            pow /= 10;
        }
        buf[d] = '\0';
    }
}

int main() {
    int fdat, fa, fb, fc, rd = 1, i, len, even = 0;
    char c, buf[MAX_DIGITS + 1];
    const char endl = '\n';
    const char *ames = "Liczb parzystych jest: ";
    
    fdat = open(FDATA, O_RDONLY);
    fa   = open(FA, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    fb   = open(FB, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    fc   = open(FC, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    
    while(rd > 0) {
        i = 0;
        len = 0;
        while((rd = read(fdat, &c, 1))) {
            if(c != '\n') {
                buf[i++] = c;
            } else break;
        }
        buf[i] = '\0';
        len = i;
        if(digit_even(buf[len-1])) even++;
        if(digit0or7(buf[len-2])) {
            write(fb, buf, len);
            write(fb, &endl, 1);
        }
        if(is_square(buf)) {
            write(fc, buf, len);
            write(fc, &endl, 1);
        }
    }
    write(fa, ames, 23);
    int2string(even, buf, MAX_DIGITS+1);
    for(int i=0; buf[i] != '\0'; i++) {
        write(fa, &(buf[i]), 1);
    }

    close(fdat);
    close(fa);
    close(fb);
    close(fc);
}
