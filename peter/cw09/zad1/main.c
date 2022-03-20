#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t workshop_quque = PTHREAD_COND_INITIALIZER; // block elf if there are already 3 elves with santa
pthread_cond_t reindeers_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t solving_problems = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_sleeping = PTHREAD_COND_INITIALIZER;

int waiting_elves_count = 0;
int are_elves_waiting = 0;
int are_reindeers_waiting = 0;
int waiting_elves_ids[3];
int delivered_presents_count = 0;

int waiting_reindeers_count = 0;

void* santa_routine(void* arg) {
    while (1) {
        pthread_mutex_lock(&santa_mutex);
        pthread_cond_wait(&santa_sleeping, &santa_mutex);
        pthread_mutex_unlock(&santa_mutex);
        printf("Mikołaj: budzę się\n");
        if (are_reindeers_waiting) {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(2 + rand() % 3);
            pthread_cond_broadcast(&reindeers_waiting);
            ++delivered_presents_count;
            if (delivered_presents_count == 3) {
                return NULL;
            }
        }
        if (are_elves_waiting) {
            printf("Mikołaj: rozwiązuję problemy elfów %d, %d, %d\n", waiting_elves_ids[0], waiting_elves_ids[1], waiting_elves_ids[2]);
            pthread_cond_broadcast(&solving_problems);
            sleep(1 + rand() % 2);
            pthread_cond_broadcast(&solving_problems);
        }
        printf("Mikołaj: zasypiam\n");
    }
    return NULL;
}

void* elf_routine(void* thread_id_ptr) {
    int thread_id = *(int*) thread_id_ptr;
    while (1) {
        sleep(2 + rand() % 4);
        pthread_mutex_lock(&elves_mutex);
        // wait till can go to santa workshop
        while (waiting_elves_count == 3) {
            printf("Elf %d: czekam na powrót elfów\n", thread_id);
            pthread_cond_wait(&workshop_quque, &elves_mutex);
        }
        waiting_elves_ids[waiting_elves_count] = thread_id;
        ++waiting_elves_count;
        printf("Elf %d: czeka %d elfów na Mikołaja\n", thread_id, waiting_elves_count);
        if (waiting_elves_count != 3) {
            // wait under santa workshop
            pthread_cond_wait(&solving_problems, &elves_mutex);
            printf("Elf %d: Mikołaj rozwiązuje problem\n", thread_id);
            pthread_cond_wait(&solving_problems, &elves_mutex);
        } else {
            // came as 3rd elf.
            // wake up santa and also wait for solving problems
            printf("Elf %d: wybudzam mikołaja\n", thread_id);
            are_elves_waiting = 1;
            pthread_cond_broadcast(&santa_sleeping);
            pthread_cond_wait(&solving_problems, &elves_mutex);
            printf("Elf %d: Mikołaj rozwiązuje problem\n", thread_id);
            pthread_cond_wait(&solving_problems, &elves_mutex);
            // problems solved - clean up
            are_elves_waiting = 0;
            waiting_elves_count -= 3;
        }
        pthread_mutex_unlock(&elves_mutex);
    }
    return NULL;
}

void* reindeer_routine(void* thread_id_ptr) {
    int thread_id = *(int*) thread_id_ptr;
    while (1) {
        sleep(5 + rand() % 6);
        pthread_mutex_lock(&reindeers_mutex);
        ++waiting_reindeers_count;
        if (waiting_reindeers_count < 9) {
            printf("Renifer %d, czeka %d reniferów na Mikołaja\n", thread_id, waiting_reindeers_count);
            pthread_cond_wait(&reindeers_waiting, &reindeers_mutex);
        } else {
            printf("Renifer %d, wybudzam Mikołaja\n", thread_id);
            are_reindeers_waiting = 1;
            pthread_cond_broadcast(&santa_sleeping);
            pthread_cond_wait(&reindeers_waiting, &reindeers_mutex);
            are_reindeers_waiting = 0;
            waiting_reindeers_count = 0;
        }
        pthread_mutex_unlock(&reindeers_mutex);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&santa_mutex, NULL);
    pthread_mutex_init(&reindeers_mutex, NULL);
    pthread_mutex_init(&elves_mutex, NULL);
    int thread_numbers[10];
    pthread_t santa_tid;
    pthread_t elf_tids[10];
    pthread_t reindeer_tids[9];
    pthread_attr_t santa_attr;
    pthread_attr_t elf_attrs[10];
    pthread_attr_t reindeer_attrs[9];
    pthread_attr_init(&santa_attr);
    for (int i = 0; i < 10; ++i) {
        thread_numbers[i] = i;
        if (i < 9) {
            pthread_attr_init(&reindeer_attrs[i]);
        }
        pthread_attr_init(&elf_attrs[i]);
    }
    pthread_create(&santa_tid, &santa_attr, santa_routine, NULL);
    for (int i = 0; i < 10; ++ i) {
        pthread_create(&elf_tids[i], &elf_attrs[i], elf_routine, &thread_numbers[i]);
        if (i < 9) {
            pthread_create(&reindeer_tids[i], &reindeer_attrs[i], reindeer_routine, &thread_numbers[i]);
        }
    }
    if (pthread_join(santa_tid, NULL) != 0) {
        perror("pthread_join");
        return EXIT_FAILURE;
    };
    for (int i = 0; i < 10; ++i) {
        pthread_attr_destroy(&elf_attrs[i]);
        pthread_attr_destroy(&reindeer_attrs[i]);
    }
    pthread_attr_destroy(&santa_attr);
    return EXIT_SUCCESS;
}
