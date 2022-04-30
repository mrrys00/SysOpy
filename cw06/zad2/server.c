#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include "config.h"

typedef struct {
    int availability;
    int connected_with;
    int queue_id;
    char client_name[100];
} client_t;

int clients_count = 0;
int server_enabled = 1;
client_t clients[MAXCLINUM];

void server_poweroff(int signal_number) {
    server_enabled = 0;
    return;
}

// void handle_disconnect_command(message_t* message_buffer) {
//     int client_id = message_buffer->client_id;
//     if (clients[client_id].availability != CLIENT_UNAVAILABLE) {
//         return;
//     }
//     clients[client_id].availability = CLIENT_AVAILABLE;
//     clients[clients[client_id].connected_with].availability = CLIENT_AVAILABLE;
//     message_t client_message;
//     client_message.mtype = DISCONNECT_COMMAND;
//     int client_queue_id = clients[clients[client_id].connected_with].queue_id;
//     mq_send(client_queue_id, (char*) &client_message, sizeof(client_message), client_message.mtype);
// }

void handle_list_command(message_t* message_buffer) {
    int client_id = message_buffer->client_id;
    message_t client_message;
    client_message.mtype = T_LIST;
    for (int i = 0; i < MAXCLINUM; ++i) {
        client_message.clients[i] = clients[i].availability;
    }
    int client_queue_id = clients[client_id].queue_id;
    mq_send(client_queue_id, (char*) &client_message, sizeof(client_message), client_message.mtype);
}

void handle_init_command(message_t* message_buffer) {
    char* client_name = message_buffer->client_name;
    message_t client_message;
    client_message.mtype = T_INIT;
    mqd_t client_queue = mq_open(client_name, O_RDWR);
    if (clients_count == MAXCLINUM) {
        client_message.client_id = -1;
        mq_send(client_queue, (char*) &client_message, sizeof(client_message), client_message.mtype);
        return;
    }
    int new_client_id = 0;
    while (clients[new_client_id].availability != CLIENT_FREE) {
        ++new_client_id;
    }
    ++clients_count;
    clients[new_client_id].availability = CLIENT_AVAILABLE;
    clients[new_client_id].queue_id = client_queue;
    strcpy(clients[new_client_id].client_name, client_name);
    client_message.client_id = new_client_id;
    mq_send(client_queue, (char*) &client_message, sizeof(client_message), client_message.mtype);
}

void handle_connect_command(message_t* message_buffer) {
    int client_id = message_buffer->client_id;
    int other_client_id = message_buffer->other_client_id;
    message_t client_message;
    client_message.mtype = CONNECT_COMMAND;
    if (!(clients[client_id].availability == CLIENT_AVAILABLE) || !(clients[other_client_id].availability == CLIENT_AVAILABLE)) {
        return;
    }
    clients[client_id].availability = CLIENT_UNAVAILABLE;
    clients[other_client_id].availability = CLIENT_UNAVAILABLE;
    clients[client_id].connected_with = other_client_id;
    clients[other_client_id].connected_with = client_id;
    client_message.other_client_id = clients[other_client_id].queue_id;
    strcpy(client_message.client_name, clients[other_client_id].client_name);
    int client_queue_id = clients[client_id].queue_id;
    mq_send(client_queue_id, (char*) &client_message, sizeof(client_message), client_message.mtype);
    int other_client_queue_id = clients[other_client_id].queue_id;
    client_message.other_client_id = clients[client_id].queue_id;
    strcpy(client_message.client_name, clients[client_id].client_name);
    mq_send(other_client_queue_id, (char*) &client_message, sizeof(client_message), client_message.mtype);
}

void handle_stop_command(message_t* message_buffer) {
    int client_id = message_buffer->client_id;
    int availabilty = clients[client_id].availability;
    mq_close(clients[client_id].queue_id);
    if (availabilty == CLIENT_AVAILABLE) {
        --clients_count;
        clients[client_id].availability = CLIENT_FREE;
    }
    if (availabilty == CLIENT_UNAVAILABLE) {
        --clients_count;
        clients[client_id].availability = CLIENT_FREE;
        clients[clients[client_id].connected_with].availability = CLIENT_AVAILABLE;
        message_t client_message;
        // client_message.mtype = DISCONNECT_COMMAND;
        int client_queue_id = clients[clients[client_id].connected_with].queue_id;
        mq_send(client_queue_id, (char*) &client_message, sizeof(client_message), client_message.mtype);
    }
}

void stop_server(mqd_t queue_id) {
    for (int i = 0; i < MAXCLINUM; ++i) {
        if (clients[i].availability != CLIENT_FREE) {
            message_t client_message;
            client_message.mtype = T_STOP;
            int client_queue_id = clients[i].queue_id;
            mq_send(client_queue_id, (char *)&client_message, sizeof(client_message), client_message.mtype);
            mq_close(client_queue_id);
        }
    }
    mq_close(queue_id);
    mq_unlink("/server");
    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv) {
    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(message_t);
    attributes.mq_maxmsg = 10;
    mqd_t queue_id = mq_open("/server", O_RDWR | O_CREAT | O_EXCL, 0777, &attributes);
    if (queue_id == -1) {
        perror("msgget");
        return EXIT_FAILURE;
    }
    struct sigaction act;
    act.sa_handler = server_poweroff;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    for (int i = 0; i < MAXCLINUM; ++i) {
        client_t empty_client;
        empty_client.availability = CLIENT_FREE;
        empty_client.connected_with = -1;
        empty_client.queue_id = -1;
        clients[i] = empty_client;
    }
    message_t message_buffer;
    while (server_enabled) {
        if (mq_receive(queue_id, (char*) &message_buffer, sizeof(message_buffer), NULL) == -1) {
            stop_server(queue_id);
        }
        switch (message_buffer.mtype) {
            case T_INIT:       handle_init_command(&message_buffer);       break;
            case T_LIST:       handle_list_command(&message_buffer);       break;
            // case CONNECT_COMMAND:    handle_connect_command(&message_buffer);    break;
            // case DISCONNECT_COMMAND: handle_disconnect_command(&message_buffer); break;
            case T_STOP:       handle_stop_command(&message_buffer);       break;
            default:                                                             break;
        }
    }
    stop_server(queue_id);
}