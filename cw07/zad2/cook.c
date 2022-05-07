#include "config.h"

int oven_id, table_id, *oven_memory, *table_memory;
sem_t *oven, *table, *ovenwin, *tablwin, *pizzrdy;

void safe_exit(int signo)
{
    sem_close(oven);
    sem_close(table);
    sem_close(ovenwin);
    sem_close(tablwin);
    sem_close(pizzrdy);
    munmap(oven_memory, OVENCAPACITY * sizeof(int));
    munmap(table_memory, TABLECAPACITY * sizeof(int));
    exit(EXIT_SUCCESS);
}

int action_with_pizza(sem_t *sem, int delta)
{
    if (delta < 0)
        return sem_wait(sem);
    else if (delta > 0)
        return sem_post(sem);
    return 0;
}

int mem_search(int *array, size_t size)
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

int main()
{
    signal(SIGINT, safe_exit);

    table = sem_open(SEMTABLE, O_RDWR);
    oven = sem_open(SEMOVEN, O_RDWR);
    tablwin = sem_open(SEMTABLWIN, O_RDWR);
    ovenwin = sem_open(SEMOVENWIN, O_RDWR);
    pizzrdy = sem_open(SEMPIZZRDY, O_RDWR);

    oven_id = shm_open(OVENMEMORY, O_RDWR, 0);
    table_id = shm_open(TABLMEMORY, O_RDWR, 0);
    ftruncate(oven_id, OVENCAPACITY * sizeof(int));
    ftruncate(table_id, TABLECAPACITY * sizeof(int));
    oven_memory = mmap(NULL, OVENCAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, oven_id, 0);
    table_memory = mmap(NULL, TABLECAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, table_id, 0);

    int idx, type;

    srand(time(NULL));

    while (1)
    {
        type = rand() % 10;
        usleep(1e6 + 1000 * (rand() % 1000));

        struct timeval te;
        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Przygotowuje pizze: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type);

        action_with_pizza(oven, -1);
        action_with_pizza(ovenwin, -1);
        idx = mem_search(oven_memory, OVENCAPACITY);
        oven_memory[idx] = type;
        action_with_pizza(ovenwin, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Dodałem pizze: %d\t Liczba pizz w piecu: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, OVENCAPACITY - semctrl(oven));
        usleep(4e6 + 1000 * (rand() % 1000));

        action_with_pizza(ovenwin, -1);
        type = oven_memory[idx];
        oven_memory[idx] = -1;
        action_with_pizza(ovenwin, 1);
        action_with_pizza(oven, 1);

        action_with_pizza(table, -1);
        action_with_pizza(tablwin, -1);
        action_with_pizza(pizzrdy, 1);
        idx = mem_search(table_memory, TABLECAPACITY);
        table_memory[idx] = type;
        action_with_pizza(tablwin, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Wyjmuję pizze:  %d\t Liczba pizz w piecu: %d\t Liczba pizz na stole:  %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, OVENCAPACITY - semctrl(oven), TABLECAPACITY - semctrl(table));
    }
}
