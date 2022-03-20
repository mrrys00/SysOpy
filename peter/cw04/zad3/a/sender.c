#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

int global_signal_count = 0;

void send_with_kill(pid_t catcher_pid, int signals_count) {
    for (int i = 0; i < signals_count; ++i) {
        kill(catcher_pid, SIGUSR1);
    }
    kill(catcher_pid, SIGUSR2);
}

void send_with_sigqueue(pid_t catcher_pid, int signals_count) {
    union sigval signal_value;
    for (int i = 0; i < signals_count; ++i) {
        signal_value.sival_int = i;
        sigqueue(catcher_pid, SIGUSR1, signal_value);
    }
    sigqueue(catcher_pid, SIGUSR2, signal_value);
}

void send_with_sigrt(pid_t catcher_pid, int signals_count) {
    for (int i = 0; i < signals_count; ++i) {
        kill(catcher_pid, SIGRTMIN);
    }
    kill(catcher_pid, SIGRTMIN + 1);
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

void signal_handler(int signal_number, siginfo_t* info, void* ucontext) {
    static int signal_count = 0;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN) {
        ++signal_count;
    } else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1) {
        int signal_value = info->si_value.sival_int;
        if (signal_value) {
            printf("Catcher received %d SIGUSR1 signals\n", signal_value);
        }
        printf("Received %d SIGUSR1 signals\n", signal_count);
        printf("Should receive %d sigals\n", global_signal_count);
        exit(0);
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        perror("Please pass in 3 arguments");
        return 1;
    }
    pid_t catcher_pid = atoi(argv[1]);
    int signals_count = atoi(argv[2]);
    char* mode = argv[3];
    global_signal_count = signals_count;
    sigset_t mask;
    block_signals(&mask);
    struct sigaction handler_struct;
    handler_struct.sa_flags = SA_SIGINFO;
    handler_struct.sa_sigaction = signal_handler;
    sigaction(SIGUSR1, &handler_struct, NULL);
    sigaction(SIGUSR2, &handler_struct, NULL);
    sigaction(SIGRTMIN, &handler_struct, NULL);
    sigaction(SIGRTMIN + 1, &handler_struct, NULL);
    if (strcmp(mode, "KILL") == 0) {
        send_with_kill(catcher_pid, signals_count);
    } else if (strcmp(mode, "SIGQUEUE") == 0) {
        send_with_sigqueue(catcher_pid, signals_count);
    } else if (strcmp(mode, "SIGRT") == 0) {
        send_with_sigrt(catcher_pid, signals_count);
    } else {
        perror("invalid mode");
        return EXIT_FAILURE;
    }
    while (1) {
        sigsuspend(&mask);
    }
}
