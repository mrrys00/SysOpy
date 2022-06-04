#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "config.h"

void safe_exit(int signal_number)
{
    exit(EXIT_SUCCESS);
}

void event_handling(int client_socket, struct epoll_event *event, int epoll, int *is_client_turn, int *should_change_player_turn, char *playing_side)
{
    char buffer[1025];
    if (event->data.fd == STDIN_FILENO)
    {
        scanf("%s", buffer);
        if (!*is_client_turn)
        {
            printf("Your move\n");
            return;
        }
        char message[100] = "turn: ";
        strcat(message, buffer);
        send(client_socket, message, strlen(message), 0);
        *is_client_turn = !*is_client_turn;
        *should_change_player_turn = 0;
    }
    if (event->data.fd == client_socket)
    {
        int read_bytes_count = recv(client_socket, buffer, 1000, 0);
        if (read_bytes_count == 0)
            exit(EXIT_SUCCESS);

        buffer[read_bytes_count] = '\0';
        if (strncmp(buffer, "requ_", 4) == 0)
        {
            char *message = "resp_";
            send(client_socket, message, strlen(message), 0);
        }
        else if (strncmp(buffer, "BOARD:\n", strlen("BOARD:\n")) == 0)
        {
            if (*should_change_player_turn)
                *is_client_turn = !*is_client_turn;
            else
                *should_change_player_turn = 1;

            printf("\n%s\n", buffer);
            if (*playing_side == ' ')
            {
                if (strchr(buffer, 'X') == NULL)
                {
                    *playing_side = 'O';
                    *is_client_turn = 0;
                }
                else
                {
                    *playing_side = 'X';
                    *is_client_turn = 1;
                }
            }
        }
        else
            printf("\n%s\n", buffer);
    }
}

int full_init(char *name, char *connection_mode, char *address, int *client_socket, int *epoll)
{
    if (strcmp(connection_mode, "network") == 0)
    {
        char *ip_address = strtok(address, ":");
        int port = atoi(strtok(NULL, ":"));
        *client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (*client_socket == -1)
        {
            perror("socket error");
            return EXIT_FAILURE;
        }

        int opt = 1;
        if (setsockopt(*client_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            perror("setsockopt error");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);

        if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0)
        {
            perror("inet_pton error");
            return EXIT_FAILURE;
        }

        if (connect(*client_socket, &server_address, sizeof(server_address)) == -1)
        {
            perror("connection error");
            return EXIT_FAILURE;
        }
    }
    else if (strcmp(connection_mode, "socket") == 0)
    {
        *client_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (*client_socket == -1)
        {
            perror("unix socket");
            return EXIT_FAILURE;
        }
        struct sockaddr_un server_unix_address;
        server_unix_address.sun_family = AF_UNIX;
        strcpy(server_unix_address.sun_path, address);

        if (connect(*client_socket, &server_unix_address, sizeof(server_unix_address)) == -1)
        {
            perror("connection error");
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("Not known connect mode\n");
        exit(EXIT_FAILURE);
    }

    *epoll = epoll_create1(0);
    struct epoll_event client_socket_event;
    client_socket_event.events = EPOLLIN;
    client_socket_event.data.fd = *client_socket;
    if (epoll_ctl(*epoll, EPOLL_CTL_ADD, *client_socket, &client_socket_event) == -1)
    {
        perror("epoll_ctl client socket");
        exit(EXIT_FAILURE);
    }

    struct epoll_event io_event;
    io_event.events = EPOLLIN;
    io_event.data.fd = STDIN_FILENO;
    if (epoll_ctl(*epoll, EPOLL_CTL_ADD, STDIN_FILENO, &io_event) == -1)
    {
        perror("epoll_ctl io error");
        exit(EXIT_FAILURE);
    }

    char message[1000] = "CLI_NAME: ";
    strcat(message, name);
    send(*client_socket, message, strlen(message), 0);

    return 0;
}

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    char *name = args[1], *connection_mode = args[2], *address = args[3];
    int is_client_turn = 1, should_change_player_turn = 0, client_socket, epoll;
    char playing_side = ' ';

    signal(SIGINT, safe_exit);

    if (full_init(name, connection_mode, address, &client_socket, &epoll) == EXIT_FAILURE)
        return EXIT_FAILURE;

    struct epoll_event events[EPOLL_EVENTS_SIZE];
    while (1)
    {
        int events_count = epoll_wait(epoll, events, EPOLL_EVENTS_SIZE, -1);
        if (events_count == -1)
        {
            perror("epoll_wait error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < events_count; i++)
            event_handling(client_socket, &events[i], epoll, &is_client_turn, &should_change_player_turn, &playing_side);
    }

    return EXIT_SUCCESS;
}
