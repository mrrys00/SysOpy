#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int seelct_type(char *str_arg)
{
    if (strcmp(str_arg, "ignore") == 0)
        return 0;
    else if (strcmp(str_arg, "handler") == 0)
        return 1;
    else if (strcmp(str_arg, "mask") == 0)
        return 2;
    else if (strcmp(str_arg, "pending") == 0)
        return 3;

    return -1;
}

int set_mask(sigset_t *new_mask, sigset_t *old_mask)
{
    sigemptyset(new_mask);
    sigaddset(new_mask, SIGUSR1);
    return sigprocmask(SIG_BLOCK, new_mask, old_mask);
}

void responser(int sig_num)
{
    printf("Signal recieved %d\n", sig_num);
    return;
}

void pending_checker(sigset_t pending_sigs)
{
    sigpending(&pending_sigs);
    if (sigismember(&pending_sigs, SIGUSR1))
        printf("Signal delivered to child\n");
    else
        printf("Signal not delivered to child\n");
    return;
}

void fork_test(int type)
{
    sigset_t new_mask, old_mask, pending_sigs;

    if (type == 0)
        signal(SIGUSR1, SIG_IGN);
    else if (type == 1)
        signal(SIGUSR1, responser);
    else if (type == 2 || type == 3)
        if (set_mask(&new_mask, &old_mask) < 0)
            perror("Cannot block signal\n");

    raise(SIGUSR1);

    pid_t pid = fork();
    if (pid == 0)
    {
        if (type == 3)
        {
            pending_checker(pending_sigs);
            return;
        }
        raise(SIGUSR1);
    }
    return;
}

int main(int argc, char *args[])
{
    int type;
    if (argc < 2)
    {
        printf("No arguments");
        exit(EXIT_FAILURE);
    }

    type = seelct_type(args[1]);
    if (type == 0 || type == 1 || type == 2 || type == 3)
        fork_test(type);
    else
    {
        perror("Illegal type\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
