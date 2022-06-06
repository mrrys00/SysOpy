
#ifndef CONFIG_H
#define CONFIG_H

#define Q_CONN_SIZE 1024
#define EPOLL_EV_SIZE 20
#define MAX_CLI_NUM 2048
#define MAX_GAMES MAX_CLI_NUM / 2

typedef struct
{
    char    name[1024];
    int     fd;
    int     paired_fd;
    int     game_id;
    int     if_rec_resp;
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
    int         *waiting_cli_fd;
    int         *epoll;
} args_t;


#endif
