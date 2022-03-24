//
// Created by mrrys00 on 3/20/22.
//
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int block_devs = 0;
int char_devs = 0;
int dirs = 0;
int pipes = 0;
int sym_links = 0;
int files = 0;
int sockets = 0;

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    char file_type[16] = {0};
    switch (sb->st_mode & S_IFMT) {
        case S_IFBLK:
            strcat(file_type, "block dev");
            block_devs++;
            break;
        case S_IFCHR:
            strcat(file_type, "char dev");
            char_devs++;
            break;
        case S_IFDIR:
            strcat(file_type, "dir");
            dirs++;
            break;
        case S_IFIFO:
            strcat(file_type, "pipe");
            pipes++;
            break;
        case S_IFLNK:
            strcat(file_type, "sym-link");
            sym_links++;
            break;
        case S_IFREG:
            strcat(file_type, "file");
            files++;
            break;
        case S_IFSOCK:
            strcat(file_type, "socket");
            sockets++;
            break;
    }
    printf("Ścieżka:\t%s\nDowiązania:\t%ld\nRodzaj:\t%s\nRozmiar:\t%ld\nOstatni dostęp:\t%ld\nOstatnia modyfikacja:\t%ld\n", fpath, sb->st_nlink, file_type, sb->st_size, sb->st_atime, sb->st_mtime);
    return 0;
}

int main(int argc, char * argv[]) {
    int flags = 1|2|4|8;

    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    printf("block_devs %d, char_devs %d, dirs %d, pipes %d, sym_links %d, files %d, sockets %d", block_devs, char_devs, dirs, pipes, sym_links, files, sockets);
    exit(EXIT_SUCCESS);
}