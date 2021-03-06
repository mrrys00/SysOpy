#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS_NR 9

void *hello(void *arg)
{
    // Napisz funkcję wypisującą komunikat typu Hello  wartość_arg TID: numer_TID wątku
    //  i zwracającą status o wartości równej agrumentowi przesłanemu do funkcji
    int arg_val = *(int *)arg;
    printf("Hello %d TID: %ld\n", arg_val, pthread_self());
    return arg;
    // koniec
}

int main(int argc, char *argv[])
{
    pthread_t hello_threads[THREADS_NR];
    int *count = malloc(sizeof(int));
    int i;

    for (i = 0; i < THREADS_NR; i++)
    {
        *count = i + 1;
        int result = pthread_create(&hello_threads[i], NULL, hello, (void *)count);
        if (result != 0)
        {
            perror("Could not create thread.");
        }
        usleep(1000);
    }
    int *hello_results[THREADS_NR];
    // Czekaj na zakończenie THREADS_NR wątków, pobierz status zakończeńczenia, zapisz go w  hello_result
    // i wypisz na ekran: Thread (indeks wątku+1) TID: (nr_TID) returned value: (status zakonczenia watku)
    for (int i = 0; i < THREADS_NR; i++)
    {
        if (pthread_join(hello_threads[i], (void**) &hello_results[i]) != 0)
        {
            err(EXIT_FAILURE, "pthread_join");
        }
    }
    printf("\n\n");
    for (int i = 0; i < THREADS_NR; i++)
    {
        printf("Thread %d TID: %d returned value: %d\n", i+1, hello_threads[i], *(int *)hello_results[i]);
    }
    free(count);
    // koniec
    return 0;
}
