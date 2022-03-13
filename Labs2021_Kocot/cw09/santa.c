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
#include <pthread.h>
#include "consts.h"

pthread_t thrds[20];
int IDs[] = {0,1,2,3,4,5,6,7,8,9};
int waitnum = 0, returned = 0, waiting[3];
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ret_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t quit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond   = PTHREAD_COND_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t solved = PTHREAD_COND_INITIALIZER;
pthread_cond_t gifts = PTHREAD_COND_INITIALIZER;
pthread_cond_t quit = PTHREAD_COND_INITIALIZER;

void fayrant(int signo) {
    if(signo == SIGINT) {
        for(int i = 0; i < 20; i++) {
            pthread_cancel(thrds[i]);
        }
        pthread_cond_broadcast(&quit);
    }
}

void randsleep(double from, double to) {
    double range = to - from;
    int us = (rand() % 1000) * 1000;
    usleep(us * range + from * 1000000);
}

void* santa_routine(void* varg) {
    while(true) {
        pthread_mutex_lock(&santa_mutex);
        pthread_cond_wait(&santa_cond, &santa_mutex);
        pthread_mutex_unlock(&santa_mutex);

        pthread_mutex_lock(&ret_mutex);
        if(returned == 9) {
            printf("Santa: delivering presents\n");
            randsleep(2.0, 4.0);
            returned = 0;
        }
        pthread_mutex_unlock(&ret_mutex);
        pthread_cond_broadcast(&gifts);

        pthread_mutex_lock(&wait_mutex);
        if(waitnum == 3) {
            printf("Santa: solving problems of %d, %d and %d\n", waiting[0], waiting[1], waiting[2]);
            randsleep(1.0, 2.0);
            waitnum = 0;
        }
        pthread_mutex_unlock(&wait_mutex);
        pthread_cond_broadcast(&solved);
    }
}

void* elf_routine(void* varg) {
    int id = *((int*) varg);
    while(true) {
        randsleep(2.0, 5.0);
        pthread_mutex_lock(&wait_mutex);
        while(waitnum >= 3) {
            printf("Elf %d: waiting for others' problems to be solved\n", id);
            pthread_cond_wait(&solved, &wait_mutex);
        }
        waiting[waitnum++] = id;
        printf("Elf %d: %d elves are waiting\n", id, waitnum);
        if(waitnum == 3) {
            printf("Elf %d: Waking santa up\n", id);
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_cond_wait(&solved, &wait_mutex);
        pthread_mutex_unlock(&wait_mutex);
    }
}

void* raindeer_routine(void* varg) {
    int id = *((int*) varg);
    while(true) {
        randsleep(5.0, 10.0);
        pthread_mutex_lock(&ret_mutex);
        returned++;
        printf("Raindeer %d: %d raindeers are waiting\n", id, returned);
        if(waitnum == 9) {
            printf("Raindeer %d: Waking santa up\n", id);
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_cond_wait(&gifts, &ret_mutex);
        pthread_mutex_unlock(&ret_mutex);
    }
}

int main(int argc, char * argv[]) {
    srand(time(0));
    signal(SIGINT, fayrant);
    pthread_t *santa = thrds, *elf = &thrds[1], *raindeer = &thrds[11];

    pthread_create(santa, NULL, santa_routine, NULL);
    for(int i = 0; i < 10; i++) {
        pthread_create(&elf[i], NULL, elf_routine, (void*) &IDs[i]);
    }
    for(int i = 0; i < 9; i++) {
        pthread_create(&raindeer[i], NULL, raindeer_routine, (void*) &IDs[i]);
    }

    pthread_cond_wait(&quit, &quit_mutex);
}

