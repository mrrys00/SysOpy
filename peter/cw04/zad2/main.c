#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int should_loop_in_event_handler = 0;

void signal_handler(int signal_number, siginfo_t* info, void* ucontext) {
    printf("I received signal no. %d\n", info->si_signo);
    printf("Sending process pid: %d\n", info->si_pid);
    printf("Real user id of sending process: %d\n", info->si_uid); 
    printf("Exit status if signal for process termination: %d\n", info->si_status);
    printf("Signal code: %d\n", info->si_code);
    if (should_loop_in_event_handler) {
        pause(); // Not a good idea to pause inside signal handler, but it shows that SA_NODEFER flag works as expected
    }
}

int main() {
    struct sigaction act;
    act.sa_sigaction = signal_handler;
    sigemptyset(&act.sa_mask);
    // SA_NODEFER - signal can be received again when inside signal handler
    // SA_RESTART - if sys call gets interrupted by a signal it will get repeated after handling
    act.sa_flags = SA_SIGINFO | SA_NODEFER | SA_RESTART;
    sigaction(SIGINT, &act, NULL);
    char test_str[1000];
    scanf("%s", test_str); // if signal happens during scanf call, bacause of SA_RESTART it will get repeated after handling singal
    printf("received str from scanf: %s\n", test_str);
    should_loop_in_event_handler = 1;
    pause();
    return 0;
}
