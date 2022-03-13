#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <time.h>

#define bool short
#define true 1
#define false 0
#define PROD "producer"
#define CONS "consumer"
#define PIPE argv[1]
#define N argv[2]

int main(int argc, char * argv[]) {
    mkfifo(PIPE, 0666);

    pid_t pid[6];
    pid[0] = fork();
    if(pid[0] == 0) {
        execl(CONS, CONS, PIPE, "shelf.txt", N, NULL);
    }
    pid[1] = fork();
    if(pid[1] == 0) {
        execl(PROD, PROD, PIPE, "1", "storage1.txt", N, NULL);
    }
    pid[2] = fork();
    if(pid[2] == 0) {
        execl(PROD, PROD, PIPE, "2", "storage2.txt", N, NULL);
    }
    pid[3] = fork();
    if(pid[3] == 0) {
        execl(PROD, PROD, PIPE, "3", "storage3.txt", N, NULL);
    }
    pid[4] = fork();
    if(pid[4] == 0) {
        execl(PROD, PROD, PIPE, "4", "storage4.txt", N, NULL);
    }
    pid[5] = fork();
    if(pid[5] == 0) {
        execl(PROD, PROD, PIPE, "5", "storage5.txt", N, NULL);
    }
    for(int i=1; i<=5; i++) {
        waitpid(pid[i], NULL, 0);
    }

    pid_t exitpid = fork();
    if(exitpid == 0) execl(PROD, PROD, PIPE, N, NULL);
    else {
        waitpid(exitpid, NULL, 0);
        unlink(PIPE);
    }
}
