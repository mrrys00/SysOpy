//
// Created by mrrys00 on 3/25/22.
//
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>

#define SPECIAL1 "."
#define SPECIAL2 ".."
#define FILEEXT ".txt"

int is_special(struct dirent *checked_file, DIR* dir_ptr) {
    if (strcmp((char*)checked_file->d_name, SPECIAL1) == 0 || strcmp((char*)checked_file->d_name, SPECIAL2) == 0) {
        return 1;
    }
    return 0;
}

int is_text_file(struct dirent *checked_file) {
    return (strlen(checked_file->d_name) > 3 && strcmp(checked_file->d_name -4 + strlen(checked_file->d_name), FILEEXT) == 0) ? 1 : 0;
}

void search_expression(char *path_dir, char *expressin, int depth, char *start_location) {
    DIR* dir_ptr = opendir(path_dir);
    struct dirent *checked_file = readdir(dir_ptr);
    int exp_len = strlen(expressin);

    while (checked_file != NULL) {
        if (is_special(checked_file, dir_ptr) == 1) {
            checked_file = readdir(dir_ptr);
            continue;
        }

        char* checked_path_file = calloc(strlen(checked_file->d_name) + strlen(path_dir) + 2, sizeof(char));
        if (checked_file->d_name[strlen((char*)checked_file->d_name)-1] != '/') {
            sprintf(checked_path_file, "%s/%s", path_dir, (char*)checked_file->d_name);
        } else {
            sprintf(checked_path_file, "%s%s", path_dir, (char*)checked_file->d_name);
        }

        struct stat file_stat;
        stat(checked_path_file, &file_stat);
        if (S_ISDIR(file_stat.st_mode) && depth > 0) {
            pid_t pid = fork();
            if (pid == 0) {
                search_expression(checked_path_file, expressin, depth - 1, start_location);
                exit(0);
            }
        } else if (strlen((char*)checked_file->d_name) > 3 && is_text_file(checked_file) == 1) {
            char block[MAX_INPUT];
            int fp = open(checked_path_file, O_RDONLY), cnt = 0, exp_iterator = 0;
            int is_printfted = 1;

            while ((cnt = read(fp, block, MAX_INPUT)) > 0 && is_printfted == 1) {
                int i = 0;

                while (i < cnt) {
                    if (block[i] == expressin[exp_iterator]) {
                        exp_iterator++;
                    } else if (exp_len == exp_iterator) {
                        printf("path: %s, pid: %d\n", checked_path_file, getpid());
                        is_printfted = 0;
                        break;
                    } else if(block[i] != expressin[exp_iterator]) {
                        exp_iterator = 0;
                    }
                    i++;
                }
            }
            close(fp);
        }
        free(checked_path_file);
        checked_file = readdir(dir_ptr);
    }

    while(wait(NULL) > 0);
    closedir(dir_ptr);
    return;
}

int main(int argc, char * argv[]) {
    if (argc < 4) {
        exit(EXIT_FAILURE);
    } 

    char* start_path = argv[1];
    char* search_exp = argv[2];
    int max_search_d = atoi(argv[3]);

    search_expression(start_path, search_exp, max_search_d, start_path);

    exit(EXIT_SUCCESS);
}