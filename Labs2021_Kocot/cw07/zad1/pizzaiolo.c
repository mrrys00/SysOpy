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
        if(array[i] == -1) return i;
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
        type = rand() % 10;
        usleep(1e6 + rand1e6);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("P__ My PID is %d. Time is %ld%03d ms. I'm preparing pizza %d\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type);

        pizzop(semph, SEMOVEN, -1);
        pizzop(semph, SEMOVENWIN, -1);
        index = findfree(oven, OVENSIZE);
        oven[index] = type;
        pizzop(semph, SEMOVENWIN, 1);

        gettimeofday(&tv, NULL);
        printf("_P_ My PID is %d. Time is %ld%03d ms. I added pizza %d; there are %d in the oven\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type, OVENSIZE - semctl(semph, SEMOVEN, GETVAL));
        usleep(4e6 + rand1e6);

        pizzop(semph, SEMOVENWIN, -1);
        type = oven[index];
        oven[index] = -1;
        pizzop(semph, SEMOVENWIN, 1);
        pizzop(semph, SEMOVEN, 1);

        pizzop(semph, SEMTABLE, -1);
        pizzop(semph, SEMTABLWIN, -1);
        pizzop(semph, SEMPIZZRDY, 1);
        index = findfree(table, TABLESIZE);
        table[index] = type;
        pizzop(semph, SEMTABLWIN, 1);

        gettimeofday(&tv, NULL);
        printf("__P My PID is %d. Time is %ld%03d ms. I finished pizza %d; there are %d in the oven; there are %d on the table\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type, 
            OVENSIZE - semctl(semph, SEMOVEN, GETVAL), 
            TABLESIZE - semctl(semph, SEMTABLE, GETVAL)
        );
    }
}
