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
    int delivered_pizza;
    int* delivery_table = shared_memory + 5;

    update_semaphore(semaphore_id, 2, -1); // decrement number of pizzas waiting for delivery
    update_semaphore(semaphore_id, 4, -1); // lock table
    update_semaphore(semaphore_id, 1,  1);  // increment number of table free places

    for (int i = 0; i < 5; i++) {
        if (delivery_table[i] != -1) {
            delivered_pizza = delivery_table[i];
            delivery_table[i] = -1;
            break;
        }
    }

    int pizzas_count = semctl(semaphore_id, 2, GETVAL);
    printf("(%d %lld) Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_time_ms(), delivered_pizza, pizzas_count);

    update_semaphore(semaphore_id, 4, 1); // unlock table

    sleep(4 + rand() % 2);
    printf("(%d %lld) Dostarczam pizze: %d.\n", getpid(), get_time_ms(), delivered_pizza);
    sleep(4 + rand() % 2);
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
    printf("deliverer\n");

    release_shared_memory(shared_memory);
    return 0;
}
