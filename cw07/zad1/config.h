#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#ifndef CONFIG_H
#define CONFIG_H

#define KEYPATH getenv("HOME")
#define PIZZAPATH "cook"
#define DELIVPATH "deliverer"
#define OVENCAPACITY 5
#define TABLECAPACITY 5

#define OVENSEMAPHORE 0
#define TABLSEMAPHORE 1
#define INOVSEMAPHORE 2
#define ONTASEMAPHORE 3
#define FINISEMAPHORE 4

#define PROJIDOVEN 1
#define PROJIDTABLE 2
#define PROJIDSEMA 3

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
