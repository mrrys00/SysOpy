#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SAVEFILE "pid_save.bin"

int usr1count = 0;
int usr2received = 0;
int maxusr1id = 0;
int quota;
pid_t catcher_pid;

void intake(sigset_t *mask);
void react(int sig, siginfo_t *info, void *ucontext);
void RTreact(int sig, siginfo_t *info, void *ucontext);
void Qreact(int sig, siginfo_t *info, void *ucontext);

int main(int argc, char * argv[]) {
    // Program sender przyjmuje trzy parametry: PID procesu catcher, ilość sygnałów do wysłania i tryb wysłania sygnałów.
    sigset_t newset, defset;
    catcher_pid = atoi(argv[1]);
    struct sigaction act, oldact;
    quota = atoi(argv[2]);
    char *mode = argv[3];

    sigfillset(&newset);
    sigdelset(&newset, SIGUSR1);
    sigdelset(&newset, SIGUSR2);
    sigdelset(&newset, SIGRTMIN+1);
    sigdelset(&newset, SIGRTMIN+2);
    sigdelset(&newset, SIGINT);  // introduced for testing purposes
    sigprocmask(SIG_BLOCK, &newset, &defset);

    if(catcher_pid == 0) {  // PID 0 is illegal in sender, so it maybe used as default argument
        FILE *pid_save = fopen(SAVEFILE, "rb");  // catcher saves its PID in given file;
        if(pid_save == NULL) {
            printf("ERROR: Received 0 as PID, but couldn't open %s", SAVEFILE);
            return 1;
        }
        fread(&catcher_pid, sizeof catcher_pid, 1, pid_save);
        fclose(pid_save);
    }

    act.sa_flags = SA_SIGINFO;

    if(strcmp(mode, "SIGQUEUE") == 0) {
        act.sa_sigaction = Qreact;
        sigaction(SIGUSR1, &act, &oldact);
        sigaction(SIGUSR2, &act, &oldact);
        quota--;
        union sigval empty;
        sigqueue(catcher_pid, SIGUSR1, empty);
        while(!usr2received) {
            sigsuspend(&newset);
        }
        printf("Sender received returning SIGUSR1 %d times\n", usr1count);
        printf("Last received SIGUSR1 had assigned number %d\n", maxusr1id);
    }
    else if(strcmp(mode, "SIGRT") == 0) {
        act.sa_sigaction = RTreact;
        sigaction(SIGRTMIN+1, &act, &oldact);
        sigaction(SIGRTMIN+2, &act, &oldact);
        quota--;
        kill(catcher_pid, SIGRTMIN+1);
        while(!usr2received) {
            sigsuspend(&newset);
        }
        printf("Sender received returning SIGRTMIN+1 %d times\n", usr1count);
    }
    else {  // using KILL as a default mode (will be chosen if illegal argument is given)
        act.sa_sigaction = react;
        sigaction(SIGUSR1, &act, &oldact);
        sigaction(SIGUSR2, &act, &oldact);
        quota--;
        kill(catcher_pid, SIGUSR1);
        while(!usr2received) {
            sigsuspend(&newset);
        }
        printf("Sender received returning SIGUSR1 %d times\n", usr1count);
    }
}

void react(int sig, siginfo_t *info, void *ucontext) {
    if(sig == SIGUSR1) {
        usr1count++;
        if(quota-- > 0) kill(catcher_pid, SIGUSR1);
        else kill(catcher_pid, SIGUSR2);
    }
    else if(sig == SIGUSR2) {
        usr2received = 1;
    }
}
void RTreact(int sig, siginfo_t *info, void *ucontext) {
    if(sig == SIGRTMIN+1) {
        usr1count++;
        if(quota-- > 0) kill(catcher_pid, SIGRTMIN+1);
        else kill(catcher_pid, SIGRTMIN+2);
    }
    else if(sig == SIGRTMIN+2) {
        usr2received = 1;
    }
}
void Qreact(int sig, siginfo_t *info, void *ucontext) {
    int usr1id;
    union sigval empty;
    if(sig == SIGUSR1) {
        usr1count++;
        usr1id = info->si_value.sival_int;
        maxusr1id = usr1id > maxusr1id ? usr1id : maxusr1id;
        if(quota-- > 0) sigqueue(catcher_pid, SIGUSR1, empty);
        else sigqueue(catcher_pid, SIGUSR2, empty);
    }
    else if(sig == SIGUSR2) {
        usr2received = 1;
    }
}
