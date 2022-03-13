#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_NAME 1024
#define MAX_LINE 4096
#define MAX_DET 1024
#define MAX_PATH 4096
#define MAIN "./finder"

int lookup(char* path, char* filename, char* goal) {
    int len = strlen(filename);
    if(len < 5) return 0;  // ignore non-txt files
    else if(filename[len-4] != '.' || filename[len-3] != 't' || filename[len-2] != 'x' || filename[len-1] != 't') return 0;  // ignore non-txt files

    char file[MAX_PATH + 1], buf[MAX_LINE+1];
    int i = 0, j = 0;
    FILE* fp;
    while(path[i] != '\0') {
        file[i] = path[i];
        i++;
    }
    file[i++] = '/';
    while(filename[j] != '\0') {
        file[i+j] = filename[j];
        j++;
    }
    file[i+j] = '\0';
    fp = fopen(file, "r");
    if(fp == NULL) return 0;
    else {
        while(fgets(buf, MAX_LINE, fp) != NULL) {
            if(strstr(buf, goal) != NULL) return 1;
        }
    }
    return 0;
}

int append(char* text, char* paper1, char* paper2) {
    int i = 0;
    while(text[i] != '\0') {
        paper1[8 + i] = text[i];
        paper2[11 + i] = text[i];
        i++;
    }
    paper1[8 + i] = '\0';
    paper2[11 + i] = '\0';
    return i;
}

int construct_path(char* path, char* part1, char* part2) {
    int j = 0, i = 0;
    while(part1[i] != '\0') {
        path[i] = part1[i];
        i++;
    }
    
    if(part2 != NULL) {
        path[i++] = '/';
        while(part2[j] != '\0') {
            path[i + j] = part2[j];
            j++;
        }
    }
    return i + j;
}

int path_offset(char* path) {
    int i;
    for(i=0; path[i] != '\0'; i++) {
        if(path[i] == '/') return i;
    }
    return i;
}

char* strip_fgets(char *str, int n, FILE *stream) {
    if(n <= 0) return NULL;
    int i = 0;
    char c = fgetc(stream);
    while(c == '\n') c = fgetc(stream);
    if(c == EOF) return NULL;
    else while(c != EOF && c != '\n') {
        str[i++] = c;
        if(i >= n) break;
        c = fgetc(stream);
    }
    str[i] = '\0';
    return str;
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

int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("Not enough arguments\n");
        return 1;
    }
    char path[MAX_PATH];
    int depth_left = atoi(argv[3]);
    //if(depth_left <= 0) return 0;

    if(argc == 4) construct_path(path, argv[1], NULL);
    else construct_path(path, argv[1], argv[4]); 

    char* target = argv[2];

    char command[MAX_PATH + 8] = "/bin/ls ";
    char detcomm[MAX_PATH + 11] = "/bin/ls -l ";
    append(path, command, detcomm);

    FILE *list, *detlist;
    list    = popen(command, "r");
    detlist = popen(detcomm, "r");
    char fname[MAX_NAME], detail[MAX_DET];
    pid_t pid;

    fgets(detail, MAX_DET, detlist);

    //strip_fgets(fname, MAX_NAME, list);
    //printf("%s\n%s\n%s\n", command, detcomm, fname);
    while (strip_fgets(fname, MAX_NAME, list) != NULL && strip_fgets(detail, MAX_DET, detlist) != NULL) {
        if(detail[0] == 'd' && depth_left > 0) {  // file is a directory
            pid = fork();
            if(pid == 0) {
                char num[10];
                int2string(depth_left - 1, num, 10);
                execl(MAIN, MAIN, path, target, num, fname, NULL);
            }
            else waitpid(pid, NULL, 0);
        } else if (detail[0] == '-') {  // file is a regular file (neither directory nor a link)
            //printf("File %s/%s, found by process %d\n", path, fname, getpid());
            if(lookup(path, fname, target)) 
                printf("--> \"%s\" found in .%s/%s by process %d\n", target, &(path[path_offset(path)]), fname, getpid());
        }
        //printf("%c : %s", detail[0], fname);
    }
}
