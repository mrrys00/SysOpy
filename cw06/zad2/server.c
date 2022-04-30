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

#include "config.h"

typedef struct {
    int queue_id;
    char client_name[128];
} client_t;

int server_enabled = 1;
// message_t in_message = {0L, 0, 0, 0, ""}, out_message = {0L, 0, 0, 0, ""};
client_t clients[MAXCLINUM];


int assign_id()
{
    for (int i = 0; i < MAXCLINUM; i++)
        if (clients[i].queue_id == -1)
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

// int sendint(int msqid, long type, int mto)
// {
//     out_message.mtype = type;
//     out_message.mto = mto;
//     out_message.mfrom = -1;
//     return msgsnd(msqid, &out_message, sizeof(out_message), 0);
// }

void server_poweroff(mqd_t server_queue)
{
    for (int i = 0; i < MAXCLINUM; ++i) {
        if (clients[i].queue_id != -1) {
            message_t client_message;
            client_message.mtype = T_STOP;
            int client_queue_id = clients[i].queue_id;
            mq_send(client_queue_id, (char *)&client_message, sizeof(client_message), client_message.mtype);
            mq_close(client_queue_id);
        }
    }
    mq_close(server_queue);
    mq_unlink(__NAME_SERVER);
    server_enabled = 0;
    printf("All clients closed. Exiting...\n");

    exit(EXIT_SUCCESS);
}

int main()
{
    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(message_t);
    attributes.mq_maxmsg = 10;
    mqd_t server_queue = mq_open(__NAME_SERVER, O_RDWR | O_CREAT | O_EXCL, 0777, &attributes);
    
    struct sigaction act;
    act.sa_handler = server_poweroff;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    int active_clients = 0;

    // key_t key = ftok(KEYPATH, SERVERID);
    // int server_queue = msgget(key, 0666 | IPC_CREAT), client_id, tpos, active_clients = 0;
    // ssize_t rec;
    // int client_queue[MAXCLINUM];

    // signal(SIGINT, server_poweroff);


    for (int i = 0; i < MAXCLINUM; ++i) {
        client_t empty_client;
        empty_client.queue_id = -1;
        clients[i] = empty_client;
    }

    // for (int i = 0; i < MAXCLINUM; i++)
    // {
    //     client_queue[i] = -1;
    // }

    message_t in_message = {0L, 0, 0, 0, "", ""};
    while (server_enabled)
    {
        // rec = msgrcv(server_queue, &in_message, sizeof(in_message), -1000, IPC_NOWAIT);
        // if (mq_receive(server_queue, (char*) &in_message, sizeof(in_message), NULL) == -1) {
        //     server_poweroff(server_queue);
        // }

        switch (in_message.mtype)
        {
        case T_INIT:
            char* client_name = in_message.client_name;
            message_t client_message;
            client_message.mtype = T_INIT;
            mqd_t client_queue = mq_open(client_name, O_RDWR);
            if (active_clients == MAXCLINUM) {
                mq_send(client_queue, (char*) &client_message, sizeof(client_message), client_message.mtype);
                return;
            }
            int client_id = assign_id(client_queue);
            // while (clients[new_client_id].availability != CLIENT_FREE) {
            //     ++new_client_id;
            // }
            // ++active_clients;
            clients[client_id].queue_id = client_queue;
            strcpy(clients[client_id].client_name, client_name);
            mq_send(client_queue, (char*) &client_message, sizeof(client_message), client_message.mtype);

            // client_id = assign_id(client_queue);
            printf("INIT - Assigning ID = %d\n", client_id);
            // client_queue[client_id] = msgget(in_message.mto, 0666);
            // sendint(client_queue[client_id], T_INIT, client_id);
            // client_id++;
            break;
        case T_STOP:
            printf("STOP - Client %d is exiting\n", in_message.mfrom);
            clients[in_message.mfrom].queue_id = -1;
            break;
        }

        // case T_LIST:
        //     printf("LIST - Client %d asks for a list\n", in_message.mfrom);
        //     in_message.mtype = T_LIST;
        //     tpos = 0;
        //     for (int i = 0; i < MAXCLINUM; i++)
        //         if (client_queue[i] != -1)
        //             tpos += sprintf(&in_message.mtext[tpos], "\t- %d is active now\n", i);
        //     msgsnd(client_queue[in_message.mfrom], &in_message, sizeof(in_message), MSG_NOERROR);
        //     break;

        // case T_TOONE:
        //     printf("Trying to send message from client %d to client %d, text: %s\n", in_message.mfrom, in_message.mto, in_message.mtext);
        //     if (in_message.mto < 0 || MAXCLINUM <= in_message.mto || client_queue[in_message.mto] == -1)
        //         sendint(client_queue[in_message.mfrom], T_ERROR, ERR_NOTFOUND);

        //     in_message.mtime = time(NULL);
        //     msgsnd(client_queue[in_message.mto], &in_message, sizeof(in_message), MSG_NOERROR);
        //     log_messages(in_message);
        //     break;

        // case T_TOALL:
        //     printf("Client %d sending message to all clients, text: %s\n", in_message.mfrom, in_message.mtext);
        //     in_message.mtime = time(NULL);
        //     for (int i = 0; i < MAXCLINUM; i++)
        //         if (client_queue[i] != -1 && i != in_message.mfrom) 
        //             msgsnd(client_queue[i], &in_message, sizeof(in_message), MSG_NOERROR);
        //     log_messages(in_message);
        //     break;


        // if (rec > 0)
        //     switch (in_message.mtype)
        //     {
        //     case T_INIT:
        //         client_id = assign_id(client_queue);
        //         printf("INIT - Assigning ID = %d\n", client_id);
        //         client_queue[client_id] = msgget(in_message.mto, 0666);
        //         sendint(client_queue[client_id], T_INIT, client_id);
        //         client_id++;
        //         break;
        //     case T_STOP:
        //         printf("STOP - Client %d is exiting\n", in_message.mfrom);
        //         client_queue[in_message.mfrom] = -1;
        //         break;

        //     case T_LIST:
        //         printf("LIST - Client %d asks for a list\n", in_message.mfrom);
        //         in_message.mtype = T_LIST;
        //         tpos = 0;
        //         for (int i = 0; i < MAXCLINUM; i++)
        //             if (client_queue[i] != -1)
        //                 tpos += sprintf(&in_message.mtext[tpos], "\t- %d is active now\n", i);
        //         msgsnd(client_queue[in_message.mfrom], &in_message, sizeof(in_message), MSG_NOERROR);
        //         break;

        //     case T_TOONE:
        //         printf("Trying to send message from client %d to client %d, text: %s\n", in_message.mfrom, in_message.mto, in_message.mtext);
        //         if (in_message.mto < 0 || MAXCLINUM <= in_message.mto || client_queue[in_message.mto] == -1)
        //             sendint(client_queue[in_message.mfrom], T_ERROR, ERR_NOTFOUND);

        //         in_message.mtime = time(NULL);
        //         msgsnd(client_queue[in_message.mto], &in_message, sizeof(in_message), MSG_NOERROR);
        //         log_messages(in_message);
        //         break;

        //     case T_TOALL:
        //         printf("Client %d sending message to all clients, text: %s\n", in_message.mfrom, in_message.mtext);
        //         in_message.mtime = time(NULL);
        //         for (int i = 0; i < MAXCLINUM; i++)
        //             if (client_queue[i] != -1 && i != in_message.mfrom) 
        //                 msgsnd(client_queue[i], &in_message, sizeof(in_message), MSG_NOERROR);
        //         log_messages(in_message);
        //         break;

        //     }
    }
    // for (int i = 0; i < MAXCLINUM; i++)
    // {
    //     if (client_queue[i] != -1)
    //     {
    //         sendint(client_queue[i], T_STOP, 0);
    //         active_clients++;
    //     }
    // }
    // while (active_clients > 0)
    // {
    //     rec = msgrcv(server_queue, &in_message, sizeof(in_message), T_STOP, IPC_NOWAIT);
    //     if (rec > 0)
    //     {
    //         printf("STOP - Client %d is exiting\n", in_message.mfrom);
    //         client_queue[in_message.mfrom] = -1;
    //         active_clients--;
    //     }
    // }
    // msgctl(server_queue, IPC_RMID, NULL);
}
