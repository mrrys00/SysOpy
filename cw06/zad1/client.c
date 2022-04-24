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

#define MAXLINE 128
#define C_NOCOMMAND -1
#define C_DISCONNECT -2
#define C_LIST -3
#define C_BADNUM -4
#define C_HELP -5

#define LIST "LIST"
#define TOALL "2ALL"
#define TOONE "2ONE"
#define STOP "STOP"

int client_queue, server_queue, client_id = -1, partnerQ = -1;
pid_t pid = -1;
message_t mes = {0L, 0, 0, ""}, mes_out = {0L, 0, 0, ""};

char *str_cut(char *line, int begin, int len)
{
    char res[len];
    for (int i=begin; i<begin+len; i++)
        res[i] = line[i];

    return res;
}

int handle(char *line)
{
    char *cmd = str_cut(line, 0, 4);
    if (strcmp(cmd, LIST) == 0)
    {

    }
    else if (strcmp(cmd, TOALL) == 0)
    {

    }
    else if (strcmp(cmd, TOONE) == 0)
    {

    }
    else if (strcmp(cmd, STOP) == 0)
    {

    }
    return C_NOCOMMAND;
}

int sendint(int msqid, long type, int mint)
{
    mes_out.mtype = type;
    mes_out.mint = mint;
    mes_out.mfrom = client_id;
    return msgsnd(msqid, &mes_out, sizeof(mes), 0);
}

int sendpacket(int msqid, long type, int mint, char *mtext)
{
    mes_out.mtype = type;
    mes_out.mint = mint;
    mes_out.mfrom = client_id;
    strcpy(mes_out.mtext, mtext);
    return msgsnd(msqid, &mes_out, sizeof(mes), 0);
}

void stop(int signo)
{
    if (pid != -1)
        kill(pid, SIGKILL);
    
    sendint(server_queue, T_STOP, client_id);
    msgctl(client_queue, IPC_RMID, NULL);
    printf(" Exiting...\n");
    exit(EXIT_SUCCESS);
}


int main()
{
    key_t key = ftok(KEYPATH, CLIENTID), servkey= ftok(KEYPATH, SERVERID);
    char inp[MAXLINE];
    int comm, rec;

    server_queue = msgget(servkey, 0);
    client_queue = msgget(key, 0666 | IPC_CREAT);

    sendint(server_queue, T_INIT, key);
    msgrcv(client_queue, &mes, sizeof(mes), T_INIT, 0);
    client_id = mes.mint;

    printf("\nClientID: %d\n", client_id);

    if ((pid = fork()) == 0)
    {
        while (1)
        {
            fgets(inp, MAXLINE, stdin);
            comm = handle(inp);
            if (comm == C_DISCONNECT)
            {
                sendint(server_queue, T_DISCONNECT, 0);
            }
            else if (comm == C_BADNUM)
            {
                printf("ERROR: Incorrect argument\n");
            }
            else if (comm == C_LIST)
            {
                sendint(server_queue, T_LIST, 0);
            }
            else if (comm >= 0)
            {
                sendint(server_queue, T_CONNECT, comm);
            }
            else
            {
                sendpacket(client_queue, T_RELAY, client_id, inp);
            }
        }
    }
    else        // reciving messages
    {
        signal(SIGINT, stop);
        while (1)
        {
            rec = msgrcv(client_queue, &mes, sizeof(mes), -1000, IPC_NOWAIT & 0);
            if (rec > 0)
            {
                switch (mes.mtype)
                {
                // case T_CONNECT:
                //     printf("Connection established\n");
                //     partnerQ = msgget(mes.mint, 0);
                //     break;
                case T_CHAT:
                    printf("mesdsage> %s", mes.mtext);
                    break;
                // case T_DISCONNECT:
                //     printf("Disconnected\n");
                //     partnerQ = -1;
                //     break;
                // case T_RELAY:
                //     if (partnerQ != -1)
                //         sendpacket(partnerQ, T_CHAT, mes.mint, mes.mtext);
                //     break;
                case T_LIST:
                    printf("\n%s\n", mes.mtext);
                    break;
                case T_ERROR:
                    if (mes.mint == ERR_SELF)
                    {
                        printf("ERR_SELF: Trying to talk with oneself\n");
                    }
                    // else if (mes.mint == ERR_TAKEN)
                    // {
                    //     printf("ERR_SELF: Chosen client is taken\n");
                    // }
                    // else if (mes.mint == ERR_BUSY)
                    // {
                    //     printf("ERR_BUSY: Another connection already established\n");
                    // }
                    else if (mes.mint == ERR_NOTFOUND)
                    {
                        printf("ERR_NOTFOUND: Chosen client not found\n");
                    }
                    // else if (mes.mint == ERR_NOCONN)
                    // {
                    //     printf("ERR_NOCONN: No connection established\n");
                    // }
                    break;
                case T_STOP:
                    stop(SIGINT);
                }
            }
        }
    }
}
