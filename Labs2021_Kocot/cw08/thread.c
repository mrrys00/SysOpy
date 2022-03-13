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
#include <sys/mman.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include "consts.h"

bool white(char c) {
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == EOF);
}

int ceildiv(int a, int b) {
    int higha = a + (b - a % b);
    return higha / b;
}

usec_t get_usec(timeptr tv) {
    usec_t ret = tv->tv_usec % 1000000L;
    ret += tv->tv_sec * 1000000L;
    return ret;
}

usec_t get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return get_usec(&tv);
}

char* readchunk(FILE* fin, char* buf) {
    char c = getc(fin);
    int i = 0;
    while(white(c) && c != EOF) {
        c = getc(fin);
    }
    if(c == '#') {
        while(c != '\n' && c != EOF) {  // whole-line comment
            c = getc(fin);
        }
        c = getc(fin);
    }
    if(c == EOF) return NULL;
    while(!white( c )) {
        if(c == '#') {  // inline comment
            buf[i] = '\0';
            while(c != '\n' && c != EOF) {
                c = getc(fin);
            }
            return buf;
        }
        else buf[i++] = c;
        c = getc(fin);
    }
    buf[i] = '\0';
    return buf;
}

void* numbers_routine(void* varg) {
    usec_t us = get_time();
    numinp *arg = (numinp*) varg;
    int datasize = arg->width * arg->height;
    for(int i = 0; i < datasize; i++) {
        if(arg->minval <= arg->data_in[i] && arg->data_in[i] <= arg->maxval) {
            arg->data_out[i] = MAXVAL - arg->data_in[i];
        }
    }
    us = get_time() - us;
    *(arg->retptr) = us;
    pthread_exit(arg->retptr);
    return NULL;
}
void* blocks_routine(void* varg) {
    usec_t us = get_time();
    numinp *arg = (numinp*) varg;
    if(arg->maxval >= 0 && arg->minval < arg->width && arg->minval <= arg->maxval)
        for(int w = 0; w < arg->width; w++) {
            for(int h = 0; h < arg->height; h++) {
                if(arg->minval <= w && w <= arg->maxval) {
                    arg->data_out[h * arg->width + w] = MAXVAL - arg->data_in[h * arg->width + w];
                }
            }
        }
    us = get_time() - us;
    *(arg->retptr) = us;
    pthread_exit(arg->retptr);
    return NULL;
}

int main(int argc, char * argv[]) {
    // ARGUMENTS: Number_of_threads Mode File_in File_out
    int threadnum = atoi(argv[1]);
    mode_t mode = (argv[2][0] == 'b' || argv[2][0] == 'B' ? BLOCK : NUMBERS);
    FILE *fin = fopen(argv[3], "r");
    FILE *fout = fopen(argv[4], "w");
    if(fin == NULL || fout == NULL) {
        printf("One of the files could not be opened\n");
        return 1;
    }

    char modeN[] = "numbers", modeB[] = "block", *mode_string;
    if(mode == BLOCK) mode_string = modeB; else mode_string = modeN;

    char buf[100];
    readchunk(fin, buf);
    int width = atoi(readchunk(fin, buf));
    int height = atoi(readchunk(fin, buf));
    readchunk(fin, buf);
    int maxval = MAXVAL;  // according to assignment rules

    int *data = malloc(width * height * sizeof(int));
    int *inverted = malloc(width * height * sizeof(int));

    for(int i = 0; i < width * height; i++) {
        data[i] = atoi(readchunk(fin, buf));
    }

    usec_t fulltime = get_time();
    pthread_t *threads = malloc(threadnum * sizeof(pthread_t));
    numinp *args = malloc(threadnum * sizeof(numinp));
    usec_t *rets = malloc(threadnum * sizeof(usec_t));
    for(int i=0; i<threadnum; i++) {
        args[i].data_in = data;
        args[i].data_out = inverted;
        args[i].width = width;
        args[i].height = height;
        args[i].retptr = &rets[i];
        if(mode == BLOCK) {
            args[i].minval = i * ceildiv(width, threadnum);
            args[i].maxval = (i + 1) * ceildiv(width, threadnum) - 1;
            pthread_create(&threads[i], NULL, blocks_routine, &args[i]);
        }
        else {
            args[i].minval = (i == 0 ? 0 : (i * maxval / threadnum) + 1);
            args[i].maxval = (i == threadnum - 1 ? maxval : ((i + 1) * maxval / threadnum));
            pthread_create(&threads[i], NULL, numbers_routine, &args[i]);
        }
    }

    int avres = width * height / threadnum;
    printf("### ~%d pixels per thread (%d/%d). Mode: %s ###\n", avres, width*height, threadnum, mode_string);

    usec_t *t;
    for(int i = 0; i < threadnum; i++) {
        pthread_join(threads[i], (void**) &t);
        printf("Thread %d: %lu us\n", i, *t);
    }

    fulltime = get_time() - fulltime;
    printf("### TOTAL: %lu us ###\n", fulltime);

    fprintf(fout, "P2\n%d %d\n%d\n", width, height, MAXVAL);
    int written = 0;
    for(int i = 0; i < width * height; i++) {
        written += fprintf(fout, "%d ", inverted[i]);
        if(written > 64) {  // for a line not to be over 70 characters
            fprintf(fout, "\n");
            written = 0;
        }
    }

    free(rets);
    free(threads);
    free(args);
    free(data);
    free(inverted);
    fclose(fin);
    fclose(fout);
}

