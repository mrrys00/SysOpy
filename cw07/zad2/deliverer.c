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
        if (array[i] != -1)
            return i;
    return -1;
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
    exit(0);
}

int semctrl(sem_t *sem)
{
    int a;
    sem_getvalue(sem, &a);
    return a;
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
        usleep(1e6 + rand1e6);

        struct timeval te;

        action_with_pizza(pizzrdy, -1);
        action_with_pizza(tablwin, -1);
        action_with_pizza(table, 1);
        index = findfree(tablemem, TABLESIZE);
        type = tablemem[index];
        tablemem[index] = -1;
        action_with_pizza(tablwin, 1);

        gettimeofday(&te, NULL);
        printf("delivery - stage 1.  pid %d\t timestamp %ld%03d ms\t I'm taking pizza %d; there are %d on the table\n",
               getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, TABLESIZE - semctrl(table));
        usleep(4e6 + rand1e6);

        gettimeofday(&te, NULL);
        printf("delivery - stage 2.  pid %d\t timestamp %ld%03d ms\t I delivered pizza %d\n",
               getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type);
        usleep(4e6 + rand1e6);
    }
}
