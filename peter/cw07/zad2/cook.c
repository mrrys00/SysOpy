#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h> 
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>


int* create_shared_memory() {
    int shared_memory_fd = shm_open("/shared_memory", O_RDWR, 0);

    if (shared_memory_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shared_memory_fd, sizeof(int[10])) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    int* shared_memeory = mmap(NULL, sizeof(int[10]), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memeory == (int*) -1) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return shared_memeory;
}

void release_shared_memory(void* shared_memory_address) {
    if (munmap(shared_memory_address, sizeof(int[10])) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
}

long long get_time_ms() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (((long long) time.tv_sec ) * 1000) + ( time.tv_usec / 1000);
}

void update_semaphore(sem_t* semaphore, int should_increment) {
    if (should_increment == 1) {
        if (sem_post(semaphore) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    } else {
        if (sem_wait(semaphore) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }
    return;
}

void work_routine(int* shared_memory, sem_t** semaphores) {
    int pizza = rand() % 10;
    printf("(%d %lld) Przygotowuje pizze %d\n", getpid(), get_time_ms(), pizza);
    sleep(1 + rand() % 2);
    update_semaphore(semaphores[0], -1); // decrement number of free places in oven
    update_semaphore(semaphores[3], -1); // block window
    for (int i = 0; i < 5; i++) {
        if (shared_memory[i] == -1) {
            shared_memory[i] = pizza;
            break;
        }
    }
    int pizzas_count;
    sem_getvalue(semaphores[0], &pizzas_count);
    pizzas_count = 5 - pizzas_count;
    if (pizzas_count == 6) {    // 6 when semctl returns -1
        perror("semctl getval");
        exit(EXIT_FAILURE);
    }
    printf("(%d %lld) Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time_ms(), pizza, pizzas_count);
    update_semaphore(semaphores[3], 1); // unblock window
    sleep(4 + rand() % 2);
    update_semaphore(semaphores[3], -1); // block window
    int taken_out_pizza = 0;
    for (int i = 0; i < 5; i++) {
        if (shared_memory[i] != -1) {
            taken_out_pizza = shared_memory[i];
            shared_memory[i] = -1;
            break;
        }
    }
    update_semaphore(semaphores[3], 1); // unblock window
    update_semaphore(semaphores[0], 1); // increment number of free places in oven
    sem_getvalue(semaphores[0], &pizzas_count);
    pizzas_count = 5 - pizzas_count;
    int table_pizzas_count;
    sem_getvalue(semaphores[1], &table_pizzas_count);
    table_pizzas_count = 5 - table_pizzas_count;
    if (pizzas_count == 6 || table_pizzas_count == 6) {     // 6 when semctl returns -1
        perror("semctl getval");
        exit(EXIT_FAILURE);
    }
    printf("(%d %lld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d, liczba pizz na stole: %d\n", getpid(), get_time_ms(), taken_out_pizza, pizzas_count, table_pizzas_count);
    update_semaphore(semaphores[1], -1); // decrement number of free table places
    update_semaphore(semaphores[4], -1); // lock table
    int* delivery_table = shared_memory + 5;
    for (int i = 0; i < 5; i++) {
        if (delivery_table[i] == -1) {
            delivery_table[i] = taken_out_pizza;
            break;
        } 
    }
    sem_getvalue(semaphores[1], &table_pizzas_count);
    table_pizzas_count = 5 - table_pizzas_count;
    if (table_pizzas_count == 6) {      // 6 when semctl returns -1
        perror("semctl");
        exit(EXIT_FAILURE);
    } 
    printf("(%d %lld) Klade pizze %d na stole. liczba pizz na stole: %d\n", getpid(), get_time_ms(), taken_out_pizza, table_pizzas_count);
    update_semaphore(semaphores[4], 1); // unlock table
    update_semaphore(semaphores[2], 1); // increment number of pizzas waiting for delivery
}

void release_semaphores(sem_t** semaphores) {
    for (int i = 0; i < 5; i++) {
        if (sem_close(semaphores[i]) == -1) {
            perror("sem_close");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char** argv) {
    srand(time(NULL) + getpid() * 5814);
    sem_t* free_oven_places = sem_open("/free_oven_places", O_RDWR);
    if (free_oven_places == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* free_table_places = sem_open("/free_table_places", O_RDWR);
    if (free_table_places == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* pizzas_waiting_for_delivery = sem_open("/pizzas_waiting_for_delivery", O_RDWR);
    if (pizzas_waiting_for_delivery == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* oven_lock = sem_open("/oven_lock", O_RDWR);
    if (oven_lock == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* table_lock = sem_open("/table_lock", O_RDWR);
    if (table_lock == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* semaphores[5] = {free_oven_places, free_table_places, pizzas_waiting_for_delivery, oven_lock, table_lock};
    int* shared_memory = create_shared_memory();

    // int test = -100;
    // sem_getvalue(semaphores[2], &test);
    // sem_post(semaphores[2]);
    // sem_wait(semaphores[2]);
    // printf("test %d\n", test);

    while (1) {
        work_routine(shared_memory, semaphores);
    }
    printf("cook\n");

    release_semaphores(semaphores);
    release_shared_memory(shared_memory);
    return 0;
}
