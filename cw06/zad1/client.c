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
#define C_NOCOMMAND -1
#define C_DISCONNECT -2
#define C_LIST -3
#define C_BADNUM -4
#define C_STOP -5

#define LIST "LIST"
#define TOALL "2ALL"
#define TOONE "2ONE"
#define STOP "STOP"

int client_queue, server_queue, client_id = -1, partnerQ = -1;
pid_t pid = -1;
message_t mes = {0L, 0, 0, ""}, mes_out = {0L, 0, 0, ""};

void str_cut(char *res, char *line, int begin, int len)
{
    int i=begin;
    for (; i<begin+len; i++)
        res[i-begin] = line[i];

    res[i-begin] = '\0';
    printf("res: %s\n", res);
    return;
}

void str_cut_to_char(char *res, char *line, int begin, int len, char last)
{
    int i=begin;
    for (; i<begin+len; i++)
    {
        // printf("%c\n",line[i]);
        if (line[i] == last)
            break;
        res[i-begin] = line[i];
    }

    res[i-begin] = '\0';
    printf("res: %s\n", res);
    return;
}

int handle(char *line)
{
    char cmd[5];
    str_cut(cmd, line, 0, 4);
    printf("cmd: %s\n", cmd);
    if (strcmp(cmd, LIST) == 0)
    {
        return C_LIST;
    }
    else if (strcmp(cmd, TOALL) == 0)
    {
        return T_TOALL;
    }
    else if (strcmp(cmd, TOONE) == 0)
    {
        return T_TOONE;
    }
    else if (strcmp(cmd, STOP) == 0)
    {
        return C_STOP;
    }
    return C_NOCOMMAND;
}

int sendint(int msqid, long type, int mto, char *mtext)
{
    mes_out.mtype = type;
    mes_out.mto = mto;
    mes_out.mfrom = client_id;
    strcpy(mes_out.mtext, mtext);
    return msgsnd(msqid, &mes_out, sizeof(mes), 0);
}

int sendpacket(int msqid, long type, int mto, char *mtext)
{
    mes_out.mtype = type;
    mes_out.mto = mto;
    mes_out.mfrom = client_id;
    strcpy(mes_out.mtext, mtext);
    return msgsnd(msqid, &mes_out, sizeof(mes), 0);
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
    key_t key = ftok(KEYPATH, CLIENTID), servkey= ftok(KEYPATH, SERVERID);
    char inp[MAXLINE];
    int comm, rec;

    server_queue = msgget(servkey, 0);
    client_queue = msgget(key, 0666 | IPC_CREAT);

    sendint(server_queue, T_INIT, key, "");
    msgrcv(client_queue, &mes, sizeof(mes), T_INIT, 0);
    client_id = mes.mto;

    printf("\nClientID: %d\n", client_id);

    if ((pid = fork()) == 0)
    {
        while (1)
        {
            fgets(inp, MAXLINE, stdin);
            comm = handle(inp);
            printf("input: %s \tcmd: %d\n", inp, comm);
            if (comm == C_BADNUM)
            {
                printf("ERROR: Incorrect argument\n");
            }
            else if (comm == C_LIST)
            {
                sendint(server_queue, T_LIST, 0, "");
            }
            else if (comm == C_STOP)
            {   
                stopTerminal();
                if (pid != -1)
                    kill(pid, SIGKILL);
                exit(EXIT_SUCCESS);
            }
            else if (comm == T_TOALL)
            {
                char mtext[MAXLINE];
                str_cut_to_char(mtext, inp, 5, MAXLINE, '\0');
                sendint(server_queue, T_TOALL, -1, mtext);
            }
            else if (comm == T_TOONE)
            {
                char client_to_str[MAXLINE], mtext[MAXLINE];
                int client_to;
                str_cut_to_char(client_to_str, inp, 5, 3, ' ');
                client_to = atoi(client_to_str);
                str_cut_to_char(mtext, inp, 5+strlen(client_to_str), MAXLINE, '\0');
                printf("client_to: %s\tmtext: %s\n", client_to_str, mtext);
                sendint(server_queue, T_TOONE, client_to, mtext);
            }
            // else
            // {
            //     printf("packet send\n");
            //     sendpacket(client_queue, T_RELAY, client_id, inp);
            // }
        }
    }
    else        // reciving messages
    {
        signal(SIGINT, stopSigint);
        while (1)
        {
            rec = msgrcv(client_queue, &mes, sizeof(mes), -1000, IPC_NOWAIT & 0);
            if (rec > 0)
            {
                switch (mes.mtype)
                {
                // case T_TOONE:
                //     printf("Connection established\n");
                //     partnerQ = msgget(mes.mto, 0);
                //     break;
                case T_CHAT:
                    printf("message> %s", mes.mtext);
                    break;
                // case T_TOALL:
                //     printf("Disconnected\n");
                //     partnerQ = -1;
                //     break;
                // case T_RELAY:
                //     if (partnerQ != -1)
                //         sendpacket(partnerQ, T_CHAT, mes.mto, mes.mtext);
                //     break;
                case T_LIST:
                    printf("\n%s\n", mes.mtext);
                    break;
                case T_ERROR:
                    if (mes.mto == ERR_SELF)
                    {
                        printf("ERR_SELF: Trying to talk with oneself\n");
                    }
                    // else if (mes.mto == ERR_TAKEN)
                    // {
                    //     printf("ERR_SELF: Chosen client is taken\n");
                    // }
                    // else if (mes.mto == ERR_BUSY)
                    // {
                    //     printf("ERR_BUSY: Another connection already established\n");
                    // }
                    else if (mes.mto == ERR_NOTFOUND)
                    {
                        printf("ERR_NOTFOUND: Chosen client not found\n");
                    }
                    // else if (mes.mto == ERR_NOCONN)
                    // {
                    //     printf("ERR_NOCONN: No connection established\n");
                    // }
                    break;
                case T_STOP:
                    stopSigint(SIGINT);
                }
            }
        }
    }
}
