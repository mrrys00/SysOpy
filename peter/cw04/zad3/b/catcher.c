#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

char* mode = "";

void signal_handler(int signal_number, siginfo_t* info, void* ucontext) {
    static int signal_count = 0;
    pid_t sender_pid = info->si_pid;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN) {
        ++signal_count;
        if (strcmp("KILL", mode) == 0) {
            kill(sender_pid, SIGUSR1);
        } else if (strcmp("SIGQUEUE", mode) == 0) {
            union sigval signal_value;
            signal_value.sival_int = signal_count;
            sigqueue(sender_pid, SIGUSR1, signal_value);
        } else if (strcmp("SIGRT", mode) == 0) {
            kill(sender_pid, SIGRTMIN);
        }
    } else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1) {
        if (strcmp("KILL", mode) == 0) {
            kill(sender_pid, SIGUSR2);
        } else if (strcmp("SIGQUEUE", mode) == 0) {
            union sigval signal_value;
            signal_value.sival_int = signal_count;
            sigqueue(sender_pid, SIGUSR2, signal_value);
        } else if (strcmp("SIGRT", mode) == 0) {
            kill(sender_pid, SIGRTMIN + 1);
        }
        printf("Received %d SIGUSR1 signals\n", signal_count);
        exit(0);
    }
}

void block_signals(sigset_t* newmask) {
    sigfillset(newmask);
    if (sigprocmask(SIG_SETMASK, newmask, NULL) < 0) {
        perror("Nie udało się zablokować sygnału");
        exit(EXIT_FAILURE);
    }
    sigdelset(newmask, SIGUSR1);
    sigdelset(newmask, SIGUSR2);
    sigdelset(newmask, SIGRTMIN);
    sigdelset(newmask, SIGRTMIN + 1);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        perror("Please specify sending mode");
        return 1;
    }
    mode = argv[1];
    printf("My pid is: %d\n", getpid());
    sigset_t signal_mask;
    block_signals(&signal_mask);
    struct sigaction handler_struct;
    handler_struct.sa_flags = SA_SIGINFO;
    handler_struct.sa_sigaction = signal_handler;
    sigaction(SIGUSR1, &handler_struct, NULL);
    sigaction(SIGUSR2, &handler_struct, NULL);
    sigaction(SIGRTMIN, &handler_struct, NULL);
    sigaction(SIGRTMIN + 1, &handler_struct, NULL);
    while (1) {
        sigsuspend(&signal_mask);
    }
}
