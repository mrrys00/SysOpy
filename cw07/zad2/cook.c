#include "config.h"

int oven_id, table_id;
int *ovenmem, *tablemem;
sem_t *oven, *table, *ovenwin, *tablwin, *pizzrdy;

int action_with_pizza(sem_t *sem, int delta)
{
    if (delta < 0)
        return sem_wait(sem);
    else if (delta > 0)
        return sem_post(sem);
    else
        return 0;
}

int findfree(int *array, size_t size)
{
    for (int i = 0; i < size; i++)
        if (array[i] == -1)
            return i;
    return -1;
}

int semctrl(sem_t *sem)
{
    int a;
    sem_getvalue(sem, &a);
    return a;
}

void safe_exit(int signo)
{
    sem_close(oven);
    sem_close(table);
    sem_close(ovenwin);
    sem_close(tablwin);
    sem_close(pizzrdy);
    munmap(ovenmem, OVENSIZE * sizeof(int));
    munmap(tablemem, TABLESIZE * sizeof(int));
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGINT, safe_exit);

    table = sem_open(SEMTABLE, O_RDWR);
    oven = sem_open(SEMOVEN, O_RDWR);
    tablwin = sem_open(SEMTABLWIN, O_RDWR);
    ovenwin = sem_open(SEMOVENWIN, O_RDWR);
    pizzrdy = sem_open(SEMPIZZRDY, O_RDWR);

    oven_id = shm_open(MEMOVEN, O_RDWR, 0);
    table_id = shm_open(MEMTABLE, O_RDWR, 0);
    ftruncate(oven_id, OVENSIZE * sizeof(int));
    ftruncate(table_id, TABLESIZE * sizeof(int));
    ovenmem = mmap(NULL, OVENSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, oven_id, 0);
    tablemem = mmap(NULL, TABLESIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, table_id, 0);

    int index, type;

    srand(time(NULL));

    while (1)
    {
        type = rand() % 10;
        usleep(1e6 + rand1e6);

        struct timeval te;
        gettimeofday(&te, NULL);
        printf("prepare - stage 1. pid %d\t timestamp %ld%03d ms\t I'm preparing pizza %d\n",
               getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type);

        action_with_pizza(oven, -1);
        action_with_pizza(ovenwin, -1);
        index = findfree(ovenmem, OVENSIZE);
        ovenmem[index] = type;
        action_with_pizza(ovenwin, 1);

        gettimeofday(&te, NULL);
        printf("prepare - stage 2. pid %d\t timestamp %ld%03d ms\t I added pizza %d; there are %d in the oven\n",
               getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, OVENSIZE - semctrl(oven));
        usleep(4e6 + rand1e6);

        action_with_pizza(ovenwin, -1);
        type = ovenmem[index];
        ovenmem[index] = -1;
        action_with_pizza(ovenwin, 1);
        action_with_pizza(oven, 1);

        action_with_pizza(table, -1);
        action_with_pizza(tablwin, -1);
        action_with_pizza(pizzrdy, 1);
        index = findfree(tablemem, TABLESIZE);
        tablemem[index] = type;
        action_with_pizza(tablwin, 1);

        gettimeofday(&te, NULL);
        printf("prepare - stage 3. pid %d\t timestamp %ld%03d ms\t I finished pizza %d; there are %d in the oven; there are %d on the table\n",
               getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type,
               OVENSIZE - semctrl(oven),
               TABLESIZE - semctrl(table));
    }
}
