
#ifndef CONFIG_H
#define CONFIG_H

#define CONNECTION_QUEUE_SIZE 1024
#define EPOLL_EVENTS_SIZE 20
#define MAX_CLI_NUM 2048
#define MAX_GAMES MAX_CLI_NUM / 2

typedef struct
{
    char    name[1024];
    int     fd;
    int     paired_fd;
    int     game_id;
    int     has_ping_responded;
} client_t;

typedef struct
{
    int     x_player;
    int     o_player;
    char    board[10];
    char    moving_side;
    int     was_started;
} game_t;

typedef struct
{
    client_t    clients[MAX_CLI_NUM];
    game_t      games[MAX_GAMES];
    int         *waiting_client_fd;
    int         *epoll;
} args_t;


#endif
