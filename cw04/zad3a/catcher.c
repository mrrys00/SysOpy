#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

char *mode;

void kill_sender(pid_t sender_proc_pid, int to_send_signals)
{
    for (int i = 0; i < to_send_signals; ++i)
        kill(sender_proc_pid, SIGUSR1);
    kill(sender_proc_pid, SIGUSR2);
    return;
}

void sigqueue_sender(pid_t sender_proc_pid, int to_send_signals)
{
    union sigval sig_val;
    for (int i = 0; i < to_send_signals; ++i)
    {
        sig_val.sival_int = i;
        sigqueue(sender_proc_pid, SIGUSR1, sig_val);
    }
    sigqueue(sender_proc_pid, SIGUSR2, sig_val);
    return;
}

void sigrt_sender(pid_t sender_proc_pid, int to_send_signals)
{
    for (int i = 0; i < to_send_signals; ++i)
        kill(sender_proc_pid, SIGRTMIN);
    kill(sender_proc_pid, SIGRTMIN + 1);
    return;
}

void signal_handler(int signal_number, siginfo_t *info, void *ucontext)
{
    static int sig_cnt = 0;
    if (signal_number == SIGUSR1 || signal_number == SIGRTMIN)
        ++sig_cnt;
    else if (signal_number == SIGUSR2 || signal_number == SIGRTMIN + 1)
    {
        pid_t sender_pid = info->si_pid;
        if (strcmp("KILL", mode) == 0)
            kill_sender(sender_pid, sig_cnt);
        else if (strcmp("SIGQUEUE", mode) == 0)
            sigqueue_sender(sender_pid, sig_cnt);
        else if (strcmp("SIGRT", mode) == 0)
            sigrt_sender(sender_pid, sig_cnt);
        printf("Received %d signals\n", sig_cnt);
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
