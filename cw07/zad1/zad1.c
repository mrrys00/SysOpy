#include "config.h"

int oven_id, table_id, semph;
int *oven, *table;

void forcekill(int signum)
{
    semctl(semph, 0, IPC_RMID);
    shmctl(oven_id, IPC_RMID, NULL);
    shmctl(table_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void setinit(int semid)
{
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

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, forcekill);
    signal(SIGSEGV, forcekill);

    oven_id = shmget(ftok(HOME, PROJOVEN), OVENSIZE * sizeof(int), IPC_CREAT | 0777);
    table_id = shmget(ftok(HOME, PROJTABLE), TABLESIZE * sizeof(int), IPC_CREAT | 0777);
    oven = shmat(oven_id, NULL, 0);
    table = shmat(table_id, NULL, 0);
    semph = semget(ftok(HOME, PROJSEM), 5, IPC_CREAT | 0777);
    pid_t pid;

    setinit(semph);
    for (int i = 0; i < OVENSIZE; i++)
        oven[i] = -1;
    for (int i = 0; i < TABLESIZE; i++)
        table[i] = -1;

    for (int i = 0; i < atoi(args[1]); i++)
    {
        pid = fork();
        if (pid == 0)
            execl(PIZZPATH, PIZZPATH, NULL);
        sleep(1);
    }
    for (int i = 0; i < atoi(args[2]); i++)
    {
        pid = fork();
        if (pid == 0)
            execl(DELIVPATH, DELIVPATH, NULL);
        sleep(1);
    }

    while (1)
        usleep(2e6);
}
