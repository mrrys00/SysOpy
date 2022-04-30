#ifndef CONFIG_H
#define CONFIG_H

#define T_INIT 01L
#define T_STOP 02L
#define T_LIST 03L
#define T_TOALL 04L
#define T_TOALL 04L
#define T_TOONE 05L
#define T_ERROR 06L

#define ERR_NOTFOUND -9
// #define DISCONNECT_COMMAND 2
// #define CONNECT_COMMAND 3

// #define MESSAGE_COMMAND 6
// #define CLIENT_FREE 0
// #define CLIENT_AVAILABLE 1
// #define CLIENT_UNAVAILABLE 2

#define MAXMESLEN 500
#define MAXCLINUM 128

#define LOGNAME "logs.log"

typedef struct {
    long mtype;
    char message[MAX_MESSAGE_LENGTH];
    int client_id;
    int other_client_id;
    char client_name[100];
} message_t;

#endif
