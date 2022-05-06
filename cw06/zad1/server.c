#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "config.h"

int server_enabled = 1;
message_t in_message = {0L, 0, 0, 0, ""}, out_message = {0L, 0, 0, 0, ""};

int assign_id(int *client_queue)
{
    for (int i = 0; i < MAXCLINUM; i++)
        if (client_queue[i] == -1)
            return i;
    return -1;
}

FILE *start_log()
{
    if (access(LOGNAME, F_OK) == 0)
        return fopen(LOGNAME, "a");
    return fopen(LOGNAME, "w");
}

void log_messages(message_t mess)
{
    FILE *fp = start_log();
    char *log = (char*)malloc((MAXMESLEN + 64) * sizeof(char));
    sprintf(log, "timestamp: %ld\tfrom: %d\tmessage: %s\n", mess.mtime, mess.mfrom, mess.mtext);
    fwrite(log, 1, strlen(log), fp);
    fclose(fp);
    free(log);
    return;
}

int sendint(int msqid, long type, int mto)
{
    out_message.mtype = type;
    out_message.mto = mto;
    out_message.mfrom = -1;
    return msgsnd(msqid, &out_message, sizeof(out_message), 0);
}

void server_poweroff(int signo)
{
    server_enabled = 0;
    return;
}

int main()
{
    key_t key = ftok(KEYPATH, SERVERID);
    int server_queue = msgget(key, 0777 | IPC_CREAT), client_id, tpos, active_clients = 0;
    ssize_t rec;
    int client_queue[MAXCLINUM];

    signal(SIGINT, server_poweroff);

    for (int i = 0; i < MAXCLINUM; i++)
    {
        client_queue[i] = -1;
    }

    while (server_enabled)
    {
        rec = msgrcv(server_queue, &in_message, sizeof(in_message), -1000, IPC_NOWAIT);
        if (rec > 0)
            switch (in_message.mtype)
            {
            case T_INIT:
                client_id = assign_id(client_queue);
                printf("INIT - Assigning ID = %d\n", client_id);
                client_queue[client_id] = msgget(in_message.mto, 0777);
                sendint(client_queue[client_id], T_INIT, client_id);
                client_id++;
                break;
            case T_STOP:
                printf("STOP - Client %d is exiting\n", in_message.mfrom);
                client_queue[in_message.mfrom] = -1;
                break;

            case T_LIST:
                printf("LIST - Client %d asks for a list\n", in_message.mfrom);
                in_message.mtype = T_LIST;
                tpos = 0;
                for (int i = 0; i < MAXCLINUM; i++)
                    if (client_queue[i] != -1)
                        tpos += sprintf(&in_message.mtext[tpos], "\t- %d is active now\n", i);
                msgsnd(client_queue[in_message.mfrom], &in_message, sizeof(in_message), MSG_NOERROR);
                break;

            case T_TOONE:
                printf("Trying to send message from client %d to client %d, text: %s\n", in_message.mfrom, in_message.mto, in_message.mtext);
                if (in_message.mto < 0 || MAXCLINUM <= in_message.mto || client_queue[in_message.mto] == -1)
                    sendint(client_queue[in_message.mfrom], T_ERROR, ERR_NOTFOUND);

                in_message.mtime = time(NULL);
                msgsnd(client_queue[in_message.mto], &in_message, sizeof(in_message), MSG_NOERROR);
                log_messages(in_message);
                break;

            case T_TOALL:
                printf("Client %d sending message to all clients, text: %s\n", in_message.mfrom, in_message.mtext);
                in_message.mtime = time(NULL);
                for (int i = 0; i < MAXCLINUM; i++)
                    if (client_queue[i] != -1 && i != in_message.mfrom) 
                        msgsnd(client_queue[i], &in_message, sizeof(in_message), MSG_NOERROR);
                log_messages(in_message);
                break;

            }
    }
    for (int i = 0; i < MAXCLINUM; i++)
    {
        if (client_queue[i] != -1)
        {
            sendint(client_queue[i], T_STOP, 0);
            active_clients++;
        }
    }
    while (active_clients > 0)
    {
        rec = msgrcv(server_queue, &in_message, sizeof(in_message), T_STOP, IPC_NOWAIT);
        if (rec > 0)
        {
            printf("STOP - Client %d is exiting\n", in_message.mfrom);
            client_queue[in_message.mfrom] = -1;
            active_clients--;
        }
    }
    msgctl(server_queue, IPC_RMID, NULL);
    printf("All clients closed. Exiting...\n");
}
