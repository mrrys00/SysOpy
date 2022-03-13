#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main( int argc, char *argv[] )
{
    char* aaa = "abcd";
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        //aaa[1] = 'W';
        printf("Inside: %p\n", aaa);
        //printf("as");
    }
    else {
        wait(NULL);
        printf("\nOutside: %p\n", aaa);
    }
}
