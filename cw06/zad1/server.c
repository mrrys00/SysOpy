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

int quit = 0;
message_t mes = {0L, 0, 0, ""};
message_t mes_out = {0L, 0, 0, ""};

int findempty(int *client_queue)
{ // find next empty client room
    for (int i = 0; i < MAXCLINUM; i++)
        if (client_queue[i] == -1)
            return i;
    return -1;
}

int keytaken(int checked, int *client_queue, int *keys)
{
    for (int i = 0; i < MAXCLINUM; i++)
        if (client_queue[i] != -1 && keys[i] == checked)
            return 1;
    return 0;
}

int sendint(int msqid, long type, int mto)
{
    mes_out.mtype = type;
    mes_out.mto = mto;
    mes_out.mfrom = -1;
    return msgsnd(msqid, &mes_out, sizeof(mes_out), 0);
}

int sendpacket(int msqid, long type, int mto, char *mtext)
{
    mes_out.mtype = type;
    mes_out.mto = mto;
    mes_out.mfrom = -1;
    strcpy(mes_out.mtext, mtext);
    return msgsnd(msqid, &mes_out, sizeof(mes_out), 0);
}

void stopserver(int signo)
{
    quit = 1;
}

int main()
{
    key_t key;
    int server_queue, client_id, tpos;
    // int p1, p2;
    ssize_t rec;

    signal(SIGINT, stopserver);

    int client_queue[MAXCLINUM];
    // int partner[MAXCLINUM];
    // key_t keys[MAXCLINUM];
    for (int i = 0; i < MAXCLINUM; i++)
    {
        client_queue[i] = -1;
    }

    key = ftok(KEYPATH, SERVERID);

    server_queue = msgget(key, 0666 | IPC_CREAT);

    while (!quit)
    {
        rec = msgrcv(server_queue, &mes, sizeof(mes), -1000, IPC_NOWAIT);
        if (rec > 0)
            switch (mes.mtype)
            {
            case T_INIT:
                client_id = findempty(client_queue);
                printf("INIT - Assigning ID = %d\n", client_id);
                // keys[client_id] = mes.mto;
                client_queue[client_id] = msgget(mes.mto, 0666);
                // partner[client_id] = -1; // set as non-talking
                sendint(client_queue[client_id], T_INIT, client_id);
                client_id++;
                break;
            case T_STOP:
                printf("STOP - Client %d is exiting\n", mes.mfrom);
                // if (partner[mes.mfrom] != -1)
                // {
                //     sendint(client_queue[partner[mes.mfrom]], T_TOALL, 0);
                //     partner[partner[mes.mfrom]] = -1;
                // }
                client_queue[mes.mfrom] = -1;
                // partner[mes.mfrom] = -1;
                break;
            case T_LIST:
                printf("LIST - Client %d asks for a list\n", mes.mfrom);
                mes.mtype = T_LIST;
                tpos = 0;
                for (int i = 0; i < MAXCLINUM; i++)
                {
                    if (client_queue[i] != -1)
                    {
                        tpos += sprintf(&mes.mtext[tpos], "\t%d -> active", i);
                        // if (partner[i] == -1)
                        //     tpos += sprintf(&mes.mtext[tpos], "free\n");
                        // else
                        //     tpos += sprintf(&mes.mtext[tpos], "talking\n");
                    }
                }
                msgsnd(client_queue[mes.mfrom], &mes, sizeof(mes), MSG_NOERROR);
                break;
            case T_TOONE:         // change to 2ONE
                printf("Sending message from client %d to client %d, text: %s\n", mes.mfrom, mes.mto, mes.mtext);
                // sendint(client_queue[p1], T_TOONE, keys[p2]);
                // sendint(client_queue[p2], T_TOONE, keys[p1]);

                // p1 = mes.mfrom;
                // p2 = mes.mto;
                // if (partner[p1] != -1)
                // {
                //     sendint(client_queue[p1], T_ERROR, ERR_BUSY);
                // }
                // else if (p1 == p2)
                // {
                //     sendint(client_queue[p1], T_ERROR, ERR_SELF);
                // }
                // else if (p2 >= MAXCLINUM || p2 < 0 || client_queue[p2] == -1)
                // {
                //     sendint(client_queue[p1], T_ERROR, ERR_NOTFOUND);
                // }
                // else if (partner[p2] != -1)
                // {
                //     sendint(client_queue[p1], T_ERROR, ERR_TAKEN);
                // }
                // else
                // {
                //     partner[p1] = p2;
                //     partner[p2] = p1;
                //     sendint(client_queue[p1], T_TOONE, keys[p2]);
                //     sendint(client_queue[p2], T_TOONE, keys[p1]);
                // }
                break;
            case T_TOALL:      // change to 2ALL
                printf("Client %d sending message to all clients, text: %s\n", mes.mfrom, mes.mtext);
                // p1 = mes.mfrom;
                // // p2 = partner[p1];
                // if (p2 == -1)
                // {
                //     sendint(client_queue[p1], T_ERROR, ERR_NOCONN);
                // }
                // else
                // {
                //     // partner[p1] = -1;
                //     // partner[p2] = -1;
                //     sendint(client_queue[p1], T_TOALL, 0);
                //     sendint(client_queue[p2], T_TOALL, 0);
                // }
                break;
            }
    }
    int running = 0;
    for (int i = 0; i < MAXCLINUM; i++)
    {
        if (client_queue[i] != -1)
        {
            sendint(client_queue[i], T_STOP, 0);
            running++;
        }
    }
    while (running > 0)
    {
        rec = msgrcv(server_queue, &mes, sizeof(mes), T_STOP, IPC_NOWAIT);
        if (rec > 0)
        {
            printf("STOP - Client %d is exiting\n", mes.mfrom);
            client_queue[mes.mfrom] = -1;
            running--;
        }
    }
    printf("All clients closed. Exiting...\n");
    msgctl(server_queue, IPC_RMID, NULL);
}
