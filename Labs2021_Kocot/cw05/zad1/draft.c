#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXPATH 4096  // maximum filename
#define MAXLINE 1024  // maximum line length in a file
#define MAXPIPES 10  // maximum chain length
#define MAXCHAIN 2048  // maximum chain letter numer
#define MAXK 10  // maxlimum parts in the file

int findeq(char*);

int main(int argc, char * argv[]) {
    int fd[2], fe[2];
    pipe(fd); pipe(fe);
    //printf("%d %d %d %d\n", fd[0], fd[1], fe[0], fe[1]);
    char aaa[] = "/etc/passwd\n";
    char ccc[100];
    write(fd[1], aaa, 13);
    pid_t pid = fork();
    if(pid==0) {
        close(fd[1]);
        close(fe[0]);
        char bbb[20];
        //read(fd[0], bbb, 20);printf("%s\n", bbb);
        dup2(fd[0], STDIN_FILENO);
        dup2(fe[1], STDOUT_FILENO);
        execlp("sh", "sh", "-c", "head -n 1", NULL);
    }
    else {
        close(fd[0]);
        close(fe[1]);
        //read(fe[0], ccc, 50);
        pid = fork();
        if(pid==0) {
            dup2(fe[0], STDIN_FILENO);
            execlp("head", "head", NULL);
        }
        else wait(NULL);
    }

    //FILE *fp = fopen(argv[1], "r");
    //char

    char *instr[MAXK];
    int eqpos[MAXK];

    instr[0] = "s1 = aaa";
    instr[1] = "s2 = bbb";
    instr[2] = "s3 = ccc";
}

int findeq(char *str) {
    for(int i=0; str[i] != '\0'; i++) {
        if(str[i] == '=') return i;
    }
    return -1;
}
