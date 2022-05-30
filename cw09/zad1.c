#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

int waiting_elfs_cnt = 0, in_office_raindeers = 0, waiting_elfs_pids[3], actors_num = 20;
pthread_t threads[20]; // mikołaj + 9 reniferów + 10 elfów

pthread_mutex_t waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t return_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_claus_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_claus_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_problem_solved = PTHREAD_COND_INITIALIZER;
pthread_cond_t give_gifts = PTHREAD_COND_INITIALIZER;
pthread_cond_t exitt = PTHREAD_COND_INITIALIZER;

void sweet_dreams(double mini, double maxi)
{
    // randomized sleep time in µseconds
    usleep(((rand() % 1000) * 1000) * (maxi - mini) + mini * 1000000);
    return;
}

void *elf_thread(void *varg)
{
    int id = *((int *)varg);
    while (1)
    {
        // Pracują przez losowy okres czasu (2-5s).
        sweet_dreams(2.0, 5.0);
        // Chcą zgłosić problem
        pthread_mutex_lock(&waiting_mutex);
        if (waiting_elfs_cnt < 3)
            printf("Elf: czeka na powrót elfów, %d\n", id);
        // Mikołaj się z nimi spotyka.
        while (waiting_elfs_cnt >= 3)
        {
            printf("Elf: Mikołaj rozwiązuje problem, %d\n", id);
            pthread_cond_wait(&elf_problem_solved, &waiting_mutex);
        }

        waiting_elfs_pids[waiting_elfs_cnt++] = id;
        printf("Elf: czeka %d elfów na Mikołaja, %d\n", waiting_elfs_cnt, id);

        // Jeśli jest trzecim elfem przed warsztatem to wybudza Mikołaja
        if (waiting_elfs_cnt == 3)
        {
            printf("Elf: wybudzam Mikołaja, %d\n", id);
            pthread_cond_broadcast(&santa_claus_cond);
        }
        pthread_cond_wait(&elf_problem_solved, &waiting_mutex);
        // Wraca do pracy
        pthread_mutex_unlock(&waiting_mutex);
    }
}

void *raindeer_thread(void *varg)
{
    int id = *((int *)varg);
    while (1)
    {
        // Są na wakacjach w ciepłych krajach losowy okres czasu (5-10s)
        sweet_dreams(5.0, 10.0);
        // Wracaja na biegun północny
        pthread_mutex_lock(&return_mutex);
        in_office_raindeers++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", id, in_office_raindeers);
        if (in_office_raindeers == 9)
        {
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            // Dostarczają zabawki grzecznym dzieciom
            pthread_cond_broadcast(&santa_claus_cond);
            in_office_raindeers = 0;
        }
        pthread_cond_wait(&give_gifts, &return_mutex);
        // Lecą na wakacje
        pthread_mutex_unlock(&return_mutex);
    }
}

void *santa_claus_thread(void *varg)
{
    while (1)
    {
        // schemat z UPEL:
        // 1. blokujemy dostęp do zasobów współdzielonych
        pthread_mutex_lock(&santa_claus_mutex);
        // 2. operujemy na współdzielonym obiekcie
        pthread_cond_wait(&santa_claus_cond, &santa_claus_mutex);
        // 3. odbkolowujemy
        pthread_mutex_unlock(&santa_claus_mutex);

        pthread_mutex_lock(&return_mutex);
        if (in_office_raindeers == 9)
        {
            printf("Mikołaj: budzę się\n");
            printf("Mikołaj: dostarczam zabawki\n");
            // Dostarczają zabawki grzecznym dzieciom
            sweet_dreams(2.0, 4.0);
            in_office_raindeers = 0;
            printf("Mikołaj: zasypiam\n");
        }
        pthread_mutex_unlock(&return_mutex);
        pthread_cond_broadcast(&give_gifts);

        pthread_mutex_lock(&waiting_mutex);
        if (waiting_elfs_cnt == 3)
        {
            printf("Mikołaj: budzę się\n");
            printf("Mikołaj: rozwiązuje problemy elfów %d %d %d ID\n", waiting_elfs_pids[0], waiting_elfs_pids[1], waiting_elfs_pids[2]);
            sweet_dreams(1.0, 2.0);
            waiting_elfs_cnt = 0;
            printf("Mikołaj: zasypiam\n");
        }
        pthread_mutex_unlock(&waiting_mutex);
        pthread_cond_broadcast(&elf_problem_solved);
    }
}

void safe_exit(int signo)
{
    // "You can't have data of your own passed to the signal handler as parameters. Instead you'll have to store your parameters in global variables." ~ https://stackoverflow.com/questions/6970224/providing-passing-argument-to-signal-handler
    if (signo == SIGINT)
    {
        for (int i = 0; i < actors_num; i++)
            pthread_cancel(threads[i]);
        pthread_cond_broadcast(&exitt);
    }
    return;
}


int main()
{
    srand(time(NULL));
    signal(SIGINT, safe_exit);
    // poniżej wskazujemy na konkretne elementy w tablicy 
    pthread_t *santa_claus_ptr = threads, *elf_ptr = &threads[10], *raindeer_ptr = &threads[1];
    int elfs_num = 10, raindeer_num = 9;
    int ids_creator[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // stworzenie mikołaja
    pthread_create(santa_claus_ptr, NULL, santa_claus_thread, NULL);

    // tworzenie elfów
    for (int i = 0; i < elfs_num; i++)
        pthread_create(&elf_ptr[i], NULL, elf_thread, (void *)&ids_creator[i]);

    // tworzenie reniferów
    for (int i = 0; i < raindeer_num; i++)
        pthread_create(&raindeer_ptr[i], NULL, raindeer_thread, (void *)&ids_creator[i]);

    // mutex wyjścia z programu
    pthread_cond_wait(&exitt, &exit_mutex);

    exit(EXIT_SUCCESS);
}
