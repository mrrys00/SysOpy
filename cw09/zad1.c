#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>


pthread_t threads[20];  // mikołaj + 9 reniferów + 10 elfów
int waitnum = 0, returned = 0, waiting[3];

pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ret_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t quit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t solved = PTHREAD_COND_INITIALIZER;
pthread_cond_t gifts = PTHREAD_COND_INITIALIZER;
pthread_cond_t quit = PTHREAD_COND_INITIALIZER;

void safe_exit(int signo)
{
    // "You can't have data of your own passed to the signal handler as parameters. Instead you'll have to store your parameters in global variables." ~ https://stackoverflow.com/questions/6970224/providing-passing-argument-to-signal-handler 
    if (signo == SIGINT)
    {
        for (int i = 0; i < 20; i++)
            pthread_cancel(threads[i]);
        pthread_cond_broadcast(&quit);
    }
    return;
}

void sweet_dreams(double mini, double maxi)
{
    // randomized sleep time in µseconds
    usleep(((rand() % 1000) * 1000) * (maxi - mini) + mini * 1000000);
    return;
}

void *santa_thread(void *varg)
{
    while (1)
    {
        pthread_mutex_lock(&santa_mutex);
        pthread_cond_wait(&santa_cond, &santa_mutex);
        pthread_mutex_unlock(&santa_mutex);

        pthread_mutex_lock(&ret_mutex);
        if (returned == 9)
        {
            printf("Mikołaj: budzę się\n");
            printf("Mikołaj: dostarczam zabawki\n");
            sweet_dreams(2.0, 4.0);
            returned = 0;
            printf("Mikołaj: zasypiam\n");
        }
        pthread_mutex_unlock(&ret_mutex);
        pthread_cond_broadcast(&gifts);

        pthread_mutex_lock(&wait_mutex);
        if (waitnum == 3)
        {
            printf("Mikołaj: budzę się\n");
            printf("Mikołaj: rozwiązuje problemy elfów %d %d %d ID\n", waiting[0], waiting[1], waiting[2]);
            sweet_dreams(1.0, 2.0);
            waitnum = 0;
            printf("Mikołaj: zasypiam\n");
        }
        pthread_mutex_unlock(&wait_mutex);
        pthread_cond_broadcast(&solved);
    }
}

void *elf_thread(void *varg)
{
    int id = *((int *)varg);
    while (1)
    {
        sweet_dreams(2.0, 5.0);
        pthread_mutex_lock(&wait_mutex);
        if (waitnum < 3)
        {
            printf("Elf: czeka na powrót elfów, %d\n", id);
        }
        while (waitnum >= 3)
        {
            printf("Elf: Mikołaj rozwiązuje problem, %d\n", id);
            pthread_cond_wait(&solved, &wait_mutex);
        }
        
        waiting[waitnum++] = id;
        printf("Elf: czeka %d elfów na Mikołaja, %d\n", waitnum, id);
        if (waitnum == 3)
        {
            printf("Elf: wybudzam Mikołaja, %d\n", id);
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_cond_wait(&solved, &wait_mutex);
        pthread_mutex_unlock(&wait_mutex);
    }
}

void *raindeer_thread(void *varg)
{
    int id = *((int *)varg);
    while (1)
    {
        sweet_dreams(5.0, 10.0);
        pthread_mutex_lock(&ret_mutex);
        returned++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", id, returned);
        if (waitnum == 9)
        {
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_cond_wait(&gifts, &ret_mutex);
        pthread_mutex_unlock(&ret_mutex);
    }
}

int main()
{
    srand(time(NULL));
    signal(SIGINT, safe_exit);
    pthread_t *santa = threads, *elf = &threads[1], *raindeer = &threads[11];
    int elfs_num = 10, raindeer_num = 9;
    int IDs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    //stworzenie mikołaja
    pthread_create(santa, NULL, santa_thread, NULL);

    // tworzenie elfów
    for (int i = 0; i < elfs_num; i++)
        pthread_create(&elf[i], NULL, elf_thread, (void *)&IDs[i]);

    // tworzenie reniferów
    for (int i = 0; i < raindeer_num; i++)
        pthread_create(&raindeer[i], NULL, raindeer_thread, (void *)&IDs[i]);

    // mutex wyjścia z programu
    pthread_cond_wait(&quit, &quit_mutex);

    exit(EXIT_SUCCESS);
}
