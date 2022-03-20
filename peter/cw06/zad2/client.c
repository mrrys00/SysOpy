#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include "communication.h"

int should_client_stop = 0;
int is_io_interrupted;

void handle_sigint(int signal_number) {
    should_client_stop = 1;
}

void handle_sigio(int signal_number) {
    is_io_interrupted = 1;
}

void stop_client(int queue_id, int client_id, int server_id, char* client_name) {
    if (client_id == -1) {
        mq_close(server_id);
        mq_close(queue_id);
        mq_unlink(client_name);
        exit(EXIT_SUCCESS);
    }
    message_buffer_t message_buffer;
    message_buffer.mtype = STOP_COMMAND;
    message_buffer.client_id = client_id;
    mq_send(server_id, (char*) &message_buffer, sizeof(message_buffer), message_buffer.mtype);
    mq_close(server_id);
    mq_close(queue_id);
    mq_unlink(client_name);
    exit(EXIT_SUCCESS);
}

void handle_init_command(int queue_id, int client_id, int server_id, char* client_name) {
    message_buffer_t message_buffer;
    message_buffer.mtype = INIT_COMMAND;
    message_buffer.client_id = queue_id;
    strcpy(message_buffer.client_name, client_name);
    mq_send(server_id, (char*) &message_buffer, sizeof(message_buffer), message_buffer.mtype);
}

void handle_list_command(int queue_id, int client_id, int server_id) {
    message_buffer_t message_buffer;
    message_buffer.mtype = LIST_COMMAND;
    message_buffer.client_id = client_id;
    mq_send(server_id, (char*) &message_buffer, sizeof(message_buffer), message_buffer.mtype);
}

void handle_connect_command(int queue_id, int client_id, int server_id, char* arg) {
    int client_to_connect = atoi(arg);
    message_buffer_t message_buffer;
    message_buffer.mtype = CONNECT_COMMAND;
    message_buffer.client_id = client_id;
    message_buffer.other_client_id = client_to_connect;
    mq_send(server_id, (char*) &message_buffer, sizeof(message_buffer), message_buffer.mtype);
}

void handle_disconnect_commnand(int queue_id, int client_id, int server_id, char* client_name) {
    message_buffer_t message_buffer;
    message_buffer.mtype = DISCONNECT_COMMAND;
    message_buffer.client_id = client_id;
    mq_send(server_id, (char*) &message_buffer, sizeof(message_buffer), message_buffer.mtype);
}

void handle_send_command(int queue_id, int client_id, int server_id, char* arg, int other_client_id, mqd_t connected_client) {
    if (other_client_id == -1) {
        return;
    }
    client_message_t client_message;
    client_message.mtype = MESSAGE_COMMAND;
    strncpy(client_message.message, arg, MAX_MESSAGE_LENGTH - 1);
    client_message.message[MAX_MESSAGE_LENGTH - 1] = '\0';
    mq_send(connected_client, (char*) &client_message, sizeof(client_message), client_message.mtype);
}

void receive_list_command(client_message_t* client_message) {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        if (client_message->clients[i] != CLIENT_FREE) {
            char* client_status = "waiting for connection";
            if (client_message->clients[i] == CLIENT_UNAVAILABLE) {
                client_status = "client already connected";
            } 
            printf("client id: %d  client status: %s\n", i, client_status);
        }
    }
}

int receive_connect_command(client_message_t* client_message, char* other_client_name, mqd_t* other_client_id) {
    printf("connected with %s\n", client_message->client_name);
    strcpy(other_client_name, client_message->client_name);
    *other_client_id = mq_open(client_message->client_name, O_RDWR);
    return client_message->other_client_id;
}

int receive_init_command(client_message_t* client_message, int queue_id, int server_id, char* client_name) {
    int client_id = client_message->client_id;
    if (client_id == -1) {
        stop_client(queue_id, client_id, server_id, client_name);
        return -1;
    }
    printf("client id: %d\n", client_id);
    return client_id;
}

void handle_stdin_received(int queue_id, int client_id, int server_id, int other_client_id, char* client_name, mqd_t connected_client) {
    char command[10000];
    scanf("%s", command);
    if (strcmp(command, "STOP") == 0) {
        stop_client(queue_id, client_id, server_id, client_name);
        return;
    }
    if (strcmp(command, "DISCONNECT") == 0) {
        handle_disconnect_commnand(queue_id, client_id, server_id, client_name);
        return;
    }
    if (strcmp(command, "LIST") == 0) {
        handle_list_command(queue_id, client_id, server_id);
        return;
    }
    char argument[100];
    scanf("%s", argument);
    if (strcmp(command, "SEND") == 0) {
        handle_send_command(queue_id, client_id, server_id, argument, other_client_id, connected_client);   
    }
    if (strcmp(command, "CONNECT") == 0) {
        handle_connect_command(queue_id, client_id, server_id, argument);
    }
}

int main(int argc, char** argv) {
    int client_id = -1;
    int connected_client_id;
    char client_name[100];
    mqd_t other_client_id = -1;
    char other_client_name[100];
    sprintf(client_name, "%s%d", "/client", getpid());
    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(client_message_t);
    attributes.mq_maxmsg = 10;
    mqd_t queue_id = mq_open(client_name, O_RDWR | O_CREAT | O_EXCL, 0777, &attributes);
    if (queue_id == -1) {
        stop_client(queue_id, -1, -1, client_name);
    }
    mqd_t server_queue_id = mq_open("/server", O_RDWR);
    if (server_queue_id == -1) {
        stop_client(queue_id, -1, server_queue_id, client_name);
    }
    struct sigaction act;
    act.sa_handler = handle_sigint;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    act.sa_handler = handle_sigio;
    sigaction(SIGIO, &act, NULL);
    fcntl(STDIN_FILENO, F_SETOWN, getpid());
    fcntl(STDIN_FILENO, F_SETFL, O_ASYNC);
    handle_init_command(queue_id, client_id, server_queue_id, client_name);
    client_message_t client_message;
    while (!should_client_stop) {
        is_io_interrupted = 0;
        mq_receive(queue_id, (char*) &client_message, sizeof(client_message), NULL);
        if (is_io_interrupted) {
            handle_stdin_received(queue_id, client_id, server_queue_id, connected_client_id, client_name, other_client_id);
            continue;
        }
        switch (client_message.mtype) {
            case INIT_COMMAND:       client_id = receive_init_command(&client_message, queue_id, server_queue_id, client_name);             break;
            case LIST_COMMAND:       receive_list_command(&client_message);                                                                 break;
            case CONNECT_COMMAND:    connected_client_id = receive_connect_command(&client_message, other_client_name, &other_client_id);   break;
            case DISCONNECT_COMMAND: connected_client_id = -1;                                                                              break;
            case STOP_COMMAND:       stop_client(queue_id, client_id, server_queue_id, client_name);                                        break;
            case MESSAGE_COMMAND:    printf("message from other client: %s\n", client_message.message);                                     break;
            default:                                                                                                                        break;
        }
    }
    stop_client(queue_id, client_id, server_queue_id, client_name);
}
