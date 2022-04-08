#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

int global_signal_count = 0;

void kill_sender(pid_t catch_proc_pid, int to_send_signals)
{
    for (int i = 0; i < to_send_signals; ++i)
        kill(catch_proc_pid, SIGUSR1);
    kill(catch_proc_pid, SIGUSR2);
    return;
}

void sigqueue_sender(pid_t catch_proc_pid, int to_send_signals)
{
    union sigval sig_val;
    for (int i = 0; i < to_send_signals; ++i)
    {
        sig_val.sival_int = i;
        sigqueue(catch_proc_pid, SIGUSR1, sig_val);
    }
    sigqueue(catch_proc_pid, SIGUSR2, sig_val);
    return;
}

void sigrt_sender(pid_t catch_proc_pid, int to_send_signals)
{
    for (int i = 0; i < to_send_signals; ++i)
        kill(catch_proc_pid, SIGRTMIN);
    kill(catch_proc_pid, SIGRTMIN + 1);
    return;
}

void mask_signals(sigset_t *newmask)
{
    sigfillset(newmask);
    sigdelset(newmask, SIGUSR1);
    sigdelset(newmask, SIGUSR2);
    sigdelset(newmask, SIGRTMIN);
    sigdelset(newmask, SIGRTMIN + 1);
    if (sigprocmask(SIG_SETMASK, newmask, NULL) < 0)
    {
        perror("Cannot block signal");
        exit(EXIT_FAILURE);
    }
}

void signal_handler(int signal_number, siginfo_t *info, void *ucontext)
{
    static int sig_cnt = 0;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN)
        ++sig_cnt;
    else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1)
    {
        int sig_val = info->si_value.sival_int;
        if (sig_val)
            printf("Catcher received %d signals\n", sig_val);
        printf("Received %d signals of %d sent sigals\n", sig_cnt, global_signal_count);
        exit(0);
    }
}

void prepare_sender(sigset_t *mask, struct sigaction *handler_struct, int to_send_signals)
{
    global_signal_count = to_send_signals;
    mask_signals(mask);
    handler_struct->sa_flags = SA_SIGINFO;
    handler_struct->sa_sigaction = signal_handler;
    sigaction(SIGUSR1, handler_struct, NULL);
    sigaction(SIGUSR2, handler_struct, NULL);
    sigaction(SIGRTMIN, handler_struct, NULL);
    sigaction(SIGRTMIN + 1, handler_struct, NULL);

    return;
}

int send_option(pid_t catch_proc_pid, int to_send_signals, char *mode)
{
    if (strcmp(mode, "KILL") == 0)
        kill_sender(catch_proc_pid, to_send_signals);
    else if (strcmp(mode, "SIGQUEUE") == 0)
        sigqueue_sender(catch_proc_pid, to_send_signals);
    else if (strcmp(mode, "SIGRT") == 0)
        sigrt_sender(catch_proc_pid, to_send_signals);
    else
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int sender(pid_t catch_proc_pid, int to_send_signals, char *mode)
{
    sigset_t mask;
    struct sigaction handler_struct;

    prepare_sender(&mask, &handler_struct, to_send_signals);
    if (send_option(catch_proc_pid, to_send_signals, mode))
        return EXIT_FAILURE;
    while (1)
        sigsuspend(&mask);

    return EXIT_SUCCESS;
}

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        printf("Not enough arguments");
        exit(EXIT_FAILURE);
    }

    pid_t catch_proc_pid = atoi(args[1]);
    int to_send_signals = atoi(args[2]);
    char *mode = args[3];

    if (sender(catch_proc_pid, to_send_signals, mode))
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}
