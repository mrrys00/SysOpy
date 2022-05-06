#include "config.h"

int oven_id, table_id, semph;
int *oven, *table;

void action_with_pizza(int semid, short semnum, int delta)
{
    sembuf s;
    s.sem_num = semnum;
    s.sem_op = delta;
    s.sem_flg = 0;
    semop(semid, &s, 1);
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
    shmdt(oven);
    shmdt(table);
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGINT, safe_exit);

    oven_id = shmget(ftok(HOME, PROJOVEN), 0, 0);
    table_id = shmget(ftok(HOME, PROJTABLE), 0, 0);
    oven = shmat(oven_id, NULL, 0);
    table = shmat(table_id, NULL, 0);
    semph = semget(ftok(HOME, PROJSEM), 0, 0);
    int index, type;

    srand(time(NULL));

    while (1)
    {
        usleep(1e6 + rand1e6);

        struct timeval te;

        action_with_pizza(semph, SEMPIZZRDY, -1);
        action_with_pizza(semph, SEMTABLWIN, -1);
        action_with_pizza(semph, SEMTABLE, 1);
        index = findfree(table, TABLESIZE);
        type = table[index];
        table[index] = -1;
        action_with_pizza(semph, SEMTABLWIN, 1);

        gettimeofday(&te, NULL);
        printf("delivery - stage 1.  pid %d\t timestamp %ld%03d ms\t I'm taking pizza %d; there are %d on the table\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, TABLESIZE - semctl(semph, SEMTABLE, GETVAL));
        usleep(4e6 + rand1e6);

        gettimeofday(&te, NULL);
        printf("delivery - stage 2.  pid %d\t timestamp %ld%03d ms\t I delivered pizza %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type);
        usleep(4e6 + rand1e6);
    }
}
