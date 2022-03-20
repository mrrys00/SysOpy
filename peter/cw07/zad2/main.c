#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

void spawn_empoloyees(int cooks_count, int deliverers_count) {
    for (int i = 0; i < cooks_count; ++i) {
        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("cook fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid == 0) {
            if (execl("cook.o", "cook.o", NULL) == -1) {
                perror("cook execl");
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < deliverers_count; ++i) {
        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("deliverer fork");
            exit(EXIT_FAILURE);
        }
        if (child_pid == 0) {
            if (execl("deliverer.o", "deliverer.o", NULL) == -1) {
                perror("deliverer execl");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int* create_shared_memory() {
    int shared_memory_fd = shm_open("/shared_memory", O_CREAT | O_TRUNC | O_RDWR, 0777);

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

    for (int i = 0; i < 10; i++) {
        shared_memeory[i] = -1;
    }

    return shared_memeory;
}

void destroy_shared_memory(void* shared_memory_address) {
    if (munmap(shared_memory_address, sizeof(int[10])) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink("/shared_memory") == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
}

void remove_semaphore(sem_t** semaphores) {
    for (int i = 0; i < 5; i++) {
        sem_t* semaphore = semaphores[i];
        if (sem_close(semaphore) == -1) {
            perror("sem_close");
            exit(EXIT_FAILURE);
        }
    }
    if (sem_unlink("/free_oven_places") == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink("/free_table_places") == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink("/pizzas_waiting_for_delivery") == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink("/oven_lock") == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink("/table_lock") == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }
}

void clean_old_semaphores() {
    sem_unlink("/free_oven_places");
    sem_unlink("/free_table_places");
    sem_unlink("/pizzas_waiting_for_delivery");
    sem_unlink("/oven_lock");
    sem_unlink("/table_lock");
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("expected 2 args\n");
        return EXIT_FAILURE;
    }
    int cooks_count = atoi(argv[1]);
    int deliverers_count = atoi(argv[2]);
    clean_old_semaphores();
    sem_t* free_oven_places = sem_open("/free_oven_places", O_CREAT, 0777, 5);
    if (free_oven_places == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* free_table_places = sem_open("/free_table_places", O_CREAT, 0777, 5);
    if (free_table_places == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* pizzas_waiting_for_delivery = sem_open("/pizzas_waiting_for_delivery", O_CREAT, 0777, 0);
    if (pizzas_waiting_for_delivery == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* oven_lock = sem_open("/oven_lock", O_CREAT, 0777, 1);
    if (oven_lock == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* table_lock = sem_open("/table_lock", O_CREAT, 0777, 1);
    if (table_lock == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_t* semaphores[5] = {free_oven_places, free_table_places, pizzas_waiting_for_delivery, oven_lock, table_lock};
    int* shared_memory = create_shared_memory();

    spawn_empoloyees(cooks_count, deliverers_count);

    while (wait(NULL) > 0);
    remove_semaphore(semaphores);
    destroy_shared_memory(shared_memory);
    return 0;
}
