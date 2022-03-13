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
#include <sys/mman.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include "consts.h"

int ovenid, tableid;
int *ovenmem, *tablemem;
sem_t *oven, *table, *ovenwin, *tablwin, *pizzrdy;

void fayrant(int signum) {
    sem_close(oven);
    sem_close(table);
    sem_close(ovenwin);
    sem_close(tablwin);
    sem_close(pizzrdy);
    sem_unlink(SEMOVEN);
    sem_unlink(SEMTABLE);
    sem_unlink(SEMOVENWIN);
    sem_unlink(SEMTABLWIN);
    sem_unlink(SEMPIZZRDY);
    munmap(ovenmem, OVENSIZE * sizeof(int));
    munmap(tablemem, TABLESIZE * sizeof(int));
    shm_unlink(MEMOVEN);
    shm_unlink(MEMTABLE);
    exit(0);
}

int semctrl(sem_t* sem) {
    int a;
    sem_getvalue(sem, &a);
    return a;
}

int main(int argc, char * argv[]) {
    signal(SIGINT, fayrant);

    table = sem_open(SEMTABLE, O_CREAT | O_RDWR, 0666, TABLESIZE);
    oven = sem_open(SEMOVEN, O_CREAT | O_RDWR, 0666, OVENSIZE);

    tablwin = sem_open(SEMTABLWIN, O_CREAT | O_RDWR, 0666, 1);
    ovenwin = sem_open(SEMOVENWIN, O_CREAT | O_RDWR, 0666, 1);
    pizzrdy = sem_open(SEMPIZZRDY, O_CREAT | O_RDWR, 0666, 0);

    ovenid = shm_open(MEMOVEN, O_CREAT | O_RDWR, 0666);
    tableid = shm_open(MEMTABLE, O_CREAT | O_RDWR, 0666);
    ftruncate(ovenid, OVENSIZE * sizeof(int));
    ftruncate(tableid, TABLESIZE * sizeof(int));
    ovenmem = mmap(NULL, OVENSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, ovenid, 0);
    tablemem = mmap(NULL, TABLESIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tableid, 0);
    
    pid_t pid;

    for(int i=0; i<OVENSIZE; i++) ovenmem[i] = -1;
    for(int i=0; i<TABLESIZE; i++) tablemem[i] = -1;
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
