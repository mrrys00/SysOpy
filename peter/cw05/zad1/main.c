#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

typedef struct {
    char* name;
    char** operations;
    int operations_count;
} Variable;

Variable vars[10000];
int vars_count;

Variable get_var(char* name) {
    for (int i = 0; i < vars_count; ++i) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i];
        }
    }
    printf("variable '%s' not found\n", name);
    exit(EXIT_FAILURE);
}

void parse_line(char* line) {
    int parsed_line_position = 0;
    int line_position = 0;
    int has_encotered_equal_sign = 0;
    while (line[line_position] != '\0') {
        if (line[line_position] == '#') {
            line[parsed_line_position] = '\0';
            return;
        }
        if (line[line_position] == '=') {
            has_encotered_equal_sign = 1;
        }
        if ((line[line_position] != ' ' && line[line_position] != '\t') || has_encotered_equal_sign) {
            line[parsed_line_position] = line[line_position];
            ++parsed_line_position;
        }
        ++line_position;
    }
    if (parsed_line_position > 0 && line[parsed_line_position - 1] == '\n') {
        line[parsed_line_position - 1] = '\0';
    }
    if (parsed_line_position > 1 && line[parsed_line_position - 2] == '\r') {
        line[parsed_line_position - 1] = '\0';
    }
    line[parsed_line_position] = '\0';
}

void copy_operations(char** operations, char** destination, int operations_count, int* destination_first_free_idx) {
    for (int i = 0; i < operations_count; ++i) {
        destination[*destination_first_free_idx + i] = operations[i];
    }
    *destination_first_free_idx += operations_count;
}

void split_operation(char** args, char* operation, int* args_count) {
    char buffer[1000];
    int buffer_length = 0;
    for (int i = 0; operation[i] != '\0'; ++i) {
        if (operation[i] != ' ') {
            buffer[buffer_length] = operation[i];
            ++buffer_length;   
        } else if (buffer_length > 0) {
            buffer[buffer_length] = '\0';
            args[*args_count] = calloc(buffer_length + 1, sizeof(char*));
            strcpy(args[*args_count], buffer);
            buffer_length = 0;
            ++(*args_count);
        }
    }
    if (buffer_length > 0) {
        buffer[buffer_length] = '\0';
        args[*args_count] = calloc(buffer_length + 1, sizeof(char*));
        strcpy(args[*args_count], buffer);
        buffer_length = 0;
        ++(*args_count);
    }
}

void fork_with_pipe(char** operations, int operation_idx, int operations_count, int is_from_mother_process) {
    pid_t child_pid;
    if (!is_from_mother_process) {
        int fd[2];
        pipe(fd);
        child_pid = fork();
        if (child_pid != 0) {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
        } else {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
        }
    } else {
        child_pid = fork();
    }
    if (child_pid == 0) {
        if (operation_idx == operations_count - 1) {
            execl("/bin/bash", "/bin/bash", "-c", operations[operation_idx], NULL);
        } else {
            fork_with_pipe(operations, operation_idx + 1, operations_count, 0);
            execl("/bin/bash", "/bin/bash", "-c", operations[operation_idx], NULL);
        }
    }
}

void execute_operations(char** operations, int operations_count) {
    printf("executing operation: ");
    for (int i = 0; i < operations_count; ++i) {
        if (i == 0) {
            printf(" %s", operations[i]);
        } else {
            printf("|%s", operations[i]);
        }
    }
    printf("\n");
    fork_with_pipe(operations, 0, operations_count, 1);
    while (wait(NULL) > 0);
    printf("\n");
}

void parse_file(FILE* interpreted_file) {
    char buffer[1000];
    while (fgets(buffer, 1000, interpreted_file) != NULL) {
        parse_line(buffer);
        char* line_to_equal_sign = strtok(buffer, "=");
        char* var_value = strtok(NULL, "=");
        if (var_value == NULL) {
            char* operations[1000];
            int operations_count = 0;
            char* variable_name = strtok(line_to_equal_sign, "|");
            if (variable_name == NULL) {
                continue;
            }
            Variable var = get_var(variable_name);
            copy_operations(var.operations, operations, var.operations_count, &operations_count);
            while((variable_name = strtok(NULL, "|")) != NULL) {
                Variable var = get_var(variable_name);
                copy_operations(var.operations, operations, var.operations_count, &operations_count);
            }
            execute_operations(operations, operations_count);
        } else {
            Variable new_var;
            char* variable_name = calloc(strlen(line_to_equal_sign) + 1, sizeof(char));
            strcpy(variable_name, line_to_equal_sign);
            new_var.name = variable_name;
            char* operations[1000];
            int operations_count = 1;
            char* operation = strtok(var_value, "|");
            char* operation_copy = calloc(strlen(operation) + 1, sizeof(char));
            strcpy(operation_copy, operation);
            operations[0] = operation_copy;
            while ((operation = strtok(NULL, "|")) != NULL) {
                char* operation_copy = calloc(strlen(operation) + 1, sizeof(char));
                strcpy(operation_copy, operation);
                operations[operations_count] = operation_copy;
                ++operations_count;
            }
            new_var.operations = calloc(operations_count, sizeof(char*));
            for (int i = 0; i < operations_count; ++i) {
                new_var.operations[i] = operations[i];
            }
            new_var.operations_count = operations_count;
            vars[vars_count] = new_var;
            ++vars_count;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        perror("please pass in one argument");
        return 1;
    }
    FILE* interpreted_file = fopen(argv[1], "r");
    if (interpreted_file == NULL) {
        perror("opening file error");
        return 1;
    }
    parse_file(interpreted_file);
    fclose(interpreted_file);
}
