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
pid_t sender_pid = -1;

void react(int sig, siginfo_t *info, void *ucontext);
void intake(sigset_t *mask);
void send_back();
void RTsend_back();
void Qsend_back();

int main(int argc, char * argv[]) {
    sigset_t newset, defset;
    struct sigaction act, oldact;
    pid_t pid = getpid();
    char *mode = argv[1];

    sigfillset(&newset);
    sigdelset(&newset, SIGUSR1);
    sigdelset(&newset, SIGUSR2);
    sigdelset(&newset, SIGRTMIN+1);
    sigdelset(&newset, SIGRTMIN+2);
    sigdelset(&newset, SIGINT);  // introduced for testing purposes
    sigprocmask(SIG_BLOCK, &newset, &defset);

    printf("Catcher reporting with PID %d\n", pid);
    FILE *pid_save = fopen(SAVEFILE, "wb");  // catcher saves its PID to file; sender will try to read it if given 0 as PID
    if(pid_save == NULL) {
        printf("WARNING: Unable to open %s", SAVEFILE);
    }
    else {
        fwrite(&pid, sizeof pid, 1, pid_save);
        fclose(pid_save);
    }

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = react;

    if(strcmp(mode, "SIGQUEUE") == 0) {
        sigaction(SIGUSR1, &act, &oldact);
        sigaction(SIGUSR2, &act, &oldact);
        intake(&newset);
        Qsend_back();
        printf("Catcher received SIGUSR1 %d times\n", usr1count);
    }
    else if(strcmp(mode, "SIGRT") == 0) {
        sigaction(SIGRTMIN+1, &act, &oldact);
        sigaction(SIGRTMIN+2, &act, &oldact);
        intake(&newset);
        RTsend_back();
        printf("Catcher received SIGRTMIN+1 %d times\n", usr1count);
    }
    else {  // using KILL as a default mode (will be chosen if illegal argument is given)
        sigaction(SIGUSR1, &act, &oldact);
        sigaction(SIGUSR2, &act, &oldact);
        intake(&newset);
        send_back();
        printf("Catcher received SIGUSR1 %d times\n", usr1count);
    }
}

void react(int sig, siginfo_t *info, void *ucontext) {
    if(sender_pid == -1) {
        sender_pid = info->si_pid;
    }

    if(sig == SIGUSR1 || sig == SIGRTMIN+1) usr1count++; 
    else if(sig == SIGUSR2 || sig == SIGRTMIN+2) usr2received = 1;
}

void intake(sigset_t *mask) {
    while(!usr2received) {
        sigsuspend(mask);
    }
}
void send_back() {
    for(int i=0; i<usr1count; i++) {
        kill(sender_pid, SIGUSR1);
    }
    kill(sender_pid, SIGUSR2);
}
void RTsend_back() {
    for(int i=0; i<usr1count; i++) {
        kill(sender_pid, SIGRTMIN+1);
    }
    kill(sender_pid, SIGRTMIN+2);
}
void Qsend_back() {
    union sigval data;
    for(int i=0; i<usr1count; i++) {
        data.sival_int = i+1;
        sigqueue(sender_pid, SIGUSR1, data);
    }
    data.sival_int = usr1count;
    sigqueue(sender_pid, SIGUSR2, data);
}
