#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void handle_stack_overflow(int sig)
{
    printf("Stack overflow\n");
    raise(SIGABRT);
    return;
}

void soft_sigaction(int signal_number, const struct sigaction *new_act, struct sigaction *old_act)
{
    if (sigaction(signal_number, new_act, old_act) != 0)
    {
        perror("sigaction() failed");
        exit(EXIT_FAILURE);
    }
    return;
}

void handle_information(int sig, siginfo_t *info, void *ucontext)
{
    printf("Signal: #%d ; sig_num: %d ; raised by pid: %d\n", sig, info->si_signo, info->si_pid);
    return;
}

void controlled_overflow()
{
#pragma GCC diagnostic push // try to ignore unused variable warning
#pragma GCC diagnostic ignored "-Wunused-variable"
    int arr[100][100]; // expected warning!!!
    controlled_overflow();
#pragma GCC diagnostic pop
    return;
}

void handle_type0(stack_t stackt, struct sigaction new_act, struct sigaction old_act)
{
    printf("SIGINFO | NODEFER - handle\n");
    new_act.sa_flags = SA_NODEFER | SA_SIGINFO;
    new_act.sa_sigaction = handle_information;
    soft_sigaction(SIGINT, &new_act, NULL);
    raise(SIGINT);
    raise(SIGINT);
    return;
}

void handle_type1(stack_t stackt, struct sigaction new_act, struct sigaction old_act)
{
    printf("ONSTACK - Stack overflow - Aborted\n");
    stackt.ss_sp = malloc(SIGSTKSZ);
    stackt.ss_size = SIGSTKSZ;
    stackt.ss_flags = 0;
    if (sigaltstack(&stackt, NULL) == -1)
    {
        perror("Failed at sigaltstack()");
        exit(EXIT_FAILURE);
    }

    old_act.sa_handler = handle_stack_overflow;
    old_act.sa_flags = SA_ONSTACK;
    soft_sigaction(SIGSEGV, &old_act, NULL);

    controlled_overflow();
    return;
}

void handle_type2(stack_t stackt, struct sigaction new_act, struct sigaction old_act)
{
    printf("ONSTACK - Stack owerflow - Segmentation vault\n");
    old_act.sa_handler = handle_stack_overflow;
    old_act.sa_flags = 0;
    soft_sigaction(SIGSEGV, &old_act, NULL);

    controlled_overflow();
    return;
}

void test(int type)
{
    struct sigaction new_act, old_act;
    stack_t stackt;
    sigemptyset(&new_act.sa_mask);

    if (type == 0)
        handle_type0(stackt, new_act, old_act);
    else if (type == 1)
        handle_type1(stackt, new_act, old_act);
    else if (type == 2)
        handle_type2(stackt, new_act, old_act);
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

    type = atoi(args[1]);
    if (type == 0 || type == 1 || type == 2)
        test(type);
    else
    {
        perror("Illegal type\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
