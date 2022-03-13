#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXPATH 4096  // maximum filename
#define MAXLINE 1024  // maximum line length in a file
#define MAXPIPES 10  // maximum pipe number in a chain
#define MAXCHAIN 2048  // maximum chain letter numer
#define MAXK 10  // maxlimum parts in the file
#define bool short
#define true 1
#define false 0

int K = 0;

int findeq(char*);
int findname(char*, char**, int*);
int locate_pipes(char*, int*);
char* cut_chain(char*, int, int*);

int main(int argc, char * argv[]) {
    //printf("Parent: %d\n", getpid());
    //int emergency[2];
    //pipe(emergency);
    FILE *fp = fopen(argv[1], "r");
    char c, buf[MAXLINE+1];

    int pipenum, in=-1, out=-1, xout=-1;
    //int reset_in  = dup(STDIN_FILENO);
    //int reset_out = dup(STDOUT_FILENO);
    pid_t pid = -1;
    char *chainfrg, *instrfrg;
    
    char *instr[MAXK];
    char *chain;
    int eqpos[MAXK];
    int pipeloc[MAXPIPES];

    c = '\0';
    bool eq;
    int bufpos;
    while(c != EOF) {
        c = getc(fp);
        if(c == '#' || c == ' ' || c == '\n') {
            while(c != '\n' && c != EOF) c = getc(fp);
            continue;
        }
        eq = false;
        bufpos = 0;
        while(c != '\n' && c != EOF) {
            buf[bufpos++] = c;
            if(c == '=') eq = true;
            c = getc(fp);
            if(c == '#') {
                while(c != '\n' && c != EOF) c = getc(fp);
                break;
            }
        }
        buf[bufpos++] = '\0';
        if(eq) {
            instr[K] = (char*) malloc(bufpos * sizeof(char));
            strcpy(instr[K], buf);
            eqpos[K] = findeq(instr[K]);
            K++;
        }
        else {
            chain = buf;
            printf("--- Chain: %s ---\n", chain);
            pipenum = locate_pipes(chain, pipeloc);
            int **fd;
            fd = (int**) malloc(pipenum * sizeof(int*));
            for(int i=0; i<pipenum; i++) {
                fd[i] = (int*) malloc(2 * sizeof(int));
            }

            for(int i=0; i<=pipenum; i++) {
                if(i < pipenum) {
                    pipe(fd[i]);
                    out = fd[i][1];
                    xout = fd[i][0];
                }
                else {
                    out = -1;
                    xout = -1;
                }
                
                chainfrg = cut_chain(chain, i, pipeloc);
                pid = fork();
                if(pid == 0) {
                    if(xout != -1) close(xout);
                    break;
                }
                else {
                    if(in != -1) close(in);
                    if(out != -1) close(out);
                    if(i < pipenum) in = fd[i][0];
                }
            }
            if(pid == 0) {
                if(in  >= 0) dup2(in,  STDIN_FILENO);
                if(out >= 0) dup2(out, STDOUT_FILENO);
                
                instrfrg = instr[findname(chainfrg, instr, eqpos)];
                execl("/bin/sh", "sh", "-c", &instrfrg[findeq(instrfrg) + 2], NULL);
            }
            else {
                for(int i=0; i<=pipenum; i++) {
                    wait(NULL);
                }

                for(int i=0; i<pipenum; i++) {
                    free(fd[i]);
                }
                free(fd);
                printf("\n");
            }
        }
    }
    for(int i=0; i<K; i++) {
        free(instr[i]);
    }

    

    //printf("s2: %d\n", findname("s2", instr, eqpos));
    //printf("\"s3 \": %d\n", findname("s3 ", instr, eqpos));
}

/////////////////////////////////////////////////////////////////////////

int findeq(char* str) {
    for(int i=0; str[i] != '\0'; i++) {
        if(str[i] == '=') return i;
    }
    return -1;
}

int findname(char* target, char** strarr, int* eqpos) {
    bool found = false;
    for(int i = 0; i < K; i++) {
        for(int c=0; c < eqpos[i]; c++) {
            //printf("%d ?= %d\n", strarr[i][c], target[c]);
            if((target[c] == '\0' || target[c] == ' ') && strarr[i][c] == ' ') {
                found = true;
                break;
            }
            else if(target[c] != strarr[i][c]) break;
        }
        if(found) return i;
    }
    return -1;
}

int locate_pipes(char* chain, int* pipeloc) {
    int pipenum = 0;
    for(int i=0; chain[i] != '\0'; i++) {
        if(chain[i] == '|') {
            if(pipenum >= MAXPIPES) return -1;  // too many pipes - will return error
            pipeloc[pipenum++] = i;
        }
    }
    return pipenum;
}

char* cut_chain(char* chain, int childno, int* pipeloc) {
    if(childno == 0) return chain;
    else {
        return &chain[pipeloc[childno - 1] + 2];
    }
}

