//
// Created by mrrys00 on 3/23/22.
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *args[]) {
    int n;
    if (argc >= 2) {
        n = atoi(args[1]);
    } else if (argc < 2) {
        printf("hey user, where is argument? try again …");
        exit(EXIT_FAILURE);
    }

    while(n > 0) {
        pid_t new_process_id = fork();
        if (new_process_id == 0) {
            printf("Notification %d\n", getpid());
            break;
        }
        n--;
    }

    exit(EXIT_SUCCESS);
}
