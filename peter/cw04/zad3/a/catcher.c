#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

char* mode = "";

void send_with_kill(pid_t sender_pid, int signals_count) {
    for (int i = 0; i < signals_count; ++i) {
        kill(sender_pid, SIGUSR1);
    }
    kill(sender_pid, SIGUSR2);
}

void send_with_sigqueue(pid_t sender_pid, int signals_count) {
    union sigval signal_value;
    for (int i = 0; i < signals_count; ++i) {
        signal_value.sival_int = i;
        sigqueue(sender_pid, SIGUSR1, signal_value);
    }
    ++signal_value.sival_int;
    sigqueue(sender_pid, SIGUSR2, signal_value);
}

void send_with_sigrt(pid_t sender_pid, int signals_count) {
    for (int i = 0; i < signals_count; ++i) {
        kill(sender_pid, SIGRTMIN);
    }
    printf("test\n");
    kill(sender_pid, SIGRTMIN + 1);
}

void signal_handler(int signal_number, siginfo_t* info, void* ucontext) {
    static int signal_count = 0;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN) {
        ++signal_count;
    } else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1) {
        pid_t sender_pid = info->si_pid;
        if (strcmp("KILL", mode) == 0) {
            send_with_kill(sender_pid, signal_count);
        } else if (strcmp("SIGQUEUE", mode) == 0) {
            send_with_sigqueue(sender_pid, signal_count);
        } else if (strcmp("SIGRT", mode) == 0) {
            send_with_sigrt(sender_pid, signal_count);
        }
        printf("Received %d SIGUSR1 signals\n", signal_count);
        exit(0);
    }
}

void block_signals(sigset_t* newmask) {
    sigfillset(newmask);
    sigdelset(newmask, SIGUSR1);
    sigdelset(newmask, SIGUSR2);
    sigdelset(newmask, SIGRTMIN);
    sigdelset(newmask, SIGRTMIN + 1);
    if (sigprocmask(SIG_SETMASK, newmask, NULL) < 0) {
        perror("Nie udało się zablokować sygnału");
        exit(EXIT_FAILURE);
    }
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
