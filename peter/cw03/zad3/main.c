#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

void search_directory(char* directry_name, char* searched_pattern, int max_depth, char* search_root) {
    DIR* directory_ptr = opendir(directry_name);
    struct dirent* file_in_directory = readdir(directory_ptr);
    while (file_in_directory != NULL) {
        char* filename = file_in_directory->d_name;
        if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            file_in_directory = readdir(directory_ptr);
            continue;
        }
        struct stat file_stats;
        char* current_file_path = calloc(strlen(filename) + strlen(directry_name) + 2, sizeof(char));
        strcpy(current_file_path, directry_name);
        if (directry_name[strlen(directry_name) - 1] != '/') {
            strcat(current_file_path, "/");
        }
        strcat(current_file_path, filename);
        stat(current_file_path, &file_stats);
        if (S_ISDIR(file_stats.st_mode) && max_depth > 1) {
            pid_t pid = fork();
            if (pid == 0) {
                search_directory(current_file_path, searched_pattern, max_depth - 1, search_root);
                exit(0);
            }
        }
        if (strlen(filename) > 3 && strcmp(filename + strlen(filename) - 4, ".txt") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                close(1);
                open("/dev/null", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                execlp("grep", "grep", searched_pattern, current_file_path, NULL);
                exit(1);
            }
            int is_pattern_found = 1;
            waitpid(pid, &is_pattern_found, 0);
            if (is_pattern_found == 0) {
                char* relative_path = current_file_path + strlen(search_root);
                if (relative_path[0] == '/') {
                    relative_path++;
                }
                printf("file: %s found by process with id: %d\n", relative_path, getpid());
            }
        }
        free(current_file_path);
        file_in_directory = readdir(directory_ptr);
    }
    while(wait(NULL) > 0);
    closedir(directory_ptr);
}

int main(int argc, char** argv) {
    char* search_root = argv[1];
    char* searched_pattern = argv[2];
    int max_search_depth = atoi(argv[3]);
    search_directory(search_root, searched_pattern, max_search_depth, search_root);
}
