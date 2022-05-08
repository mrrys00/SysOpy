#include "config.h"

int oven_id, table_id, semph, *oven, *table;

void safe_exit(int signo)
{
    shmdt(oven);
    shmdt(table);
    exit(EXIT_SUCCESS);
}

void action_with_pizza(int semid, short semnum, int delta)
{
    sembuf s;
    s.sem_num = semnum;
    s.sem_op = delta;
    s.sem_flg = 0;
    semop(semid, &s, 1);
    return;
}

int mem_search(int *list, size_t size)
{
    for (int i = 0; i < size; i++)
        if (list[i] == -1)
            return i;
    return -1;
}

int main()
{
    int idx, pizza_type;
    srand(time(NULL));
    signal(SIGINT, safe_exit);

    oven_id = shmget(ftok(KEYPATH, PROJIDOVEN), 0, 0);
    table_id = shmget(ftok(KEYPATH, PROJIDTABLE), 0, 0);
    oven = shmat(oven_id, NULL, 0);
    table = shmat(table_id, NULL, 0);
    semph = semget(ftok(KEYPATH, PROJIDSEMA), 0, 0);

    while (1)
    {
        pizza_type = rand() % 10;
        usleep(1e6 + 1000 * (rand() % 1000));

        struct timeval te;
        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Przygotowuje pizze: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type);

        action_with_pizza(semph, OVENSEMAPHORE, -1);
        action_with_pizza(semph, INOVSEMAPHORE, -1);
        idx = mem_search(oven, OVENCAPACITY);
        oven[idx] = pizza_type;
        action_with_pizza(semph, INOVSEMAPHORE, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Dodałem pizze: %d\t Liczba pizz w piecu: %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type, OVENCAPACITY - semctl(semph, OVENSEMAPHORE, GETVAL));
        usleep(4e6 + 1000 * (rand() % 1000));

        action_with_pizza(semph, INOVSEMAPHORE, -1);
        pizza_type = oven[idx];
        oven[idx] = -1;
        action_with_pizza(semph, INOVSEMAPHORE, 1);
        action_with_pizza(semph, OVENSEMAPHORE, 1);

        action_with_pizza(semph, TABLSEMAPHORE, -1);
        action_with_pizza(semph, ONTASEMAPHORE, -1);
        action_with_pizza(semph, FINISEMAPHORE, 1);
        idx = mem_search(table, TABLECAPACITY);
        table[idx] = pizza_type;
        action_with_pizza(semph, ONTASEMAPHORE, 1);

        gettimeofday(&te, NULL);
        printf("pid %d\t timestamp %ld%03d \t Wyjmuję pizze:  %d\t Liczba pizz w piecu: %d\t Liczba pizz na stole:  %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), pizza_type, OVENCAPACITY - semctl(semph, OVENSEMAPHORE, GETVAL), TABLECAPACITY - semctl(semph, TABLSEMAPHORE, GETVAL));
    }
}
