#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <limits.h>
#include <time.h>
#include "config.h"
#include <errno.h>

#define MAXLINE 512
#define C_NOSUCHCOMMAND -1
#define C_NOTKNOWNARG -2

int client_queue, server_queue, client_id = -1;
pid_t pid = -1;
message_t in_message = {0L, 0, 0, 0, ""}, out_message = {0L, 0, 0, 0, ""};

void str_cut(char *res, char *line, int begin, int len)
{
    int i=begin;
    for (; i<begin+len; i++)
        res[i-begin] = line[i];

    res[i-begin] = '\0';
    return;
}

void str_cut_to_char(char *res, char *line, int begin, int len, char last)
{
    if (line[begin] == ' ') begin++;
    int i=begin;
    for (; i<begin+len; i++)
    {
        if (line[i] == last)
            break;
        res[i-begin] = line[i];
    }

    res[i-begin] = '\0';
    return;
}

int handle(char *line)
{
    char cmd[5];
    str_cut(cmd, line, 0, 4);
    if (strcmp(cmd, "LIST") == 0)
        return T_LIST;
    else if (strcmp(cmd, "2ALL") == 0)
        return T_TOALL;
    else if (strcmp(cmd, "2ONE") == 0)
        return T_TOONE;
    else if (strcmp(cmd, "STOP") == 0)
        return T_STOP;
    return C_NOSUCHCOMMAND;
}

int sendint(int msqid, long type, int mto, char *mtext)
{
    out_message.mtype = type;
    out_message.mto = mto;
    out_message.mfrom = client_id;
    strcpy(out_message.mtext, mtext);
    return msgsnd(msqid, &out_message, sizeof(in_message), 0);
}

void stopSigint(int signo)
{
    if (pid != -1)
        kill(pid, SIGKILL);
    
    sendint(server_queue, T_STOP, client_id, "");
    msgctl(client_queue, IPC_RMID, NULL);
    printf(" Exiting (SIGINT)...\n");
    exit(EXIT_SUCCESS);
}

void stopTerminal()
{
    sendint(server_queue, T_STOP, client_id, "");
    msgctl(client_queue, IPC_RMID, NULL);
    printf(" Exiting (terminal)...\n");
}


int main()
{
    key_t key = ftok(KEYPATH, CLIENTID), server_key = ftok(KEYPATH, SERVERID);
    char user_input[MAXLINE];
    int command, recieved;

    server_queue = msgget(server_key, 0);
    client_queue = msgget(key, 0777 | IPC_CREAT);

    sendint(server_queue, T_INIT, key, "");
    msgrcv(client_queue, &in_message, sizeof(in_message), T_INIT, 0);
    client_id = in_message.mto;

    printf("\nClientID: %d\n", client_id);

    if ((pid = fork()) == 0)
    {
        while (1)
        {
            fgets(user_input, MAXLINE, stdin);
            command = handle(user_input);
            if (command == C_NOTKNOWNARG)
            {
                printf("ERROR: No such command\n");
            }
            else if (command == T_LIST)
            {
                sendint(server_queue, T_LIST, 0, "");
            }
            else if (command == T_STOP)
            {   
                stopTerminal();
                if (pid != -1)
                    kill(pid, SIGKILL);
                exit(EXIT_SUCCESS);
            }
            else if (command == T_TOALL)
            {
                char mtext[MAXLINE];
                str_cut_to_char(mtext, user_input, 5, MAXLINE, '\0');
                sendint(server_queue, T_TOALL, -1, mtext);
            }
            else if (command == T_TOONE)
            {
                char client_to_str[MAXLINE], mtext[MAXLINE];
                int client_to;
                str_cut_to_char(client_to_str, user_input, 5, 3, ' ');
                client_to = atoi(client_to_str);
                str_cut_to_char(mtext, user_input, 5+strlen(client_to_str), MAXLINE, '\0');
                sendint(server_queue, T_TOONE, client_to, mtext);
            }
        }
    }
    else        // reciving messages
    {
        signal(SIGINT, stopSigint);
        while (1)
        {
            recieved = msgrcv(client_queue, &in_message, sizeof(in_message), -1000, IPC_NOWAIT & 0);
            if (recieved > 0)
            {
                switch (in_message.mtype)
                {
                case T_TOALL:
                    printf("message from %d to ALL at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
                    break;

                case T_TOONE:
                    printf("message from %d to you at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
                    break;

                case T_LIST:
                    printf("\n%s\n", in_message.mtext);
                    break;

                case T_ERROR:
                    if (in_message.mto == ERR_NOTFOUND)
                    {
                        printf("WARNING: Client not found\n");
                    }
                    break;

                case T_STOP:
                    stopSigint(SIGINT);
                }
            }
        }
    }
}
