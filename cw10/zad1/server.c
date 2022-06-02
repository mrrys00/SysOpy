#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include "config.h"

client_t clients[MAX_CLIENT];
game_t games[MAX_GAMES];
int waiting_client_fd = 0;
char *unix_socket_path;
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll;

void safe_exit(int signal_number)
{
    unlink(unix_socket_path);
    exit(EXIT_SUCCESS);
}

void add_client(int client_fd, int epoll)
{
    struct epoll_event client_event;
    client_event.events = EPOLLIN;
    client_event.data.fd = client_fd;

    if (epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
    {
        perror("epoll_ctl add");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].fd == 0)
        {
            clients[i].fd = client_fd;
            return;
        }
    }
}

void remove_game(int game_index)
{
    strcpy(games[game_index].board, "         ");
    games[game_index].has_started = 0;
    games[game_index].moving_side = 'X';
}

void game_to_string(int game_id, char *result)
{
    result[0] = '\0';
    strcat(result, "BOARD:\n");
    char *board = games[game_id].board;
    char row[21];
    for (int i = 0; i < 3; i++)
    {
        sprintf(row, "%c | %c | %c\n---------\n", board[i * 3], board[i * 3 + 1], board[i * 3 + 2]);
        strcat(result, row);
    }
}

void remove_client(int client_fd, int epoll, int remove_paired_client)
{
    printf("removing client %d\n", client_fd);
    if (epoll_ctl(epoll, EPOLL_CTL_DEL, client_fd, NULL) == -1)
    {
        perror("epoll_ctl remove");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].fd == client_fd)
        {
            if (clients[i].paired_fd != 0 && remove_paired_client)
            {
                remove_game(clients[i].game_id);
                remove_client(clients[i].paired_fd, epoll, 0);
            }
            clients[i].fd = 0;
            clients[i].name[0] = '\0';
            clients[i].paired_fd = 0;
            clients[i].game_id = -1;
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
}

int set_client_name(int client_fd, char *name)
{
    for (int i = 0; i < MAX_CLIENT; i++)
        if (strcmp(clients[i].name, name) == 0)
            return -1;

    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].fd == client_fd)
        {
            strcpy(clients[i].name, name);
            return 0;
        }
    }
    return -1;
}

void accept_connection(int server_socket, int epoll)
{
    int client_fd = accept4(server_socket, NULL, NULL, SOCK_NONBLOCK);

    if (client_fd == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    add_client(client_fd, epoll);
    printf("client with fd %d connected\n", client_fd);
}

int create_game(int client_fd, int other_client_fd)
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

void make_move(int game_id, int field_index)
{
    if (games[game_id].board[field_index - 1] != ' ')
        return;

    games[game_id].board[field_index - 1] = games[game_id].moving_side;
    if (games[game_id].moving_side == 'X')
        games[game_id].moving_side = 'O';
    else
        games[game_id].moving_side = 'X';
};

char check_winner(int game_id)
{
    char *board = games[game_id].board;
    for (int i = 0; i < 3; i++)
    {
        // check columns
        if (board[i] != ' ' && board[i] == board[i + 3] && board[i + 3] == board[i + 6])
            return board[i];
        // check rows
        if (board[3 * i] != ' ' && board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2])
            return board[3 * i];
    }
    // diagonals
    if (board[0] != ' ' && board[0] == board[4] && board[4] == board[8])
        return board[0];
    if (board[2] != ' ' && board[2] == board[4] && board[4] == board[6])
        return board[2];
    for (int i = 0; i < 9; i++)
        if (board[i] == ' ')
            return ' '; // no winner

    return 'D'; // draw
}

void read_from_socket(int client_fd, int epoll)
{
    char buffer[1001];
    int read_bytes_count = recv(client_fd, buffer, 1000, 0);

    if (read_bytes_count == 0)
    {
        remove_client(client_fd, epoll, 1);
        return;
    }

    buffer[read_bytes_count] = '\0';
    printf("received message from client %d: %s\n", client_fd, buffer);

    if (strncmp(buffer, "NAME: ", strlen("NAME: ")) == 0)
    {
        if (set_client_name(client_fd, buffer + strlen("NAME: ")) == -1)
        {
            char *message = "This name is already taken";
            if (send(client_fd, message, strlen(message), 0) == -1)
            {
                remove_client(client_fd, epoll, 0);
                return;
            }
            remove_client(client_fd, epoll, 0);
        }
        else if (waiting_client_fd != 0)
        {
            int game_id = create_game(waiting_client_fd, client_fd);
            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (clients[i].fd == waiting_client_fd)
                {
                    clients[i].paired_fd = client_fd;
                    clients[i].game_id = game_id;
                }
                if (clients[i].fd == client_fd)
                {
                    clients[i].paired_fd = waiting_client_fd;
                    clients[i].game_id = game_id;
                }
            }
            char message[1000];
            game_to_string(game_id, message);
            int message_length = strlen(message);
            if (games[game_id].player_x == client_fd)
                strcat(message, "\nPlaying with X\n");
            else
                strcat(message, "\nPlaying with O\n");

            if (send(client_fd, message, strlen(message), 0) == -1)
                remove_client(client_fd, epoll, 1);
                return;
                
            message[message_length] = '\0';
            if (games[game_id].player_x == waiting_client_fd)
                strcat(message, "\nPlaying with X\n");
            else
                strcat(message, "\nPlaying with O\n");

            if (send(waiting_client_fd, message, strlen(message), 0) == -1)
            {
                remove_client(waiting_client_fd, epoll, 1);
                return;
            }
            waiting_client_fd = 0;
        }
        else
            waiting_client_fd = client_fd;
    }

    if (strcmp(buffer, "PONG") == 0)
    {
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clients[i].fd == client_fd)
            {
                clients[i].has_ping_responded = 1;
                break;
            }
        }
    }

    if (strncmp(buffer, "MOVE: ", strlen("MOVE: ")) == 0)
    {
        int field_index;
        sscanf(buffer, "MOVE: %d", &field_index);
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clients[i].fd == client_fd)
            {
                int game_id = clients[i].game_id;
                int opponent_fd = clients[i].paired_fd;
                if (games[game_id].moving_side == 'X' && games[game_id].player_x == client_fd)
                    make_move(game_id, field_index);
                if (games[game_id].moving_side == 'O' && games[game_id].player_o == client_fd)
                    make_move(game_id, field_index);
                char winner = check_winner(game_id);
                char message[1000];
                game_to_string(game_id, message);
                char client_side = games[game_id].player_x == client_fd ? 'X' : 'O';
                if (winner == 'D')
                    strcat(message, "\nDRAW!");
                int message_length = strlen(message);
                if (winner == 'X' || winner == 'O')
                {
                    if (client_side == winner)
                        strcat(message, "\nYOU WON!");
                    else
                        strcat(message, "\nYOU LOST!");
                }

                if (send(client_fd, message, strlen(message), 0) == -1)
                {
                    remove_client(client_fd, epoll, 1);
                    return;
                }
                message[message_length] = '\0';
                if (winner == 'X' || winner == 'O')
                {
                    if (client_side == winner)
                        strcat(message, "\nYOU LOST!");
                    else
                        strcat(message, "\nYOU WON!");
                }
                if (send(opponent_fd, message, strlen(message), 0) == -1)
                {
                    remove_client(opponent_fd, epoll, 1);
                    return;
                }

                if (winner != ' ')
                    remove_client(client_fd, epoll, 1);
                break;
            }
        }
    }
}

void handle_event(int server_socket, int unix_socket, struct epoll_event *event, int epoll)
{
    if (event->data.fd == server_socket)
    {
        accept_connection(server_socket, epoll);
        return;
    }

    if (event->data.fd == unix_socket)
    {
        accept_connection(unix_socket, epoll);
        return;
    }

    int client_fd = event->data.fd;
    if (event->events & EPOLLIN)
        read_from_socket(client_fd, epoll);

    if (event->events & EPOLLHUP)
        remove_client(client_fd, epoll, 1);
}

void start_event_loop(int server_socket, int unix_server_socket)
{
    epoll = epoll_create1(0);
    struct epoll_event server_event;
    server_event.events = EPOLLIN;
    server_event.data.fd = server_socket;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, server_socket, &server_event) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    server_event.data.fd = unix_server_socket;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, unix_server_socket, &server_event) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[EPOLL_EVENTS_SIZE];
    while (1)
    {
        pthread_mutex_unlock(&socket_mutex);
        int events_count = epoll_wait(epoll, events, EPOLL_EVENTS_SIZE, -1);
        if (events_count == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        
        pthread_mutex_lock(&socket_mutex);
        for (int i = 0; i < events_count; i++)
            handle_event(server_socket, unix_server_socket, &events[i], epoll);
    }
}

void *pinging_thread_routine(void *arg)
{
    while (1)
    {
        sleep(15);
        pthread_mutex_lock(&socket_mutex);
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clients[i].fd != 0)
            {
                if (!clients[i].has_ping_responded)
                {
                    if (clients[i].paired_fd != 0)
                        remove_client(clients[i].fd, epoll, 1);
                    else
                        remove_client(clients[i].fd, epoll, 0);
                }
                else
                {
                    char *message = "PING";
                    send(clients[i].fd, message, strlen(message), 0);
                }
            }
        }
        pthread_mutex_unlock(&socket_mutex);
    }
    return NULL;
}

int main(int argc, char *args[])
{
    srand(time(NULL));
    if (argc < 3)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }
    int port = atoi(args[1]);
    unix_socket_path = args[2];

    client_t empty_client;
    empty_client.fd = 0;
    empty_client.paired_fd = 0;
    empty_client.game_id = -1;
    empty_client.name[0] = '\0';
    empty_client.has_ping_responded = 1;
    for (int i = 0; i < MAX_CLIENT; i++)
        clients[i] = empty_client;

    game_t empty_game;
    strcpy(empty_game.board, "         ");
    empty_game.has_started = 0;
    empty_game.moving_side = 'X';
    for (int i = 0; i < MAX_GAMES; i++)
        games[i] = empty_game;

    int server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_socket == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    int opt = 1;

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        return EXIT_FAILURE;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, CONNECTION_QUEUE_SIZE) == -1)
    {
        perror("listen");
        return EXIT_FAILURE;
    }

    int unix_server_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (unix_server_socket == -1)
    {
        perror("unix socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_un server_unix_address;
    server_unix_address.sun_family = AF_UNIX;
    strcpy(server_unix_address.sun_path, unix_socket_path);

    if (bind(unix_server_socket, (struct sockaddr *)&server_unix_address, sizeof(server_unix_address)) == -1)
    {
        perror("unix bind");
        return EXIT_FAILURE;
    }

    if (listen(unix_server_socket, CONNECTION_QUEUE_SIZE) == -1)
    {
        perror("unix listen");
        return EXIT_FAILURE;
    }

    signal(SIGINT, safe_exit); // remove unix socket on CRTL + C

    pthread_t pinging_thread;
    pthread_create(&pinging_thread, NULL, pinging_thread_routine, NULL);

    start_event_loop(server_socket, unix_server_socket);
    return EXIT_SUCCESS;
}
