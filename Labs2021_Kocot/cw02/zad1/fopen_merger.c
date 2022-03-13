#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 4096 

void merge_two(FILE *fin1, FILE *fin2) {
    char c1='\n', c2='\n';
    int state = 1;
    while (c1 != EOF && c2 != EOF) {
        printf("\n");
        if (state == 1) {
            while ((c1 = fgetc(fin1)) != '\n' && c1 != EOF) {
                printf("%c", c1);
            }
            state = 2;
        } else {
            while ((c2 = fgetc(fin2)) != '\n' && c2 != EOF) {
                printf("%c", c2);
            }
            state = 1;
        }
    }
    if(c1 == EOF && c2 != EOF) {
        while ((c2 = fgetc(fin2)) != EOF) {
            printf("%c", c2);
        }
    } else if(c1 != EOF && c2 == EOF) {
        while ((c1 = fgetc(fin1)) != EOF) {
            printf("%c", c1);
        }
    }
}

int main(int argc, char * argv[]) {
    char file1[PATH_MAX+1], file2[PATH_MAX+1];
    FILE *fp1, *fp2;
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
    
    fp1 = fopen(file1, "r");
    fp2 = fopen(file2, "r");
    
    merge_two(fp1, fp2);
    printf("\n");
    
    fclose(fp1);
    fclose(fp2);
}
