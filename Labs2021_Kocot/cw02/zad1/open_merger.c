#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096 

void merge_two(int fin1, int fin2) {
    char c1='\n', c2='\n';
    int state = 1, rd = 1;
    while (rd > 0) {
        printf("\n");
        if (state == 1) {
            while ((rd = read(fin1, &c1, 1)) && c1 != '\n') {
                printf("%c", c1);
            }
            state = 2;
        }
        else {
            while ((rd = read(fin2, &c2, 1)) && c2 != '\n') {
                printf("%c", c2);
            }
            state = 1;
        }
    }
    while (read(fin1, &c1, 1)) {
        printf("%c", c1);
    }
    while (read(fin2, &c2, 1)) {
        printf("%c", c2);
    }
}

int main(int argc, char * argv[]) {
    char file1[PATH_MAX+1], file2[PATH_MAX+1];
    int fd1, fd2;
    if(argc >= 3) {
        strcpy(file1, argv[1]);
        strcpy(file2, argv[2]);
    } else if(argc == 2) {
        strcpy(file1, argv[1]);
        printf("Enter file 2 path: ");
        scanf("%s", file2);
    } else {
        printf("Enter file paths: ");
        scanf("%s %s", file1, file2);
    }
    
    fd1 = open(file1, O_RDONLY, 0666);
    fd2 = open(file2, O_RDONLY, 0666);
    
    merge_two(fd1, fd2);
    printf("\n");
    
    close(fd1);
    close(fd2);
}
