#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define PFIFO "./fifo"
#define PRODUCER "./prod"
#define CONSUMER "./cons"
#define CHECK "./check"
#define OUTCONSUMER "out_cons.txt"

int main(int argc, char *args[])
{
    if (argc < 5)
    {
        printf("Not enough args");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(PFIFO, S_IFIFO | S_IRUSR | S_IWUSR) != 0)
        exit(EXIT_FAILURE);

    printf("prod: %d\tcons: %d\tN: %d\n", atoi(args[1]), atoi(args[2]), atoi(args[3]));

    for (int i = 0; i < atoi(args[1]); ++i)     // execute producers
        if (fork() == 0)
        {
            char _id[16], rfp[128];
            sprintf(_id, "%d", i + 1);
            sprintf(rfp, args[4], i + 1);
            execl(PRODUCER, PRODUCER, PFIFO, args[3], _id, rfp, NULL);
        }

    remove(OUTCONSUMER);
    for (int i = 0; i < atoi(args[2]); ++i)     // execute consumers
        if (fork() == 0)
            execl(CONSUMER, CONSUMER, PFIFO, args[3], OUTCONSUMER, NULL);
    while (wait(NULL) > 0)
        ;

    for (int i = 0; i < atoi(args[1]); ++i)     // execute checks
    {
        if (fork() == 0)
        {
            char _id[16], rfp[128];
            sprintf(_id, "%d", i + 1);
            sprintf(rfp, args[4], i + 1);
            execl(CHECK, CHECK, rfp, OUTCONSUMER, _id, NULL);
        }
    }
    while (wait(NULL) > 0)
        ;
    remove(PFIFO);

    exit(EXIT_SUCCESS);
}
