#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NAME 1024
#define MAX_DET 1024
#define MAX_PATH 4096  // + 10 for "/bin/ls ./"

int append(char* text, char* paper1, char* paper2, int pathlen) {
    int i = 0;
    while(text[i] != '\0') {
        paper1[8+pathlen+i] = text[i];
        paper2[11+pathlen+i] = text[i];
        i++;
    }
    paper1[8+pathlen+i] = '\0';
    paper2[11+pathlen+i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    char command[MAX_PATH + 8] = "/bin/ls ";
    char detcomm[MAX_PATH + 11] = "/bin/ls -l ";
    //char path[MAX_PATH];
    int pathlen = append("data", command, detcomm, 0);  // current path length ("./" is equal to 0)

    FILE *list, *detlist;
    list    = popen(command, "r");
    detlist = popen(detcomm, "r");
    char fname[MAX_NAME], detail[MAX_DET];
    pid_t pid;

    fgets(detail, MAX_DET, detlist);

    //printf("%s\n%s\n", command, detcomm);
    while (fgets(fname, MAX_NAME, list) != NULL && fgets(detail, MAX_DET, detlist) != NULL) {
        if(detail[0] == 'd') {  // file is a directory
            pid = fork();
            if(pid == 0) {

            }
        } else if (detail[0] == '-') {  // file is a regular file (neither directory nor a link)
            printf("File %s, found by process %d", fname, getpid());
        }
        //printf("%c : %s", detail[0], fname);
    }
}
