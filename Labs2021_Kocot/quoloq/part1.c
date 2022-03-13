#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <limits.h>
#include <time.h>
#include <errno.h>

void alloc() {
    void *a = malloc(10 * sizeof(int));
    void *b = calloc(10, sizeof(int));
    a = realloc(a, 12 * sizeof(int));
    free(a); free(b);
}

// TIMING - times()

void sysfiles() {
    int fd = open("filename", O_WRONLY | O_CREAT | 0666 | O_TRUNC);
    char buf[20];
    int readBytes = read(fd, buf, sizeof(char) * 20);
    int writtenBytes = read(fd, buf, sizeof(char) * 20);
    close(fd);
}
void libfiles() {
    FILE *fp = fopen("filename", "[rwa+b]");
    char buf[20];
    int readBytes = fread(buf, sizeof(char), 20, fp);
    char c = fgetc(fp);
    int writtenBytes = fread(buf, sizeof(char), 20, fp);
    fprintf(fp, "%c", c);
    // fseek(fp, offset, mode);  // mode: 0=from start; 1=from here; 2=from EOF
    // fsetpos, fgetpos
    fclose(fp);
}
void filesample() {
    int fin =  open("file1", O_RDONLY, 0666);
    int fout = open("file2", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    int rd;
    char c = '\n';
    const char endl = '\n';
    while((rd = read(fin, &c, 1))) {
        if(c == '\n') {
            //
        }
        else {
            //
        }
    }
}

void widelec() {
    pid_t pid = fork();
    if(pid == 0) {}
    else {}
}
void exec() {
    execl("main", "main", "argument1...", NULL);
}

// SIGNALS
int set_custom_mask(sigset_t *new, sigset_t *old) {
    sigemptyset(new);
    sigaddset(new, SIGUSR1);
    return sigprocmask(SIG_BLOCK, new, old);
}
void pending() {
    sigset_t pend;
    sigpending(&pend);
    if(sigismember(&pend, SIGUSR1)) printf("SIGUSR1 is pending in parent process\n");
    else printf("SIGUSR1 is NOT pending in parent process\n");
}
void handlers() {
    //void info_handler(int sig, siginfo_t *info, void *ucontext) {
    //    printf("[custom handler] Signal #%d with number %d, raised by the process with PID=%d\n", sig, info->si_signo, info->si_pid);
    //}

    //void def_handler(int sig) {
    //    printf("[custom handler] Signal #%d caught\n", sig);
    //}
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    //act.sa_sigaction = info_handler;
    safe_sigaction(SIGUSR2, &act, NULL);
    raise(SIGUSR2);
}
void safe_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    if(sigaction(signum, act, oldact) != 0) {
        perror("Failed at sigaction()");
        exit(EXIT_FAILURE);
    }
}
void Qreact(int sig, siginfo_t *info, void *ucontext) {
    int usr1id;
    if(sig == SIGUSR1) {
        usr1count++;
        usr1id = info->si_value.sival_int;
        maxusr1id = usr1id > maxusr1id ? usr1id : maxusr1id;
    }
    else if(sig == SIGUSR2) usr2received = 1;
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

// DUPY
void relay() {
    int fd1[2];
    pipe(fd1);
    int in = fd1[1];
    int out = fd1[0];
    if(in  >= 0) dup2(in,  STDIN_FILENO);
    if(out >= 0) dup2(out, STDOUT_FILENO);
}
void named() {
    mkfifo("pathname", O_RDONLY);
    // use like file
}

int main(int argc, char * argv[]) {

}
