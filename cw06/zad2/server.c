#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "config.h"

int server_enabled = 1;
message_t in_message = {0L, 0, 0, 0, "", ""}, out_message = {0L, 0, 0, 0, "", ""};

int assign_id(client_t *client_queues)
{
    for (int i = 0; i < MAXCLINUM; i++)
        if (client_queues[i].queue_id == -1)
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

int sendint(mqd_t queue, long type, int mto, int mfrom, char *mtext, char *mname)
{
    out_message.mtype = type;
    out_message.mto = mto;
    out_message.mfrom = mfrom;
    strcpy(out_message.clina, mname);
    strcpy(out_message.mtext, mtext);
    out_message.mtime = time(NULL);
    printf("sending message to queue %d\n", queue);
    return mq_send(queue, (char*) &out_message, sizeof(out_message), type);
}

void server_poweroff(int signo)
{
    server_enabled = 0;
    printf("All clients closed. Exiting...\n");
}

int main()
{
    int client_id, tpos, active_clients = 0;
    ssize_t rec;

    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(message_t);
    attributes.mq_maxmsg = MAXQUEMES;

    struct sigaction act;
    act.sa_handler = server_poweroff;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    mqd_t server_queue = mq_open(__NAME_SERVER, O_RDWR | O_EXCL | O_CREAT , 0777, &attributes);
    if (server_queue == -1)
    {
        mq_unlink(__NAME_SERVER);
        server_queue = mq_open(__NAME_SERVER, O_RDWR | O_EXCL | O_CREAT , 0777, &attributes);
    }
    printf("server_queue: %d\n", server_queue);

    client_t client_queues[MAXCLINUM];

    for (int i = 0; i < MAXCLINUM; i++)
    {
        client_queues[i].queue_id = -1;
        // client_queues[i].client_name = "";
    }

    while (server_enabled)
    {
        rec = mq_receive(server_queue, (char*) &in_message, sizeof(in_message), NULL);
        if (rec > 0)
            switch (in_message.mtype)
            {
            case T_INIT:
                // client_id - pos in cli arr
                // client.queue_id - real cli_id
                client_id = assign_id(client_queues);
                client_queues[client_id].queue_id = in_message.mto;
                strcpy(client_queues[client_id].client_name, in_message.clina);
                mqd_t client_queue = mq_open(client_queues[client_id].client_name, O_RDWR);
                client_queues[client_id].queue_id = client_queue;
                printf("client_queue: %d\n", client_queue);

                sendint(client_queues[client_id].queue_id, T_INIT, client_id, -1, "", in_message.clina);
                client_id++;
                printf("INIT - Assigning ID = %d\n", client_id);
                break;

            case T_STOP:
                printf("STOP - Client %d is exiting\n", in_message.mfrom);
                client_queues[in_message.mfrom].queue_id = -1;
                strcpy(client_queues[in_message.mfrom].client_name, "");
                break;

            case T_LIST:
                printf("LIST - Client %d asks for a list\n", in_message.mfrom);
                in_message.mtype = T_LIST;
                tpos = 0;
                for (int i = 0; i < MAXCLINUM; i++)
                    if (client_queues[i].queue_id != -1)
                        tpos += sprintf(&in_message.mtext[tpos], "\t- %d is active now\n", i);
                mq_send(client_queues[in_message.mfrom].queue_id, (char*) &in_message, sizeof(in_message), MSG_NOERROR);
                break;

            case T_TOONE:
                printf("Trying to send message from client %d to client %d, text: %s\n", in_message.mfrom, in_message.mto, in_message.mtext);
                if (in_message.mto < 0 || MAXCLINUM <= in_message.mto || client_queues[in_message.mto].queue_id == -1)
                    sendint(client_queues[in_message.mfrom].queue_id, T_ERROR, ERR_NOTFOUND, -1, "Erroc: client not found", in_message.clina);

                in_message.mtime = time(NULL);
                sendint(client_queues[in_message.mto].queue_id, T_TOONE, in_message.mto, in_message.mfrom, in_message.mtext, in_message.clina);
                log_messages(in_message);
                break;

            case T_TOALL:
                printf("Client %d sending message to all clients, text: %s\n", in_message.mfrom, in_message.mtext);
                in_message.mtime = time(NULL);
                for (int i = 0; i < MAXCLINUM; i++)
                    if (client_queues[i].queue_id != -1 && i != in_message.mfrom) 
                        sendint(client_queues[i].queue_id, T_TOALL, in_message.mto, in_message.mfrom, in_message.mtext, in_message.clina);
                log_messages(in_message);
                break;

            }
    }

    for (int i = 0; i < MAXCLINUM; i++)
    {
        if (client_queues[i].queue_id != -1)
        {
            sendint(client_queues[i].queue_id, T_STOP, i, -1, "STOP!", in_message.clina);
            active_clients++;
        }
    }
    while (active_clients > 0)
    {
        rec = mq_receive(server_queue, (char*) &in_message, sizeof(in_message), NULL);
        if (rec > 0)
        {
            printf("STOP - Client %d is exiting\n", in_message.mfrom);
            client_queues[in_message.mfrom].queue_id = -1;
            active_clients--;
        }
    }
    mq_close(server_queue);
    mq_unlink(__NAME_SERVER);
}
