#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/ipc.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

typedef union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
} semctl_arg;

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

int* create_shared_memory(key_t key, int* shared_memory_id) {
    *shared_memory_id = shmget(key, sizeof(int[10]), IPC_PRIVATE | IPC_CREAT); // 5 in oven + 5 on the table

    if (*shared_memory_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int* shared_memory = shmat(*shared_memory_id, NULL, 0);
    if (shared_memory == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
        shared_memory[i] = -1;
    }
    return shared_memory;
}

void destroy_shared_memory(void* shared_memory_address, int shared_memory_id) {
    if (shmdt(shared_memory_address) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shared_memory_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

int create_semaphore(key_t key) {
    int semaphore_id = semget(key, 5, IPC_PRIVATE | IPC_CREAT);
    if (semaphore_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    semctl_arg arg;
    arg.val = 5;
    if (semctl(semaphore_id, 0, SETVAL, arg) == -1) {
        perror("semctl setval");
        exit(EXIT_FAILURE);
    }
    if (semctl(semaphore_id, 1, SETVAL, arg) == -1) {
        perror("semctl setval");
        exit(EXIT_FAILURE);
    }
    arg.val = 0;
    if (semctl(semaphore_id, 2, SETVAL, arg) == -1) {
        perror("semctl setval");
        exit(EXIT_FAILURE);
    }
    arg.val = 1;
    if (semctl(semaphore_id, 3, SETVAL, arg) == -1) {
        perror("semctl setval");
        exit(EXIT_FAILURE);
    }
    if (semctl(semaphore_id, 4, SETVAL, arg) == -1) {
        perror("semctl setval");
        exit(EXIT_FAILURE);
    }
    return semaphore_id;
}

void remove_semaphore(int semaphore_id) {
    if (semctl(semaphore_id, 0, IPC_RMID) == -1) {
        perror("semctl remove");
        exit(EXIT_FAILURE);
    }
} 

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("expected 2 args\n");
        return EXIT_FAILURE;
    }
    int cooks_count = atoi(argv[1]);
    int deliverers_count = atoi(argv[2]);

    key_t key = ftok("main.o", 'p');
    int semaphore_id = create_semaphore(key);
    int shared_memory_id; // create_shared_memory will assign proper value to this variable
    int* shared_memory = create_shared_memory(key, &shared_memory_id);


    spawn_empoloyees(cooks_count, deliverers_count);

    while (wait(NULL) > 0);
    remove_semaphore(semaphore_id);
    destroy_shared_memory(shared_memory, shared_memory_id);
    return 0;
}