#include "config.h"

int oven_id, table_id;
int *ovenmem, *tablemem;
sem_t *oven, *table, *ovenwin, *tablwin, *pizzrdy;

void safe_exit(int signum)
{
    sem_close(oven);
    sem_close(table);
    sem_close(ovenwin);
    sem_close(tablwin);
    sem_close(pizzrdy);
    sem_unlink(SEMOVEN);
    sem_unlink(SEMTABLE);
    sem_unlink(SEMOVENWIN);
    sem_unlink(SEMTABLWIN);
    sem_unlink(SEMPIZZRDY);
    munmap(ovenmem, OVENSIZE * sizeof(int));
    munmap(tablemem, TABLESIZE * sizeof(int));
    shm_unlink(MEMOVEN);
    shm_unlink(MEMTABLE);
    exit(EXIT_SUCCESS);
}

int semctrl(sem_t *sem)
{
    int a;
    sem_getvalue(sem, &a);
    return a;
}

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, safe_exit);

    table = sem_open(SEMTABLE, O_CREAT | O_RDWR, 0777, TABLESIZE);
    oven = sem_open(SEMOVEN, O_CREAT | O_RDWR, 0777, OVENSIZE);

    tablwin = sem_open(SEMTABLWIN, O_CREAT | O_RDWR, 0777, 1);
    ovenwin = sem_open(SEMOVENWIN, O_CREAT | O_RDWR, 0777, 1);
    pizzrdy = sem_open(SEMPIZZRDY, O_CREAT | O_RDWR, 0777, 0);

    oven_id = shm_open(MEMOVEN, O_CREAT | O_RDWR, 0777);
    table_id = shm_open(MEMTABLE, O_CREAT | O_RDWR, 0777);
    ftruncate(oven_id, OVENSIZE * sizeof(int));
    ftruncate(table_id, TABLESIZE * sizeof(int));
    ovenmem = mmap(NULL, OVENSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, oven_id, 0);
    tablemem = mmap(NULL, TABLESIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, table_id, 0);

    pid_t pid;

    for (int i = 0; i < OVENSIZE; i++)
        ovenmem[i] = -1;
    for (int i = 0; i < TABLESIZE; i++)
        tablemem[i] = -1;
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
