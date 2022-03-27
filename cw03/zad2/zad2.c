#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <fcntl.h>
#include <ftw.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <linux/limits.h>

#define WORKDIR "./workdir/"

void start_time_measure(clock_t *s1, struct tms *s2) {
    *s1 = times(s2);
}

void finish_time_measure(clock_t s1, struct tms s2, char* operation_name) {
    char *command = (char*)malloc(200 * sizeof(char));
    int tics_per_second = sysconf(_SC_CLK_TCK);
    clock_t e1;
    struct tms e2;
    double treal, tuser, tsys;

    e1 = times(&e2);
    treal = ((double) (e1 - s1)) / tics_per_second;
    tuser = ((double) (e2.tms_utime - s2.tms_utime)) / tics_per_second;
    tsys  = ((double) (e2.tms_stime - s2.tms_stime)) / tics_per_second;

    sprintf(command,"User time: %f\nSystem time: %f\nReal time: %f\n\n", tuser, tsys, treal);
    printf("Operation %s execution times:\n", operation_name);
    printf(command);
}


double precisive_surfaces(double from_, double to_, double prec) {
    double surface = 0, e = from_ + prec;
    while (e < to_) {
        // rectangle surface = a * h 
        surface += (e-from_) * (4/(((from_+e)/2)*((from_+e)/2)+1));
        from_ = e;
        e += prec;
    }
    return surface;
}

void integration(int n, double prec) {
    double result = 0, from_ = 0, to_ = 1;
    int i = 0;

    while (i < n) {
        double s = (to_ - from_) * i / n, e;
        if (i == n - 1)
            e = to_;
        else 
            e = (to_ - from_) * (i + 1) / n;

        pid_t pid = fork();
        if (pid == 0) {
            double res = precisive_surfaces(s, e, prec);
            char res_path[PATH_MAX]={0}, res_[16]={0};

            // https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
            struct stat st = {0};
            if (stat(WORKDIR, &st) == -1) mkdir(WORKDIR, 0777);
            sprintf(res_path, "%sw%d.txt", WORKDIR, i);
            sprintf(res_, "%lf\n", res);

            int fp = open(res_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            write(fp, res_, strlen(res_) + 1);
            close(fp);
            
            exit(0);
        }
        i++;
    }

    i = 0;
    while (i < n) {
        wait(NULL);
        i++;
    }

    i = 0;
    while (i < n) {
        char res_path[PATH_MAX]={0}, res_[16]={0};
        sprintf(res_path, "%sw%d.txt", WORKDIR, i);

        int fp = open(res_path, O_RDONLY);
        read(fp, res_, 16);
        close(fp);

        double res = strtod(res_, NULL);
        result += res;

        remove(res_path);
        i++;
    }

    printf("Result for n: %d and precision: %lf = %.32f\n", n, prec, result);
    remove(WORKDIR);
    return;
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    } 

    double prec = strtod(argv[1], NULL);
    int n = atoi(argv[2]);

    // double prec = 0.001;
    // int n = 10;

    clock_t fs1;
    struct tms fs2;

    start_time_measure(&fs1, &fs2);

    integration(n, prec);

    finish_time_measure(fs1, fs2, "integration");

    exit(EXIT_SUCCESS);
}
