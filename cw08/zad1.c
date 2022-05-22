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

typedef struct
{
    int *i_data;
    int *o_data;
    int width;
    int height;
    int color_range;
    long *retptr;
} thread_args;

int my_atoi(char *str)
{   
    // skąd pomysł na poprawianie czegoś co "działa" czyli my_atoi?
    // konwertując char* nie będącego "intem" do typu int otrzymuję błąd
    // Floating point exception (core dumped)
    // ta funkcja ma zapewnić, że błędy tego typu zostaną obsłużone 
    int res = 0;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] > '9' || str[i] < '0')
            return -1; 
        res = res * 10 + str[i] - '0';
    }

    return res;
}

void err_handling(char *err)
{
    printf("%s\n", err);
    exit(EXIT_FAILURE);
}

long get_time()
{
    // µs timestamps
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long ret = tv.tv_usec % 1000000L;
    ret += tv.tv_sec * 1000000L;
    return ret;
}

char *read_numb(FILE *fip, char *buf)
{
    char c = getc(fip);
    int i = 0;

    // iterujemy aż whitespaces się skończą
    while (c == ' ' || c == '\n' || c == '\r' || c == '\t')
        c = getc(fip);

    // koniec pliku - nic nie mamy w buforze więc ok
    if (c == EOF)
        return NULL;

    // zczytujemy do momentu wykrycia białego znaku - konca liczby
    while (c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != EOF)
    {
        buf[i] = c;
        i++;
        c = getc(fip);
    }

    buf[i] = '\0';
    return buf;
}

void *numbe_proc(void *varg)
{
    long tm_µs = get_time();
    thread_args *arg = (thread_args *)varg;

    int pix_num = arg -> width * arg -> height;

    for (int i = 0; i < pix_num; i++)
        arg -> o_data[i] = arg -> color_range - arg -> i_data[i];

    tm_µs = get_time() - tm_µs;
    *(arg -> retptr) = tm_µs;
    pthread_exit(arg -> retptr);
    return NULL;
}

void *block_proc(void *varg)
{
    long tm_µs = get_time();
    thread_args *arg = (thread_args *)varg;
    
    for (int w = 0; w < arg -> width; w++)
        for (int h = 0; h < arg -> height; h++)
                arg -> o_data[h * arg -> width + w] = arg -> color_range - arg -> i_data[h * arg -> width + w];

    tm_µs = get_time() - tm_µs;
    *(arg -> retptr) = tm_µs;
    pthread_exit(arg -> retptr);
    return NULL;
}

int main(int argc, char *args[])
{
    char *block_str = "block", *numbe_str = "numbers", *mode_str;
    short block_num = 1, numbe_num = 0;
    char buf[100];

    // sprawdzanie poprawności wprowadzonych danych, 
    if (argc < 4)
        err_handling("not enough arguments!");

    int thread_num = my_atoi(args[1]);
    if (thread_num == -1)
        err_handling("invalid integer!");

    short mode;
    if (strcmp(args[2], block_str) == 0) 
    {
        mode = block_num;
        mode_str = block_str;
    }
    else if (strcmp(args[2], numbe_str) == 0) 
    {
        mode = numbe_num;
        mode_str = numbe_str;
    }
    else err_handling("invalid type! expected \"block\" or \"numbers\"");

    FILE *fip = fopen(args[3], "r");
    if (fip == NULL) err_handling("cannot read input file!");
    // czyli wszystko wczytane poprawnie - przetwarzamy

    read_numb(fip, buf);        // to jest wybitnie leniwe przesuniecie miejsca czytania pliku żeby nie przejmować się nagłówkiem P2
    // a linijke niżej zbieranie danych o obrazku
    int width = my_atoi(read_numb(fip, buf)), height = my_atoi(read_numb(fip, buf)), color_range = my_atoi(read_numb(fip, buf));

    // czytamy wsystkie dane zdjęcia
    int *i_data = malloc(width * height * sizeof(int));
    for (int i = 0; i < width * height; i++)
        i_data[i] = my_atoi(read_numb(fip, buf));
    
    fclose(fip);
    // koniec przetwarzania wejścia

    // przygotowujemy się do przetworzenia zdjęcia 
    int *o_data = malloc(width * height * sizeof(int));
    pthread_t *threads = malloc(thread_num * sizeof(pthread_t));
    thread_args *t_args = malloc(thread_num * sizeof(thread_args));
    long *retptr = malloc(thread_num * sizeof(long)), *t, t_times[thread_num];

    long exec_time = get_time();
    // tworzenie wątków
    for (int i = 0; i < thread_num; i++)
    {
        t_args[i].i_data = i_data;
        t_args[i].o_data = o_data;
        t_args[i].width = width;
        t_args[i].height = height;
        t_args[i].retptr = &retptr[i];
        t_args[i].color_range = color_range;
        if (mode == block_num)
            pthread_create(&threads[i], NULL, block_proc, &t_args[i]);
        else
            pthread_create(&threads[i], NULL, numbe_proc, &t_args[i]);
    }

    // start wątków
    for (int i = 0; i < thread_num; i++)
    {
        pthread_join(threads[i], (void **)&t);
        t_times[i] = *t;
    }

    exec_time = get_time() - exec_time; // koniec pomiaru czasu

    free(retptr);
    free(threads);
    free(t_args);
    free(i_data);

    // wypis danych na temrinal - makefile pokieruje do raportu
    printf("%s\t %lu\t %d\t", mode_str, exec_time, thread_num);
    for (int i = 0; i < thread_num; i++)
        printf("%lu\t", t_times[i]);
    printf("\n");

    // zapisanie pliku wyjściowego
    FILE *fop = fopen(args[4], "w");
    if (fop == NULL) err_handling("cannot write output file!");
    int space_cnt = 0;
    fprintf(fop, "P2\n%d %d\n%d\n", width, height, color_range);
    for (int i = 0; i < width * height; i++)
    {
        fprintf(fop, "%d ", o_data[i]);
        space_cnt++;
        if (space_cnt == 19)    // dlaczego 19? a no dlatego, że każda linia pliku mojego zdjęcia zawiera 19 spacji - chaciałem stworzyć tak samo plik wyjściowy
        { 
            fprintf(fop, "\n");
            space_cnt = 0;
        }
    }
    free(o_data);
    fclose(fop);

    exit(EXIT_SUCCESS);
}
