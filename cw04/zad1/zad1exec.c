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

void exec_test(int type)
{
    char *ctype;
    sigset_t new_mask, old_mask;
    if (type == 0)
    {
        signal(SIGUSR1, SIG_IGN);
        ctype = "0";
    }
    else if (type == 2 || type == 3)
    {
        if (set_mask(&new_mask, &old_mask) < 0)
            perror("Cannot block signal\n");

        if (type == 2) ctype = "2";
        else ctype = "3";
    }
    raise(SIGUSR1);
    execl("zad1test", "zad1test", ctype, NULL);
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
    printf("%d\n", type);
    if (type == 0 || type == 2 || type == 3)
        exec_test(type);
    else
    {
        perror("Illegal type\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
