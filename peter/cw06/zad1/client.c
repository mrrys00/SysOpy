#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
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

void stop_client(int queue_id, int client_id, int server_id) {
    if (client_id == -1) {
        msgctl(queue_id, IPC_RMID, NULL);
        exit(EXIT_SUCCESS);
    }
    message_buffer_t message_buffer;
    message_buffer.mtype = STOP_COMMAND;
    message_buffer.client_id = client_id;
    msgsnd(server_id, &message_buffer, sizeof(message_buffer), 0);
    msgctl(queue_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void handle_init_command(int queue_id, int client_id, int server_id) {
    message_buffer_t message_buffer;
    message_buffer.mtype = INIT_COMMAND;
    message_buffer.client_id = queue_id;
    msgsnd(server_id, &message_buffer, sizeof(message_buffer), 0);
}

void handle_list_command(int queue_id, int client_id, int server_id) {
    message_buffer_t message_buffer;
    message_buffer.mtype = LIST_COMMAND;
    message_buffer.client_id = client_id;
    msgsnd(server_id, &message_buffer, sizeof(message_buffer), 0);
}

void handle_connect_command(int queue_id, int client_id, int server_id, char* arg) {
    int client_to_connect = atoi(arg);
    message_buffer_t message_buffer;
    message_buffer.mtype = CONNECT_COMMAND;
    message_buffer.client_id = client_id;
    message_buffer.other_client_id = client_to_connect;
    msgsnd(server_id, &message_buffer, sizeof(message_buffer), 0);
}

void handle_disconnect_commnand(int queue_id, int client_id, int server_id) {
    message_buffer_t message_buffer;
    message_buffer.mtype = DISCONNECT_COMMAND;
    message_buffer.client_id = client_id;
    msgsnd(server_id, &message_buffer, sizeof(message_buffer), 0);
    stop_client(queue_id, client_id, server_id);
}

void handle_send_command(int queue_id, int client_id, int server_id, char* arg, int other_client_id) {
    if (other_client_id == -1) {
        return;
    }
    client_message_t client_message;
    client_message.mtype = MESSAGE_COMMAND;
    strncpy(client_message.message, arg, MAX_MESSAGE_LENGTH - 1);
    client_message.message[MAX_MESSAGE_LENGTH - 1] = '\0';
    msgsnd(other_client_id, &client_message, sizeof(client_message), 0);
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

int receive_connect_command(client_message_t* client_message) {
    printf("connected with %d\n", client_message->other_client_id);
    return client_message->other_client_id;
}

int receive_init_command(client_message_t* client_message, int queue_id, int server_id) {
    int client_id = client_message->client_id;
    if (client_id == -1) {
        stop_client(queue_id, client_id, server_id);
        return -1;
    }
    printf("client id: %d\n", client_id);
    return client_id;
}

void handle_stdin_received(int queue_id, int client_id, int server_id, int other_client_id) {
    char command[10000];
    scanf("%s", command);
    if (strcmp(command, "STOP") == 0) {
        stop_client(queue_id, client_id, server_id);
        return;
    }
    if (strcmp(command, "DISCONNECT") == 0) {
        handle_disconnect_commnand(queue_id, client_id, server_id);
        return;
    }
    if (strcmp(command, "LIST") == 0) {
        handle_list_command(queue_id, client_id, server_id);
        return;
    }
    char argument[100];
    scanf("%s", argument);
    if (strcmp(command, "SEND") == 0) {
        handle_send_command(queue_id, client_id, server_id, argument, other_client_id);   
    }
    if (strcmp(command, "CONNECT") == 0) {
        handle_connect_command(queue_id, client_id, server_id, argument);
    }
}

int main(int argc, char** argv) {
    key_t server_id = ftok("./server.c", 's');
    int client_id = -1;
    int connected_client_id;
    int queue_id = msgget(IPC_PRIVATE, 0777);
    if (queue_id == -1) {
        stop_client(queue_id, -1, -1);
    }
    int server_queue_id = msgget(server_id, 0);;
    if (server_queue_id == -1) {
        stop_client(queue_id, -1, server_queue_id);
    }
    signal(SIGINT, handle_sigint);
    signal(SIGIO, handle_sigio);
    fcntl(STDIN_FILENO, F_SETOWN, getpid());
    fcntl(STDIN_FILENO, F_SETFL, O_ASYNC);
    handle_init_command(queue_id, client_id, server_queue_id);
    client_message_t client_message;
    while (!should_client_stop) {
        is_io_interrupted = 0;
        msgrcv(queue_id, &client_message, sizeof(client_message), -7, 0);
        if (is_io_interrupted) {
            handle_stdin_received(queue_id, client_id, server_queue_id, connected_client_id);
            continue;
        }
        switch (client_message.mtype) {
            case INIT_COMMAND:       client_id = receive_init_command(&client_message, queue_id, server_queue_id); break;
            case LIST_COMMAND:       receive_list_command(&client_message);                                        break;
            case CONNECT_COMMAND:    connected_client_id = receive_connect_command(&client_message);               break;
            case DISCONNECT_COMMAND: connected_client_id = -1;                                                     break;
            case STOP_COMMAND:       stop_client(queue_id, client_id, server_queue_id);                            break;
            case MESSAGE_COMMAND:    printf("message from other client: %s\n", client_message.message);            break;
            default:                                                                                               break;
        }
    }
    stop_client(queue_id, client_id, server_queue_id);
}
