
#ifndef CONFIG_H
#define CONFIG_H

#define CONNECTION_QUEUE_SIZE 1024
#define EPOLL_EVENTS_SIZE 20
#define MAX_CLIENT 1024
#define MAX_GAMES MAX_CLIENT / 2
#define EPOLL_EVENTS_SIZE 20

typedef struct
{
    char name[1024];
    int fd;
    int paired_fd;
    int game_id;
    int has_ping_responded;
} client_t;

typedef struct
{
    int player_x;
    int player_o;
    char board[10];
    char moving_side;
    int has_started;
} game_t;

typedef struct
{
    client_t clients[MAX_CLIENT];
    game_t games[MAX_GAMES];
    int *waiting_client_fd;      // change to ptr
    int *epoll;      // change to ptr
} args_t;


#endif
