#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    int children_count;
    if (argc == 0) {
        printf("how many processes should be created: ");
        scanf("%d", &children_count);
    } else {
        children_count = atoi(argv[1]);
    }
    for (int i = 0; i < children_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("message from child process with pid: %d\n", (int)getpid());
            return 0;
        }
    }
    while (wait(NULL) > 0);
}