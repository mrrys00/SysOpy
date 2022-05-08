#include "config.h"

int oven_id, table_id, *oven_memory, *table_memory;
sem_t *oven, *table, *inoven, *intable, *finished;

void safe_exit(int signo)
{
    sem_close(oven);
    sem_close(table);
    sem_close(inoven);
    sem_close(intable);
    sem_close(finished);
    munmap(oven_memory, OVENCAPACITY * sizeof(int));
    munmap(table_memory, TABLECAPACITY * sizeof(int));
    exit(EXIT_SUCCESS);
}

int action_with_pizza(sem_t *sem, int act)
{
    if (act == -1)
        return sem_wait(sem);
    else if (act == 1)
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

int semctrl(sem_t *semaph)
{
    int val;
    sem_getvalue(semaph, &val);
    return val;
}

int main()
{
    int idx, pizza_type;
    srand(time(NULL));
    signal(SIGINT, safe_exit);

    table = sem_open(TABLSEMAPHORE, O_RDWR);
    oven = sem_open(OVENSEMAPHORE, O_RDWR);
    intable = sem_open(ONTASEMAPHORE, O_RDWR);
    inoven = sem_open(INOVSEMAPHORE, O_RDWR);
    finished = sem_open(FINISEMAPHORE, O_RDWR);

    oven_id = shm_open(OVENMEMORY, O_RDWR, 0);
    table_id = shm_open(TABLMEMORY, O_RDWR, 0);
    ftruncate(oven_id, OVENCAPACITY * sizeof(int));
    ftruncate(table_id, TABLECAPACITY * sizeof(int));
    oven_memory = mmap(NULL, OVENCAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, oven_id, 0);
    table_memory = mmap(NULL, TABLECAPACITY * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, table_id, 0);

    while (1)
    {
        pizza_type = rand() % 10;
        usleep(1e6 + 1000 * (rand() % 1000));

        struct timeval te;
        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Przygotowuje pizze: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type);

        action_with_pizza(oven, -1);
        action_with_pizza(inoven, -1);
        idx = mem_search(oven_memory, OVENCAPACITY);
        oven_memory[idx] = pizza_type;
        action_with_pizza(inoven, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Dodałem pizze: %d\t Liczba pizz w piecu: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type, OVENCAPACITY - semctrl(oven));
        usleep(4e6 + 1000 * (rand() % 1000));

        action_with_pizza(inoven, -1);
        pizza_type = oven_memory[idx];
        oven_memory[idx] = -1;
        action_with_pizza(inoven, 1);
        action_with_pizza(oven, 1);

        action_with_pizza(table, -1);
        action_with_pizza(intable, -1);
        action_with_pizza(finished, 1);
        idx = mem_search(table_memory, TABLECAPACITY);
        table_memory[idx] = pizza_type;
        action_with_pizza(intable, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Wyjmuję pizze:  %d\t Liczba pizz w piecu: %d\t Liczba pizz na stole:  %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type, OVENCAPACITY - semctrl(oven), TABLECAPACITY - semctrl(table));
    }
}
