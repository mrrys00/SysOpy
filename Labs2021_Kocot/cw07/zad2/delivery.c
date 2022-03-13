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
#include <sys/mman.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include "consts.h"

int ovenid, tableid;
int *ovenmem, *tablemem;
sem_t *oven, *table, *ovenwin, *tablwin, *pizzrdy;

int pizzop(sem_t* sem, int delta) {  // operation on pizza
    if(delta < 0) return sem_wait(sem);
    else if(delta > 0) return sem_post(sem);
    else return 0;
}

int findfree(int* array, size_t size) {
    for(int i = 0; i < size; i++) {
        if(array[i] != -1) return i;
    }
    return -1;
}

void fayrant(int signo) {
    sem_close(oven);
    sem_close(table);
    sem_close(ovenwin);
    sem_close(tablwin);
    sem_close(pizzrdy);
    munmap(ovenmem, OVENSIZE * sizeof(int));
    munmap(tablemem, TABLESIZE * sizeof(int));
    exit(0);
}

int semctrl(sem_t* sem) {
    int a;
    sem_getvalue(sem, &a);
    return a;
}

int main(int argc, char * argv[]) {
    signal(SIGINT, fayrant);

    table = sem_open(SEMTABLE, O_RDWR);
    oven = sem_open(SEMOVEN, O_RDWR);
    tablwin = sem_open(SEMTABLWIN, O_RDWR);
    ovenwin = sem_open(SEMOVENWIN, O_RDWR);
    pizzrdy = sem_open(SEMPIZZRDY, O_RDWR);

    ovenid = shm_open(MEMOVEN, O_RDWR, 0);
    tableid = shm_open(MEMTABLE, O_RDWR, 0);
    ftruncate(ovenid, OVENSIZE * sizeof(int));
    ftruncate(tableid, TABLESIZE * sizeof(int));
    ovenmem = mmap(NULL, OVENSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, ovenid, 0);
    tablemem = mmap(NULL, TABLESIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tableid, 0);
    
    int index, type;

    srand(time(NULL));

    while(true) {
        usleep(1e6 + rand1e6);

        struct timeval tv;

        pizzop(pizzrdy, -1);
        pizzop(tablwin, -1);
        pizzop(table, 1);
        index = findfree(tablemem, TABLESIZE);
        type = tablemem[index];
        tablemem[index] = -1;
        pizzop(tablwin, 1);

        gettimeofday(&tv, NULL);
        printf("D_  My PID is %d. Time is %ld%03d ms. I'm taking pizza %d; there are %d on the table\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type, TABLESIZE - semctrl(table));
        usleep(4e6 + rand1e6);

        gettimeofday(&tv, NULL);
        printf("_D  My PID is %d. Time is %ld%03d ms. I delivered pizza %d\n", 
            getpid(), tv.tv_sec, (int) (tv.tv_usec / 1e3), type);
        usleep(4e6 + rand1e6);
    }
}
