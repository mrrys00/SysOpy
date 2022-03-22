//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <ftw.h>
#include <stdint.h>
#define _XOPEN_SOURCE 500

struct FTW {
    int base;
    int level;
};

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    char command[4096];
    sprintf(command, "[ -p \"%s\" ] && echo \"\" || err", fpath);
    printf("%-3s %2d %7jd   %-40s %d %s\n",
           (tflag == FTW_D) ?   "dir" :
           (tflag == FTW_DNR) ? "dnr" :
           (tflag == FTW_SL) ?  "sym-link" :
           (system(command) == 0) ? "pipe" :
           (tflag == FTW_F) ?   "file" :
           (tflag == FTW_NS) ?  "ns" :
           "???",
           ftwbuf->level, (intmax_t) sb->st_size, fpath, ftwbuf->base, fpath + ftwbuf->base);
    return 0;           /* To tell nftw() to continue */
}

int main(int argc, char * argv[]) {
//    int nftw_status = -1;
//    if (argc < 2) {
//        printf("no directory selected\n");
//        exit(1);
//    }

    int flags = 1|2|4|8;

//    if (argc > 2 && strchr(argv[2], 'd') != NULL)
//        flags |= FTW_DEPTH;
//    if (argc > 2 && strchr(argv[2], 'p') != NULL)
//        flags |= FTW_PHYS;

    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}