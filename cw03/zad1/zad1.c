//
// Created by mrrys00 on 3/23/22.
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *args[]) {
    if (argc < 2) {
        printf("No arguments");
        exit(EXIT_FAILURE);
    }
    int n = atoi(args[1]);

    while(n > 0) {
        pid_t new_process_id = fork();
        if (new_process_id == 0) {
            printf("Notification: pid = %d\n", getpid());
            break;
        }
        n--;
    }

    exit(EXIT_SUCCESS);
}
