#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void signal_handler(int signal_number) {
    printf("I received signal %d\n", signal_number);
}

void fork_test(char* mode) {
    if (strcmp(mode, "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    } else if (strcmp(mode, "handler") == 0) {
        signal(SIGUSR1, signal_handler);
    } else if (strcmp(mode, "mask") == 0 || strcmp(mode, "pending") == 0) {
        sigset_t newmask;
        sigset_t oldmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
            perror("Nie udało się zablokować sygnału");
        }
    } else {
        perror("invalid argument");
        return;
    }
    raise(SIGUSR1);

    pid_t pid = fork();
    if (pid == 0) {
        if (strcmp(mode, "pending") == 0) {
            sigset_t pending_signals;
            sigpending(&pending_signals);
            if (sigismember(&pending_signals, SIGUSR1)) {
                printf("inherited pending signal");
            } else {
                printf("pending signal not has not been inherited");
            }
            return;
        }
        raise(SIGUSR1);
    }
}

int main(int argc, char** argv) {
    char* mode; 
    if (argc == 1) {
        mode = calloc(1000, sizeof(char));
        printf("please select mode: ");
        scanf("%s", mode);
    } else {
        mode = argv[1];
    }
    fork_test(mode);
}
