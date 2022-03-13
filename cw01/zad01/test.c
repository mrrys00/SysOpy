//
// Created by mrrys00 on 3/13/22.
//
#include <stdio.h>
#include <stdlib.h>

#define FILENAME "test.c"
#define CNTFILE "cnt_file"
#define AWKUTIL "| awk '{print $1}'"

int main(int argc, char * argv[]) {
    FILE fp_in = fopen(FILENAME);
    FILE fp_out = fopen(CNTFILE);
    char *command = (char*)malloc(200 * sizeof(char));
    sprintf(command, "wc -l %s %s > %s && wc -w %s %s >> %s && wc -m %s %s >> %s", FILENAME, AWKUTIL, CNTFILE, FILENAME, AWKUTIL, CNTFILE, FILENAME, AWKUTIL, CNTFILE);
//    char command[200] = "wc -l " + FILENAME + " > " + CNTFILE + " && wc -w " + FILENAME + " >> " + CNTFILE + " && wc -m " + FILENAME + " >> " + CNTFILE;
    system(command);

    printf("argc %d", argc);

    return 0;
}
