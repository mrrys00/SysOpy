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
    char c, target = argv[1][0];
    char *file = argv[2];
    
    int fd = open(file, O_RDONLY, 0666);
    int rd = 1, found, offset;
    
    while(rd > 0) {
        found = 0;
        offset = 0;
        while ((rd = read(fd, &c, 1))) {
            offset++;
            if(c == '\n') break; 
            else if(c == target) found = 1;
        }
        if(found) {
            lseek(fd, -offset, SEEK_CUR);
            for(int i=0; i<offset; i++) {
                read(fd, &c, 1);
                printf("%c", c);
            }
        }
    }
    
    close(fd);
}
