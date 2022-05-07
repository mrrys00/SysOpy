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
#define OVENCAPACITY 5
#define TABLECAPACITY 5

#define SEMOVEN "/oven"
#define SEMTABLE "/table"
#define SEMOVENWIN "/ovenwin"
#define SEMTABLWIN "/tablwin"
#define SEMPIZZRDY "/pizzrdy"

#define OVENMEMORY "/memoven"
#define TABLMEMORY "/memtable"

#define PIZZAPATH "cook"
#define DELIVPATH "deliverer"

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
