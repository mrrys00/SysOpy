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
        if (array[i] == -1)
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
        type = rand() % 10;
        usleep(1e6 + rand1e6);

        struct timeval te;
        gettimeofday(&te, NULL);
        printf("prepare - stage 1. pid %d\t timestamp %ld%03d ms\t I'm preparing pizza %d\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type);

        action_with_pizza(semph, SEMOVEN, -1);
        action_with_pizza(semph, SEMOVENWIN, -1);
        index = findfree(oven, OVENSIZE);
        oven[index] = type;
        action_with_pizza(semph, SEMOVENWIN, 1);

        gettimeofday(&te, NULL);
        printf("prepare - stage 2. pid %d\t timestamp %ld%03d ms\t I added pizza %d; there are %d in the oven\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, OVENSIZE - semctl(semph, SEMOVEN, GETVAL));
        usleep(4e6 + rand1e6);

        action_with_pizza(semph, SEMOVENWIN, -1);
        type = oven[index];
        oven[index] = -1;
        action_with_pizza(semph, SEMOVENWIN, 1);
        action_with_pizza(semph, SEMOVEN, 1);

        action_with_pizza(semph, SEMTABLE, -1);
        action_with_pizza(semph, SEMTABLWIN, -1);
        action_with_pizza(semph, SEMPIZZRDY, 1);
        index = findfree(table, TABLESIZE);
        table[index] = type;
        action_with_pizza(semph, SEMTABLWIN, 1);

        gettimeofday(&te, NULL);
        printf("prepare - stage 3. pid %d\t timestamp %ld%03d ms\t I finished pizza %d; there are %d in the oven; there are %d on the table\n", getpid(), te.tv_sec, (int)(te.tv_usec / 1e3), type, OVENSIZE - semctl(semph, SEMOVEN, GETVAL), TABLESIZE - semctl(semph, SEMTABLE, GETVAL));
    }
}
