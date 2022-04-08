#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *mode;

void kill_sender(pid_t sender_proc_pid, int signal)
{
    kill(sender_proc_pid, signal);
    return;
}

void sigqueue_sender(pid_t sender_proc_pid, int signal, int signal_count)
{
    union sigval empty;
    empty.sival_int = signal_count;
    sigqueue(sender_proc_pid, signal, empty);
    return;
}

void sigrt_sender(pid_t sender_proc_pid, int signal)
{
    kill(sender_proc_pid, signal);
    return;
}

void signal_handler(int signal_number, siginfo_t *info, void *ucontext)
{
    static int signal_count = 0;
    pid_t sender_pid = info->si_pid;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN)
    {
        ++signal_count;
        if (strcmp("KILL", mode) == 0)
            kill_sender(sender_pid, SIGUSR1);
        else if (strcmp("SIGQUEUE", mode) == 0)
            sigqueue_sender(sender_pid, SIGUSR1, signal_count);
        else if (strcmp("SIGRT", mode) == 0)
            sigrt_sender(sender_pid, SIGRTMIN);
    }
    else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1)
    {
        if (strcmp("KILL", mode) == 0)
            kill_sender(sender_pid, SIGUSR2);
        else if (strcmp("SIGQUEUE", mode) == 0)
            sigqueue_sender(sender_pid, SIGUSR2, signal_count);
        else if (strcmp("SIGRT", mode) == 0)
            sigrt_sender(sender_pid, SIGRTMIN + 1);
        printf("Received %d signals\n", signal_count);
        exit(0);
    }
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

void prepare_catcher(sigset_t *mask, struct sigaction *handler_struct)
{
    mask_signals(mask);
    handler_struct->sa_flags = SA_SIGINFO;
    handler_struct->sa_sigaction = signal_handler;
    sigaction(SIGUSR1, handler_struct, NULL);
    sigaction(SIGUSR2, handler_struct, NULL);
    sigaction(SIGRTMIN, handler_struct, NULL);
    sigaction(SIGRTMIN + 1, handler_struct, NULL);

    return;
}

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        printf("Not enough arguments");
        exit(EXIT_FAILURE);
    }
    mode = args[1];
    sigset_t mask;
    struct sigaction handler_struct;

    printf("Catcher PID: %d\n", getpid());
    mask_signals(&mask);

    prepare_catcher(&mask, &handler_struct);

    while (1)
    {
        sigsuspend(&mask);
    }

    exit(EXIT_SUCCESS);
}
