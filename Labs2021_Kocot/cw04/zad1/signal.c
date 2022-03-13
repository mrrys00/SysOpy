#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int choose_mode(char* str) {
    if(strcmp(str, "ignore") == 0) return 0;
    else if(strcmp(str, "handler") == 0) return 1;
    else if(strcmp(str, "mask") == 0) return 2;
    else if(strcmp(str, "pending") == 0) return 3;
    else return -1;
}

int set_custom_mask(sigset_t *new, sigset_t *old) {
    sigemptyset(new);
    sigaddset(new, SIGUSR1);
    return sigprocmask(SIG_BLOCK, new, old);
}

void react(int signum) {
    printf("Received signal #%d in process #%d\n", signum, getpid());
}

int main(int argc, char * argv[]) {
    int mode = choose_mode(argv[1]);
    sigset_t new, old, pend;
    switch(mode) {
        case 0:
            signal(SIGUSR1, SIG_IGN);
            break;
        case 1:
            signal(SIGUSR1, react);
            break;
        case 2:
        case 3:
            if(set_custom_mask(&new, &old) < 0) {
                perror("Unable to block signal");
            }
            break;
        default:
            perror("Illegal argument");
    }

    raise(SIGUSR1);

    if(mode == 3) {
        sigpending(&pend);
        if(sigismember(&pend, SIGUSR1)) printf("SIGUSR1 is pending in parent process\n");
        else printf("SIGUSR1 is NOT pending in parent process\n");
    }

    pid_t pid = fork();
    if(pid == 0) {
        if(mode == 3) {
            sigpending(&pend);
            if(sigismember(&pend, SIGUSR1)) printf("SIGUSR1 is pending in the child process\n");
            else printf("SIGUSR1 is NOT pending in the child process\n");
        }
        else {
            raise(SIGUSR1);
        }
        exit(0);
    }
    else {
        wait(NULL);
        execl("signal_exec", "signal_exec", argv[1], NULL);
    }
}
