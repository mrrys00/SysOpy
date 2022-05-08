#include "config.h"

int oven_id, table_id;
int *oven_memory, *table_memory;
sem_t *oven, *table, *inoven, *intable, *finished;

void safe_exit(int signum)
{
    sem_close(oven);
    sem_close(table);
    sem_close(inoven);
    sem_close(intable);
    sem_close(finished);
    sem_unlink(OVENSEMAPHORE);
    sem_unlink(TABLSEMAPHORE);
    sem_unlink(INOVSEMAPHORE);
    sem_unlink(ONTASEMAPHORE);
    sem_unlink(FINISEMAPHORE);
    munmap(oven_memory, OVENCAPACITY * sizeof(int));
    munmap(table_memory, TABLECAPACITY * sizeof(int));
    shm_unlink(OVENMEMORY);
    shm_unlink(TABLMEMORY);
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
    pid_t pid;

    table = sem_open(TABLSEMAPHORE, O_CREAT | O_RDWR, 0777, TABLECAPACITY);
    oven = sem_open(OVENSEMAPHORE, O_CREAT | O_RDWR, 0777, OVENCAPACITY);

    intable = sem_open(ONTASEMAPHORE, O_CREAT | O_RDWR, 0777, 1);
    inoven = sem_open(INOVSEMAPHORE, O_CREAT | O_RDWR, 0777, 1);
    finished = sem_open(FINISEMAPHORE, O_CREAT | O_RDWR, 0777, 0);

    oven_id = shm_open(OVENMEMORY, O_CREAT | O_RDWR, 0777);
    table_id = shm_open(TABLMEMORY, O_CREAT | O_RDWR, 0777);
    ftruncate(oven_id, OVENCAPACITY * sizeof(int));
    ftruncate(table_id, TABLECAPACITY * sizeof(int));
    oven_memory = mmap(NULL, OVENCAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, oven_id, 0);
    table_memory = mmap(NULL, TABLECAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, table_id, 0);


    for (int i = 0; i < OVENCAPACITY; i++)
        oven_memory[i] = -1;

    for (int i = 0; i < TABLECAPACITY; i++)
        table_memory[i] = -1;

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
