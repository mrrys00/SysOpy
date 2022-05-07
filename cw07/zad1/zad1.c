#include "config.h"

int oven_id, table_id, semph, *oven, *table;

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
    arg.val = TABLECAPACITY;
    semctl(semid, SEMTABLE, SETVAL, arg);
    arg.val = OVENCAPACITY;
    semctl(semid, SEMOVEN, SETVAL, arg);
    arg.val = 1;
    semctl(semid, SEMTABLWIN, SETVAL, arg);
    semctl(semid, SEMOVENWIN, SETVAL, arg);
    arg.val = 0;
    semctl(semid, SEMPIZZRDY, SETVAL, arg);
    return;
}

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    signal(SIGINT, forcekill);
    signal(SIGSEGV, forcekill);

    oven_id = shmget(ftok(KEYPATH, PROJIDOVEN), OVENCAPACITY * sizeof(int), IPC_CREAT | 0777);
    table_id = shmget(ftok(KEYPATH, PROJIDTABLE), TABLECAPACITY * sizeof(int), IPC_CREAT | 0777);
    oven = shmat(oven_id, NULL, 0);
    table = shmat(table_id, NULL, 0);
    semph = semget(ftok(KEYPATH, PROJIDSEMA), 5, IPC_CREAT | 0777);

    setinit(semph);
    for (int i = 0; i < OVENCAPACITY; i++)
        oven[i] = -1;
        
    for (int i = 0; i < TABLECAPACITY; i++)
        table[i] = -1;

    for (int i = 0; i < atoi(args[1]); i++)
    {
        pid = fork();
        if (pid == 0)
            execl(PIZZAPATH, PIZZAPATH, NULL);
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
