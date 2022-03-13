#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096
#define MAX_LINE 260

int main(int argc, char * argv[]) {
    char c = '\n', target = argv[1][0];
    char *file = argv[2];
    
    FILE *fp = fopen(file, "r");
    int found, offset;
    
    while(c != EOF) {
        found = 0;
        offset = 0;
        while ((c = fgetc(fp)) != EOF) {
            offset++;
            if(c == '\n') break; 
            else if(c == target) found = 1;
        }
        if(found) {
            fseek(fp, -offset, SEEK_CUR);
            for(int i=0; i<offset; i++) {
                c = fgetc(fp);
                printf("%c", c);
            }
        }
    }
    
    fclose(fp);
}
