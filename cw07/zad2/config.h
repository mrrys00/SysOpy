#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <time.h>
#include <semaphore.h>

#ifndef CONFIG_H
#define CONFIG_H

#define KEYPATH getenv("HOME")
#define PIZZAPATH "cook"
#define DELIVPATH "deliverer"
#define OVENCAPACITY 5
#define TABLECAPACITY 5

#define OVENSEMAPHORE "/oven"
#define TABLSEMAPHORE "/table"
#define INOVSEMAPHORE "/inoven"
#define ONTASEMAPHORE "/intabl"
#define FINISEMAPHORE "/finish"

#define OVENMEMORY "/ovenmemory"
#define TABLMEMORY "/tablememory"

// "man 2 semctl"
typedef union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                                         (Linux-specific) */
} semun;

typedef struct sembuf sembuf;

#endif
