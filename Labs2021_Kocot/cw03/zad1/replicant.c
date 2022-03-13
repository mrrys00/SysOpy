#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    int n = atoi(argv[1]);
    pid_t pid = 1;
    for(int i=0; i<n && pid != 0; i++) {
        pid = fork();
    }
    if(pid == 0) {
        printf("Process #%d\n", (int)getpid());
    }
    else {
        for(int i=0; i<n; i++) {
            wait(NULL);  // waiting for any child to finish its work
        }
    }
}
