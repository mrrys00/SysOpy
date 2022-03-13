#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <limits.h>
#include <time.h>

#define bool short
#define true 1
#define false 0
#define MINSLEEP 1.0  // minimum sleep time in seconds
#define MAXSLEEP 2.0  // maximum sleep time in seconds

void randsleep() {
    int rnd = rand() % (1000);
    unsigned usec = 1e6 * MINSLEEP + 1e3 * rnd * (MAXSLEEP - MINSLEEP);
    usleep(usec);
}

bool sendQuit(char* opath, int N) {
    FILE *opipe = fopen(opath, "w");
    if(opipe == NULL) return false;
    else {
        char *empty_packet = (char*) calloc(N+1, sizeof(char));
        fwrite(empty_packet, sizeof(char), N+1, opipe);  // empty packet will tell consumer to exit
        fclose(opipe);
        free(empty_packet);
        return true;
    }
}

int main(int argc, char * argv[]) {
    if(argc < 5) {
        if(argc == 3) {
            if(sendQuit(argv[1], atoi(argv[2]))) return 0;  // sent QUIT packet to consumer
        }
        else {
            perror("Wrong number of arguments");
            return 1;
        }
    }
    int opipe;
    int id = atoi(argv[2]);
    FILE *ifile = fopen(argv[3], "r");
    int N = atoi(argv[4]);
    if(ifile == NULL) {
        perror("Couldn't open the file");
        return 1;
    }
    if(N > PIPE_BUF) {
        printf("N too large. Using %d instead\n", PIPE_BUF);
        N = PIPE_BUF;
    }

    char *packet = (char*) calloc(N+2, sizeof(char));  // first byte is the line id
    packet[0] = id;
    char *buf = &(packet[1]);  // only the string
    int bytes_read;
    
    srand(time(0));

    while((bytes_read = fread(buf, sizeof(char), N, ifile)) > 0) {
        opipe = open(argv[1], O_WRONLY);
        if(bytes_read < N) buf[bytes_read] = '\0';
        write(opipe, packet, N+1);
        close(opipe);
        randsleep();
    }
    
    fclose(ifile);
    free(packet);
}

/*Producent:

- przyjmuje cztery argumenty: ścieżka do potoku nazwanego, numer wiersza, ścieżka do pliku tekstowego z dowolną zawartością, N - liczba znaków odczytywanych jednorazowo z pliku
- otwiera potok nazwany
- wielokrotnie (aż do odczytania całego pliku):
    > odczekuje losową ilość czasu (np. 1-2 sekund)
    > zapisuje do potoku nazwanego: numer wiersza oraz odczytany fragment pliku (N odczytanych znaków)
*/