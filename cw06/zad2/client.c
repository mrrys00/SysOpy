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
#include <mqueue.h>
#include <fcntl.h>
#include "config.h"
#include <errno.h>

#define MAXLINE 512
#define C_NOSUCHCOMMAND -1
#define C_NOTKNOWNARG -2

mqd_t client_queue, server_queue, client_id = -1;
pid_t pid = -1;
char client_name[128];
message_t in_message = {0L, 0, 0, 0, ""}, out_message = {0L, 0, 0, 0, ""};
int client_enabled = 1;

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
    printf("handle OK\n");
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
    return mq_send(msqid, (char*) &out_message, sizeof(in_message), out_message.mtype);
}

void stopSigint(int signo)
{
    if (pid != -1)
        kill(pid, SIGKILL);
    
    sendint(server_queue, T_STOP, client_id, "");
    // msgctl(client_queue, IPC_RMID, NULL);
    mq_close(server_queue);
    mq_close(client_queue);
    mq_unlink(client_name);
    printf(" Exiting (SIGINT)...\n");
    exit(EXIT_SUCCESS);
}

void stopTerminal()
{
    sendint(server_queue, T_STOP, client_id, "");
    // msgctl(client_queue, IPC_RMID, NULL);
    mq_close(server_queue);
    mq_close(client_queue);
    mq_unlink(client_name);
    printf(" Exiting (terminal)...\n");
}


int main()
{
    char user_input[MAXLINE];
    int command, recieved;

    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(message_t);
    attributes.mq_maxmsg = 16;

    server_queue = mq_open(__NAME_SERVER, O_RDWR);
    client_queue = mq_open(__NAME_CLIENT, O_RDWR | O_CREAT | O_EXCL, 0777, &attributes);

    // sendint(server_queue, T_INIT, key, "");
    struct sigaction act;
    act.sa_handler = stopSigint;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    // act.sa_handler = handle_sigio;
    // sigaction(SIGIO, &act, NULL);
    fcntl(STDIN_FILENO, F_SETOWN, getpid());
    fcntl(STDIN_FILENO, F_SETFL, O_ASYNC);
    mq_receive(client_queue, (char*) &in_message, sizeof(in_message), NULL);
    client_id = in_message.mto;

    printf("\nClientID: %d\n", client_id);

    sprintf(client_name, "%s%d", __NAME_CLIENT, getpid());
    strcpy(out_message.client_name, client_name);
    mq_send(server_queue, (char*) &out_message, sizeof(out_message), out_message.mtype);


    // if ((pid = fork()) == 0)
    // {
    while (client_enabled)
    {
        recieved = mq_receive(client_queue, (char*) &in_message, sizeof(in_message), NULL);
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
        printf("eeOK\n");
        fgets(user_input, MAXLINE, stdin);
        command = handle(user_input);
        printf("eeOK2\n");

        if (command == C_NOTKNOWNARG)
        {
            printf("ERROR: No such command\n");
        }
        else if (command == T_LIST)
        {
            printf("eeOK2\n");
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
    // }
    // else        // reciving messages
    // {
    //     signal(SIGINT, stopSigint);
    //     while (1)
    //     {
    //         recieved = mq_receive(client_queue, (char*) &in_message, sizeof(in_message), NULL);
    //         if (recieved > 0)
    //         {
    //             switch (in_message.mtype)
    //             {
    //             case T_TOALL:
    //                 printf("message from %d to ALL at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
    //                 break;

    //             case T_TOONE:
    //                 printf("message from %d to you at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
    //                 break;

    //             case T_LIST:
    //                 printf("\n%s\n", in_message.mtext);
    //                 break;

    //             case T_ERROR:
    //                 if (in_message.mto == ERR_NOTFOUND)
    //                 {
    //                     printf("WARNING: Client not found\n");
    //                 }
    //                 break;

    //             case T_STOP:
    //                 stopSigint(SIGINT);
    //             }
    //         }
    //     }
    // }
}
