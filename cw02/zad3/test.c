#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int a = system("[ -p ./test.c ] && echo OK || err ");
    printf("a = %d\n", a);
    return 0;
}