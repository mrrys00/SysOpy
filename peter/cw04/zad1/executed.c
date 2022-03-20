#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv) {
    printf("executing next program\n");
    char* mode = argv[1];
    if (strcmp(mode, "pending") == 0) {
        sigset_t pending_signals;
        sigpending(&pending_signals);
        if (sigismember(&pending_signals, SIGUSR1)) {
            printf("inherited pending signal");
        } else {
            printf("pending signal not has not been inherited");
        }
        return 0;
    }
    raise(SIGUSR1);
}
