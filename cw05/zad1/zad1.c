#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <linux/limits.h>

#define INDEPENDENTLENGTH 2048

typedef struct
{
    char *name;
    char **commands;
    int commandc;
} ingredient;

int process_line(char *l_buf)
{
    int process_idx = 0, iterator = 0, is_equal = 0;
    while (l_buf[iterator] != '\0')
    {
        if (!is_equal && l_buf[iterator] == '=')
            is_equal = 1;
        if ((l_buf[iterator] != ' ' && l_buf[iterator] != '\t') || is_equal)
        {
            l_buf[process_idx] = l_buf[iterator];
            process_idx++;
        }
        iterator++;
    }
    if (process_idx > 0 && l_buf[process_idx - 1] == '\n')
    {
        l_buf[process_idx - 1] = '\0';
        return 0;
    }
    l_buf[process_idx] = '\0';

    return 0;
}

void cp_cmds(char **commands, char **dest, int commandc, int *dest_left_free)
{
    for (int i = 0; i < commandc; i++)
        dest[*dest_left_free + i] = commands[i];
    *dest_left_free += commandc;

    return;
}

ingredient get_cmd(char *name, int ing_cnt, ingredient ings[])
{
    int i = 0;
    while(i < ing_cnt)
    {
        if (strcmp(ings[i].name, name) == 0)
            return ings[i];
        i++;
    }

    ingredient err;
    err.name = "command not found";
    err.commands = NULL;
    err.commandc = -1;

    return err;
}

void rec_executor(char **commands, int command_idx, int commandc, int same_parent)
{
    pid_t child_pid;
    if (same_parent)
        child_pid = fork();
    else
    {
        int fd[2];
        pipe(fd);
        child_pid = fork();
        if (child_pid == 0)
        {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
        }
        else
        {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
        }
    }
    if (child_pid == 0)
    {
        if (command_idx != commandc - 1)        // recursive exec needed
        {
            rec_executor(commands, command_idx + 1, commandc, 0);
            execl("/bin/bash", "/bin/bash", "-c", commands[command_idx], NULL);
        }
        else
            execl("/bin/bash", "/bin/bash", "-c", commands[command_idx], NULL);
    }
    return;
}

int executor(char *command_definition, int ing_cnt, ingredient ings[])
{
    char *commands[INDEPENDENTLENGTH];
    int commandc = 0;
    
    ingredient var = get_cmd(command_definition, ing_cnt, ings);
    if (var.commandc == -1) return 1;
    cp_cmds(var.commands, commands, var.commandc, &commandc);
    while ((command_definition = strtok(NULL, "|")) != NULL)
    {
        ingredient var = get_cmd(command_definition, ing_cnt, ings);
        if (var.commandc == -1) return 1;
        cp_cmds(var.commands, commands, var.commandc, &commandc);
    }

    rec_executor(commands, 0, commandc, 1);
    while (wait(NULL) > 0);     // wait till end

    return 0;
}

int ingredient_catcher(char *before_equal_separator, char *cmd_content, int *ing_cnt, ingredient ings[])
{
    ingredient ing;
    char *command_definition = calloc(strlen(before_equal_separator) + 1, sizeof(char));
    strcpy(command_definition, before_equal_separator);
    ing.name = command_definition;
    char *commands[INDEPENDENTLENGTH];
    int commandc = 1;
    char *command = strtok(cmd_content, "|");
    char *operation_copy = calloc(strlen(command) + 1, sizeof(char));
    strcpy(operation_copy, command);
    commands[0] = operation_copy;
    while ((command = strtok(NULL, "|")) != NULL)
    {
        char *operation_copy = calloc(strlen(command) + 1, sizeof(char));
        strcpy(operation_copy, command);
        commands[commandc] = operation_copy;
        commandc++;
    }
    ing.commands = calloc(commandc, sizeof(char *));
    for (int i = 0; i < commandc; i++)
    {
        ing.commands[i] = commands[i];
    }
    ing.commandc = commandc;
    ings[*ing_cnt] = ing;
    (*ing_cnt)++;
    return 0;
}

int interpreter(FILE *fp)
{
    char buf[_SC_LINE_MAX];
    ingredient ings[4*INDEPENDENTLENGTH];
    int ing_cnt = 0;

    while (fgets(buf, _SC_LINE_MAX, fp) != NULL)
    {
        if (process_line(buf)) return 1;
        char *before_equal_separator = strtok(buf, "="), *cmd_content = strtok(NULL, "=");
        if (cmd_content == NULL)          // line with command execution
        {
            char *command_definition = strtok(before_equal_separator, "|");
            if (command_definition == NULL) continue;
            else if (executor(command_definition, ing_cnt, ings)) return 1;
        }
        else                            // line with command definition
            if (ingredient_catcher(before_equal_separator, cmd_content, &ing_cnt, ings)) return 1;
    }
    return 0;
}

int main(int argc, char *args[])
{
    if (argc < 2)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(args[1], "r");
    if (interpreter(fp))
        exit(EXIT_FAILURE);
    fclose(fp);

    exit(EXIT_SUCCESS);
}
