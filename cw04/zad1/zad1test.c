#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void pending_checker(sigset_t pending_sigs)
{
    sigpending(&pending_sigs);
    if (sigismember(&pending_sigs, SIGUSR1))
        printf("Signal delivered to child\n");
    else
        printf("Signal not delivered to child\n");
    return;
}

int main(int argc, char *args[])
{
    printf("Child process\n");
    sigset_t pending_sigs;

    int type = atoi(args[1]);
    if (type == 3)
    {
        pending_checker(pending_sigs);
        exit(0);
    }
    raise(SIGUSR1);

    exit(0);
}
