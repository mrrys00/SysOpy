//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define SELFDIRPTR "."
#define UPDIRPTR ".."

int block_devs = 0;
int char_devs = 0;
int dirs = 0;
int pipes = 0;
int sym_links = 0;
int files = 0;
int sockets = 0;

void display_info(const char *fpath, struct dirent *sb) {
    // stat OK
    struct stat sptr;
    stat(fpath, &sptr);
    char file_type[16] = {0};
    switch (sb->d_type) {
        case DT_BLK:
            strcat(file_type, "block dev");
            block_devs++;
            break;
        case DT_CHR:
            strcat(file_type, "char dev");
            char_devs++;
            break;
        case DTdelivery - stage 2.IR:
            strcat(file_type, "dir");
            dirs++;
            break;
        case DT_FIFO:
            strcat(file_type, "pipe");
            pipes++;
            break;
        case DT_LNK:
            strcat(file_type, "sym-link");
            sym_links++;
            break;
        case DT_REG:
            strcat(file_type, "file");
            files++;
            break;
        case DT_SOCK:
            strcat(file_type, "socket");
            sockets++;
            break;
    }
    printf("Ścieżka:\t%s\nDowiązania:\t%ld\nRodzaj:\t%s\nRozmiar:\t%ld\nOstatni dostęp:\t%ld\nOstatnia modyfikacja:\t%ld\n", fpath, sptr.st_nlink, file_type, sptr.st_size, sptr.st_atime, sptr.st_mtime);
    return;
}

int special_dir_ptr(struct dirent *direntt) {
    // && prevent going up to high - very important!!!
    return (direntt->d_type == DTdelivery - stage 2.IR && strcmp(direntt->d_name, SELFDIRPTR) && strcmp(direntt->d_name, UPDIRPTR)) ? 1 : 0;
}

void display_wrapper(char *dpath) {
    DIR *dir_ptr = opendir(dpath);
    if (!dir_ptr) return;

    struct dirent *direntt_dir = readdir(dir_ptr), *direntt = readdir(dir_ptr);
    char fpath_dir[PATH_MAX] = {0}, fpath[PATH_MAX] = {0};

    printf("Directory %s\n", realpath(dpath, NULL));
    while (direntt_dir) {
        sprintf(fpath_dir, "%s/%s", dpath, direntt_dir->d_name);
        display_info(fpath_dir, direntt_dir);
        direntt_dir = readdir(dir_ptr);
    }
    // back!
    rewinddir(dir_ptr);

    while (direntt) {
        if (!special_dir_ptr(direntt)) {
            direntt = readdir(dir_ptr);
        } else {
            sprintf(fpath, "%s/%s", dpath, direntt->d_name);
            display_wrapper(fpath);
            direntt = readdir(dir_ptr);
        }
    }
    closedir(dir_ptr);
    return;
}

int main(int argc, char * argv[]) {
    display_wrapper((argc < 2) ? SELFDIRPTR : argv[1]);

    printf("block_devs %d, char_devs %d, dirs %d, pipes %d, sym_links %d, files %d, sockets %d", block_devs, char_devs, dirs, pipes, sym_links, files, sockets);
    exit(EXIT_SUCCESS);
}