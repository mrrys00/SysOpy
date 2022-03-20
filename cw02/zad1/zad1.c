//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

int main(int argc, char ** args) {
    char* fn1, fn2;
    if (argc == 0) {
        scanf("%s %s", &fn1, &fn2);
    } else {
        strcpy(fn1, args[1]);
        strcpy(fn2, args[2]);
    }

}
