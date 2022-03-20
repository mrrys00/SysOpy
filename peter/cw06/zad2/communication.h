#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define STOP_COMMAND 1
#define DISCONNECT_COMMAND 2
#define CONNECT_COMMAND 3
#define LIST_COMMAND 4
#define INIT_COMMAND 5
#define MESSAGE_COMMAND 6
#define CLIENT_FREE 0
#define CLIENT_AVAILABLE 1
#define CLIENT_UNAVAILABLE 2
#define MAX_CLIENT_COUNT 100
#define MAX_MESSAGE_LENGTH 1000

typedef struct {
    long mtype;
    int clients[MAX_CLIENT_COUNT];
    char message[MAX_MESSAGE_LENGTH];
    int client_id;
    int other_client_id;
    char client_name[100];
} client_message_t;

typedef struct {
    long mtype;
    int client_id;
    int other_client_id;
    char client_name[100];
} message_buffer_t;

#endif
