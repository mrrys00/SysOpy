#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
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

void pizzop(int semid, short semnum, int delta) {  // operation on pizza
    sembuf s;
    s.sem_num = semnum;
    s.sem_op = delta;
    s.sem_flg = 0;
    semop(semid, &s, 1);
}

int findfree(int* array, size_t size) {
    for(int i = 0; i < size; i++) {
        if(array[i] != -1) return i;
    }
    return -1;
}

void fayrant(int signo) {
    shmdt(oven);
    shmdt(table);
    exit(0);
}

int main(int argc, char * argv[]) {
    signal(SIGINT, fayrant);

    ovenid = shmget(ftok(HOME, PROJOVEN), 0, 0);
    tableid = shmget(ftok(HOME, PROJTABLE), 0, 0);
    oven = shmat(ovenid, NULL, 0);
    table = shmat(tableid, NULL, 0);
    semph = semget(ftok(HOME, PROJSEM), 0, 0);
    int index, type;

    srand(time(NULL));

    while(true) {
        usleep(1e6 + rand1e6);

        struct timeval tv;

        pizzop(semph, SEMPIZZRDY, -1);
        pizzop(semph, SEMTABLWIN, -1);
        pizzop(semph, SEMTABLE, 1);
        index = findfree(table, TABLESIZE);
        type = table[index];
        table[index] = -1;
        pizzop(semph, SEMTABLWIN, 1);

        gettimeofday(&tv, NULL);
        printf("D_  My PID is %d. Time is %ld%03d ms. I'm taking pizza %d; there are %d on the table\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type, TABLESIZE - semctl(semph, SEMTABLE, GETVAL));
        usleep(4e6 + rand1e6);

        gettimeofday(&tv, NULL);
        printf("_D  My PID is %d. Time is %ld%03d ms. I delivered pizza %d\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type);
        usleep(4e6 + rand1e6);
    }
}
