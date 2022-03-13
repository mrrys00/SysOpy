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
#define MAXIDBYTES 1

bool quit = false;

int main(int argc, char * argv[]) {
    if(argc < 4) {
        perror("Too few arguments");
        return 1;
    }
    int ipipe;
    FILE *ofile;
    int N = atoi(argv[3]);
    if(N > PIPE_BUF) {
        printf("N too large. Using %d instead\n", PIPE_BUF);
        N = PIPE_BUF;
    }

    int chars, lineid;
    int outlines = 1;
    int *lens = (int*) calloc(1, sizeof(int));
    char **outbuf = (char**) malloc(sizeof(char*));
    outbuf[0] = (char*) calloc(1, sizeof(char));

    char *packet = (char*) calloc(N+2, sizeof(char));  // first byte is the line id
    char *buf = &(packet[1]);  // only the string
    while(!quit) {
        ipipe = open(argv[1], O_RDONLY);
        while(read(ipipe, packet, N+1)) {
            if(buf[0] == '\0') {  // QUIT packet received
                quit = true;
            }
            else {
                lineid = (int) packet[0] - 1;  // we need from 0, producers count from 1
                chars = 0;
                while(buf[chars] != '\0') chars++;
                if(outlines < lineid + 1) {
                    outbuf = (char**) realloc(outbuf, (lineid + 1) * sizeof(char*));
                    lens = (int*) realloc(lens, (lineid + 1) * sizeof(char*));
                    if(lens == NULL || outbuf == NULL) {
                        printf("Reallocation error\n");
                        raise(SIGABRT);
                    }
                    while(outlines < lineid + 1) {
                        outbuf[outlines] = (char*) calloc(1, sizeof(char));
                        lens[outlines] = 0;
                        outlines++;
                    } 
                }
                outbuf[lineid] = (char*) realloc(outbuf[lineid], (lens[lineid]+1+chars) * sizeof(char));
                for(int i=0; i<chars; i++) {
                    outbuf[lineid][lens[lineid]++] = buf[i];
                }
                outbuf[lineid][lens[lineid]] = '\0';
            }
        }
        close(ipipe);
    }
    ofile = fopen(argv[2], "w");
    if(ofile == NULL) {
        perror("Couldn't open the file");
        return 1;
    }
    for(int i=0; i<outlines; i++) {
        fprintf(ofile, "%s\n", outbuf[i]);
        free(outbuf[i]);
    }
    free(outbuf);
    free(lens);

    fclose(ofile);
    free(packet);
}

/*Konsument:

- przyjmuje trzy argumenty: ścieżka do potoku nazwanego, ścieżka do pliku tekstowego (do którego będzie zapisywany odczytany tekst), N — liczba znaków odczytywanych jednorazowo z pliku
- otwiera potok nazwany
- wielokrotnie:
    - odczytuje numer wiersza i oraz N kolejnych znaków potoku nazwanego
    - umieszcza odczytane znaki w linii nr i pliku tekstowego (różnym od plików, z których korzystają producenci)
*/