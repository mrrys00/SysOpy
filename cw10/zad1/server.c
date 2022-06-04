#define _GNU_SOURCE
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
char *unix_socket_path;

void safe_exit(int signal_number)
{
    unlink(unix_socket_path);
    exit(EXIT_SUCCESS);
}

void connect_client(int client_fd, int *epoll, client_t *clients)
{
    struct epoll_event client_event;
    client_event.events = EPOLLIN;
    client_event.data.fd = client_fd;

    if (epoll_ctl(*epoll, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
    {
        perror("error epoll_ctl add");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLI_NUM; i++)
    {
        if (clients[i].fd == 0)
        {
            clients[i].fd = client_fd;
            return;
        }
    }
    return;
}

void clean_game(int game_index, game_t *games)
{
    strcpy(games[game_index].board, "         ");
    games[game_index].has_started = 0;
    games[game_index].moving_side = 'X';
    return;
}

void print_board(int game_id, char *res, game_t *games)
{
    res[0] = '\0';
    strcat(res, "BOARD:\n");
    char *board = games[game_id].board;
    char row[21];
    for (int i = 0; i < 3; i++)
    {
        sprintf(row, "%c | %c | %c\n---------\n", board[i * 3], board[i * 3 + 1], board[i * 3 + 2]);
        strcat(res, row);
    }
    return;
}

void clean_client(int client_fd, int remove_paired_client, args_t *args)
{
    printf("clean client %d\n", client_fd);
    if (epoll_ctl(*args->epoll, EPOLL_CTL_DEL, client_fd, NULL) == -1)
    {
        perror("epoll_ctl remove");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLI_NUM; i++)
    {
        if (args->clients[i].fd == client_fd)
        {
            if (args->clients[i].paired_fd != 0 && remove_paired_client)
            {
                clean_game(args->clients[i].game_id, args->games);
                clean_client(args->clients[i].paired_fd, 0, args);
            }
            args->clients[i].fd = 0;
            args->clients[i].name[0] = '\0';
            args->clients[i].paired_fd = 0;
            args->clients[i].game_id = -1;
            break;
        }
    }

    if (shutdown(client_fd, SHUT_RDWR) == -1)
    {
        perror("shutdown");
        exit(EXIT_FAILURE);
    };

    if (close(client_fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    };
    return;
}

int set_client_name(int client_fd, char *name, client_t *clients)
{
    for (int i = 0; i < MAX_CLI_NUM; i++)
        if (strcmp(clients[i].name, name) == 0)
            return -1;

    for (int i = 0; i < MAX_CLI_NUM; i++)
    {
        if (clients[i].fd == client_fd)
        {
            strcpy(clients[i].name, name);
            return 0;
        }
    }
    return -1;
}

void accept_connection(int serv_sock, int *epoll, client_t *clients)
{
    int client_fd = accept4(serv_sock, NULL, NULL, SOCK_NONBLOCK);

    if (client_fd == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    connect_client(client_fd, epoll, clients);
    printf("client with fd %d connected\n", client_fd);
    return;
}

int create_game(int client_fd, int other_client_fd, game_t *games)
{
    for (int i = 0; i < MAX_GAMES; i++)
    {
        if (!games[i].has_started)
        {
            games[i].has_started = 1;
            if (rand() % 2)
            {
                games[i].player_x = client_fd;
                games[i].player_o = other_client_fd;
            }
            else
            {
                games[i].player_x = other_client_fd;
                games[i].player_o = client_fd;
            };
            return i;
        }
    }
    return -1;
}

void take_action(int game_id, int field_index, game_t *games)
{
    if (games[game_id].board[field_index - 1] != ' ')
        return;

    games[game_id].board[field_index - 1] = games[game_id].moving_side;
    if (games[game_id].moving_side == 'X')
        games[game_id].moving_side = 'O';
    else
        games[game_id].moving_side = 'X';

    return;
};

char game_status(int game_id, game_t *games)
{
    char *board = games[game_id].board;

    // diagonals
    if (board[0] != ' ' && board[0] == board[4] && board[4] == board[8])
        return board[0];

    if (board[2] != ' ' && board[2] == board[4] && board[4] == board[6])
        return board[2];

    for (int i = 0; i < 3; i++)
    {
        // rows
        if (board[3 * i] != ' ' && board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2])
            return board[3 * i];

        // columns
        if (board[i] != ' ' && board[i] == board[i + 3] && board[i + 3] == board[i + 6])
            return board[i];
    }

    // game in progress
    for (int i = 0; i < 9; i++)
        if (board[i] == ' ')
            return ' ';

    // draw
    return 'D';
}

void read_socket(int client_fd, args_t *args)
{
    char buf[1001];
    int read_bytes_count = recv(client_fd, buf, 1000, 0);

    if (read_bytes_count == 0)
    {
        clean_client(client_fd, 1, args);
        return;
    }

    buf[read_bytes_count] = '\0';
    printf("client %d: %s\n", client_fd, buf);

    if (strncmp(buf, "CLI_NAME: ", strlen("CLI_NAME: ")) == 0)
    {
        if (set_client_name(client_fd, buf + strlen("CLI_NAME: "), args->clients) == -1)
        {
            char *message = "Taken name - choose different";
            if (send(client_fd, message, strlen(message), 0) == -1)
            {
                clean_client(client_fd, 0, args);
                return;
            }
            clean_client(client_fd, 0, args);
        }
        else if (*args->waiting_client_fd != 0)
        {
            int game_id = create_game(*args->waiting_client_fd, client_fd, args->games);
            for (int i = 0; i < MAX_CLI_NUM; i++)
            {
                if (args->clients[i].fd == *args->waiting_client_fd)
                {
                    args->clients[i].paired_fd = client_fd;
                    args->clients[i].game_id = game_id;
                }
                if (args->clients[i].fd == client_fd)
                {
                    args->clients[i].paired_fd = *args->waiting_client_fd;
                    args->clients[i].game_id = game_id;
                }
            }
            char message[1000];
            print_board(game_id, message, args->games);
            int message_length = strlen(message);
            if (args->games[game_id].player_x == client_fd)
                strcat(message, "\nPlaying with X\n");
            else
                strcat(message, "\nPlaying with O\n");

            if (send(client_fd, message, strlen(message), 0) == -1)
            {
                clean_client(client_fd, 1, args);
                return;
            }
                
            message[message_length] = '\0';
            if (args->games[game_id].player_x == *args->waiting_client_fd)
                strcat(message, "Your sign is: X\n");
            else
                strcat(message, "Your sign is: O\n");

            if (send(*args->waiting_client_fd, message, strlen(message), 0) == -1)
            {
                clean_client(*args->waiting_client_fd, 1, args);
                return;
            }
            *args->waiting_client_fd = 0;
        }
        else
            *args->waiting_client_fd = client_fd;
    }

    if (strcmp(buf, "resp_") == 0)
    {
        for (int i = 0; i < MAX_CLI_NUM; i++)
        {
            if (args->clients[i].fd == client_fd)
            {
                args->clients[i].has_ping_responded = 1;
                break;
            }
        }
    }

    if (strncmp(buf, "turn: ", strlen("turn: ")) == 0)
    {
        int field_index;
        sscanf(buf, "turn: %d", &field_index);
        for (int i = 0; i < MAX_CLI_NUM; i++)
        {
            if (args->clients[i].fd == client_fd)
            {
                int game_id = args->clients[i].game_id, opponent_fd = args->clients[i].paired_fd;
                if (args->games[game_id].moving_side == 'X' && args->games[game_id].player_x == client_fd)
                    take_action(game_id, field_index, args->games);

                if (args->games[game_id].moving_side == 'O' && args->games[game_id].player_o == client_fd)
                    take_action(game_id, field_index, args->games);

                char winner = game_status(game_id, args->games);
                char message[1000];
                print_board(game_id, message, args->games);
                char client_side = args->games[game_id].player_x == client_fd ? 'X' : 'O';

                if (winner == 'D')
                    strcat(message, "\nDRAW!");

                int message_length = strlen(message);
                if (winner == 'X' || winner == 'O')
                {
                    if (client_side == winner)
                        strcat(message, "\nWON!");
                    else
                        strcat(message, "\nLOST!");
                }

                if (send(client_fd, message, strlen(message), 0) == -1)
                {
                    clean_client(client_fd, 1, args);
                    return;
                }

                message[message_length] = '\0';
                if (winner == 'X' || winner == 'O')
                {
                    if (client_side == winner)
                        strcat(message, "\nLOST!");
                    else
                        strcat(message, "\nWON!");
                }

                if (send(opponent_fd, message, strlen(message), 0) == -1)
                {
                    clean_client(opponent_fd, 1, args);
                    return;
                }

                if (winner != ' ')
                    clean_client(client_fd, 1, args);
                break;
            }
        }
    }

    return;
}

void event_handling(int serv_sock, int unix_socket, struct epoll_event *event, args_t *args)
{
    if (event->data.fd == serv_sock)
    {
        accept_connection(serv_sock, args->epoll, args->clients);
        return;
    }

    if (event->data.fd == unix_socket)
    {
        accept_connection(unix_socket, args->epoll, args->clients);
        return;
    }

    int client_fd = event->data.fd;
    if (event->events & EPOLLIN)
        read_socket(client_fd, args);

    if (event->events & EPOLLHUP)
        clean_client(client_fd, 1, args);
    
    return;
}

void event_init(int serv_sock, int unix_sock, args_t *args)
{
    *args->epoll = epoll_create1(0);
    struct epoll_event server_event;
    server_event.events = EPOLLIN;
    server_event.data.fd = serv_sock;
    if (epoll_ctl(*args->epoll, EPOLL_CTL_ADD, serv_sock, &server_event) == -1)
    {
        perror("epoll_ctl error ");
        exit(EXIT_FAILURE);
    }

    server_event.data.fd = unix_sock;
    if (epoll_ctl(*args->epoll, EPOLL_CTL_ADD, unix_sock, &server_event) == -1)
    {
        perror("epoll_ctl error");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[EPOLL_EVENTS_SIZE];
    while (1)
    {
        pthread_mutex_unlock(&socket_mutex);
        int events_count = epoll_wait(*args->epoll, events, EPOLL_EVENTS_SIZE, -1);
        if (events_count == -1)
        {
            perror("epoll_wait error ");
            exit(EXIT_FAILURE);
        }
        
        pthread_mutex_lock(&socket_mutex);
        for (int i = 0; i < events_count; i++)
            event_handling(serv_sock, unix_sock, &events[i], args);
    }
    return;
}

void *play_t_routine(void *arg)
{
    args_t *args = (args_t *)arg;
    while (1)
    {
        sleep(5);
        pthread_mutex_lock(&socket_mutex);
        for (int i = 0; i < MAX_CLI_NUM; i++)
        {
            if (args->clients[i].fd != 0)
            {
                if (!args->clients[i].has_ping_responded)
                {
                    if (args->clients[i].paired_fd != 0)
                        clean_client(args->clients[i].fd, 1, args);
                    else
                        clean_client(args->clients[i].fd, 0, args);
                }
                else
                {
                    char *message = "requ_";
                    send(args->clients[i].fd, message, strlen(message), 0);
                }
            }
        }
        pthread_mutex_unlock(&socket_mutex);
    }
    return NULL;
}

int full_init(int *port, int *serv_sock, int *unix_sock, args_t *args)
{
    *serv_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (*serv_sock == -1)
    {
        perror("socket error");
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(*serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt error");
        return EXIT_FAILURE;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(*port);

    if (bind(*serv_sock, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("socket binding error: ");
        return EXIT_FAILURE;
    }

    if (listen(*serv_sock, CONNECTION_QUEUE_SIZE) == -1)
    {
        perror("socket listening error: ");
        return EXIT_FAILURE;
    }

    *unix_sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (*unix_sock == -1)
    {
        perror("unix server socket error ");
        return EXIT_FAILURE;
    }

    struct sockaddr_un server_unix_address;
    server_unix_address.sun_family = AF_UNIX;
    strcpy(server_unix_address.sun_path, unix_socket_path);

    if (bind(*unix_sock, (struct sockaddr *)&server_unix_address, sizeof(server_unix_address)) == -1)
    {
        perror("unix server binding error:");
        return EXIT_FAILURE;
    }

    if (listen(*unix_sock, CONNECTION_QUEUE_SIZE) == -1)
    {
        perror("unix server listening error: ");
        return EXIT_FAILURE;
    }

    pthread_t pinging_thread;
    pthread_create(&pinging_thread, NULL, play_t_routine, (void *)&args);

    return 0;
}

void init_client(client_t clients[])
{
    client_t _client;
    _client.fd = 0;
    _client.paired_fd = 0;
    _client.game_id = -1;
    _client.name[0] = '\0';
    _client.has_ping_responded = 1;
    for (int i = 0; i < MAX_CLI_NUM; i++)
        clients[i] = _client;
    return;
}

void init_game(game_t games[])
{
    game_t _game;
    strcpy(_game.board, "         ");
    _game.has_started = 0;
    _game.moving_side = 'X';
    for (int i = 0; i < MAX_GAMES; i++)
        games[i] = _game;
    return;
}

int main(int argc, char *args[])
{
    if (argc < 3)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }
    int port = atoi(args[1]);
    unix_socket_path = args[2];

    srand(time(NULL));
    int waiting_client_fd = 0, epoll;
    args_t thread_args;
    thread_args.waiting_client_fd = &waiting_client_fd;
    thread_args.epoll = &epoll;
    init_client(thread_args.clients);
    init_game(thread_args.games);
    signal(SIGINT, safe_exit);

    int serv_sock, unix_sock;
    if (full_init(&port, &serv_sock, &unix_sock, &thread_args) == EXIT_FAILURE)
        return EXIT_FAILURE;

    event_init(serv_sock, unix_sock, &thread_args);
    return EXIT_SUCCESS;
}
