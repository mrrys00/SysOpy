#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include "consts.h"

int ovenid, tableid, semph;
int *oven, *table;

void forcekill(int signum) {
    //for(int i=0; i<OVENSIZE; i++) printf("%d\n", oven[i]); 
    semctl(semph, 0, IPC_RMID);
    shmctl(ovenid, IPC_RMID, NULL);
    shmctl(tableid, IPC_RMID, NULL);
    exit(0);
}

void setinit(int semid) {
    semun arg;
    arg.val = TABLESIZE;
    semctl(semid, SEMTABLE, SETVAL, arg);
    arg.val = OVENSIZE;
    semctl(semid, SEMOVEN, SETVAL, arg);
    arg.val = 1;
    semctl(semid, SEMTABLWIN, SETVAL, arg);
    semctl(semid, SEMOVENWIN, SETVAL, arg);
    arg.val = 0;
    semctl(semid, SEMPIZZRDY, SETVAL, arg);
}

int main(int argc, char * argv[]) {
    signal(SIGINT, forcekill);
    signal(SIGSEGV, forcekill);

    ovenid = shmget(ftok(HOME, PROJOVEN), OVENSIZE * sizeof(int), IPC_CREAT | 0666);
    tableid = shmget(ftok(HOME, PROJTABLE), TABLESIZE * sizeof(int), IPC_CREAT | 0666);
    oven = shmat(ovenid, NULL, 0);
    table = shmat(tableid, NULL, 0);
    semph = semget(ftok(HOME, PROJSEM), 5, IPC_CREAT | 0666);
    pid_t pid;

    setinit(semph);
    for(int i=0; i<OVENSIZE; i++) oven[i] = -1;
    for(int i=0; i<TABLESIZE; i++) table[i] = -1;

    for(int i = 0; i < PIZZNUM; i++) {
        pid = fork();
        if(pid == 0) {
            execl(PIZZPATH, PIZZPATH, NULL);
        }
        sleep(1);
    }
    for(int i = 0; i < DELIVNUM; i++) {
        pid = fork();
        if(pid == 0) {
            execl(DELIVPATH, DELIVPATH, NULL);
        }
        sleep(1);
    }

    while(true) {
        usleep(2e6);
    }
}
