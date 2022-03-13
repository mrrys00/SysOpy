#define bool short
#define true 1
#define false 0
#define HOME getenv("HOME")
#define OVENSIZE 5
#define TABLESIZE 5
#define PIZZNUM 3  // number of pizzaiolos
#define DELIVNUM 3  // number of deliverers

#define SEMOVEN "/oven"
#define SEMTABLE "/table"
#define SEMOVENWIN "/ovenwin"
#define SEMTABLWIN "/tablwin"
#define SEMPIZZRDY "/pizzrdy"

#define MEMOVEN "/memoven"
#define MEMTABLE "/memtable"

#define PIZZPATH "pizzaiolo"
#define DELIVPATH "delivery"

#define rand1e6 1000 * (rand() % 1000)

typedef union semun {  // taken from "man 2 semctl"
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
} semun;

typedef struct sembuf sembuf;
