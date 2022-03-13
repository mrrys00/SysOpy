#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void stack_handler(int sig);
void info_handler(int sig, siginfo_t *info, void *ucontext);
void def_handler(int sig);
void rec();
void safe_sigaltstack(const stack_t *restrict ss, stack_t *restrict old_ss);
void safe_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

int main(int argc, char * argv[]) {
    int mode;
    if(argc < 2) mode = 1;
    else mode = atoi(argv[1]);

    stack_t stck;
    struct sigaction act, stact;
    sigemptyset(&act.sa_mask);

    if(mode == 1) {
        printf("-> Testing SIGINFO and RESETHAND\n");
        act.sa_flags = SA_SIGINFO | SA_RESETHAND;
        act.sa_sigaction = info_handler;
        safe_sigaction(SIGUSR2, &act, NULL);
        raise(SIGUSR2);
        raise(SIGUSR2);
    }
    else if(mode == 2) {
        printf("-> Testing ONSTACK (Custom function should be executed despite stack overflow)\n");
        stck.ss_sp = malloc(SIGSTKSZ);
        stck.ss_size = SIGSTKSZ;
        stck.ss_flags = 0;
        safe_sigaltstack(&stck, NULL);
    
        stact.sa_handler = stack_handler;
        stact.sa_flags = SA_ONSTACK;
        safe_sigaction(SIGSEGV, &stact, NULL);

        rec();
    }
    else if(mode == 3) {
        printf("-> Testing lack of ONSTACK (Segmentation fault will occur due to stack overflow)\n");
        stact.sa_handler = stack_handler;
        stact.sa_flags = 0;
        safe_sigaction(SIGSEGV, &stact, NULL);

        rec();
    }
    else printf("illegal mode\n");

    
}

void rec() {
    int a[100][100];  // NOTE: This will produce "unused variable" warning. This is intentional, as this function is made to naturally raise Stack Overflow.
    rec();
}

void stack_handler(int sig) {
    printf("[custom handler] Stack overflow\n");
    raise(SIGABRT);
}

void safe_sigaltstack(const stack_t *restrict ss, stack_t *restrict old_ss) {
    if(sigaltstack(ss, old_ss) == -1) {
        perror("Failed at sigaltstack()");
        exit(EXIT_FAILURE);
    }
}

void safe_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    if(sigaction(signum, act, oldact) != 0) {
        perror("Failed at sigaction()");
        exit(EXIT_FAILURE);
    }
}

void info_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("[custom handler] Signal #%d with number %d, raised by the process with PID=%d\n", sig, info->si_signo, info->si_pid);
}

void def_handler(int sig) {
    printf("[custom handler] Signal #%d caught\n", sig);
}
