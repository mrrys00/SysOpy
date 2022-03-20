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

int* create_shared_memory(key_t key, int* shared_memory_id) {
    *shared_memory_id = shmget(key, 0, 0);

    if (*shared_memory_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int* shared_memeory = shmat(*shared_memory_id, NULL, 0);
    if (shared_memeory == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shared_memeory;
}

void release_shared_memory(void* shared_memory_address) {
    if (shmdt(shared_memory_address) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

int get_semaphore_id(key_t key) {
    int semaphore_id = semget(key, 0, 0);
    if (semaphore_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semaphore_id;
}

long long get_time_ms() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (((long long) time.tv_sec ) * 1000) + ( time.tv_usec / 1000);
}

void update_semaphore(int semaphore_id, int semaphore_number, int value) {
    struct sembuf operation;
    operation.sem_num = semaphore_number;
    operation.sem_op = value;
    operation.sem_flg = 0;
    if (semop(semaphore_id, &operation, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void work_routine(int* shared_memory, int semaphore_id) {
    int pizza = rand() % 10;
    printf("(%d %lld) Przygotowuje pizze %d\n", getpid(), get_time_ms(), pizza);
    sleep(1 + rand() % 2);
    update_semaphore(semaphore_id, 0, -1); // decrement number of free places in oven
    update_semaphore(semaphore_id, 3, -1); // block window
    for (int i = 0; i < 5; i++) {
        if (shared_memory[i] == -1) {
            shared_memory[i] = pizza;
            break;
        }
    }
    int pizzas_count = 5 - semctl(semaphore_id, 0, GETVAL);
    if (pizzas_count == 6) {    // 6 when semctl returns -1
        perror("semctl getval");
        exit(EXIT_FAILURE);
    }
    printf("(%d %lld) Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_time_ms(), pizza, pizzas_count);
    update_semaphore(semaphore_id, 3, 1); // unblock window
    sleep(4 + rand() % 2);
    update_semaphore(semaphore_id, 3, -1); // block window
    int taken_out_pizza = 0;
    for (int i = 0; i < 5; i++) {
        if (shared_memory[i] != -1) {
            taken_out_pizza = shared_memory[i];
            shared_memory[i] = -1;
            break;
        }
    }
    update_semaphore(semaphore_id, 3, 1); // unblock window
    update_semaphore(semaphore_id, 0, 1); // increment number of free places in oven
    pizzas_count = 5 - semctl(semaphore_id, 0, GETVAL);
    int table_pizzas_count = 5 - semctl(semaphore_id, 1, GETVAL);
    if (pizzas_count == 6 || table_pizzas_count == 6) {     // 6 when semctl returns -1
        perror("semctl getval");
        exit(EXIT_FAILURE);
    }
    printf("(%d %lld) Wyjmuję pizze: %d. Liczba pizz w piecu: %d, liczba pizz na stole: %d\n", getpid(), get_time_ms(), taken_out_pizza, pizzas_count, table_pizzas_count);
    update_semaphore(semaphore_id, 1, -1); // decrement number of free table places
    update_semaphore(semaphore_id, 4, -1); // lock table
    int* delivery_table = shared_memory + 5;
    for (int i = 0; i < 5; i++) {
        if (delivery_table[i] == -1) {
            delivery_table[i] = taken_out_pizza;
            break;
        } 
    }
    table_pizzas_count = 5 - semctl(semaphore_id, 1, GETVAL);
    if (table_pizzas_count == 6) {      // 6 when semctl returns -1
        perror("semctl");
        exit(EXIT_FAILURE);
    } 
    printf("(%d %lld) Klade pizze %d na stole. liczba pizz na stole: %d\n", getpid(), get_time_ms(), taken_out_pizza, table_pizzas_count);
    update_semaphore(semaphore_id, 4, 1); // unlock table
    update_semaphore(semaphore_id, 2, 1); // increment number of pizzas waiting for delivery
}

int main(int argc, char** argv) {
    srand(time(NULL) + getpid() * 5814);
    key_t key = ftok("main.o", 'p');
    int semaphore_id = get_semaphore_id(key);
    int shared_memory_id; // create_shared_memory will assign proper value to this variable
    int* shared_memory = create_shared_memory(key, &shared_memory_id);

    while (1) {
        work_routine(shared_memory, semaphore_id);
    }
    printf("cook\n");

    release_shared_memory(shared_memory);
    return 0;
}
