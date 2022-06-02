#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/un.h>
#include <signal.h>
#include <arpa/inet.h>
#include "config.h"

int is_client_turn = 1;
int should_change_player_turn = 0;
char playing_side = ' ';

void safe_exit(int signal_number)
{
    exit(EXIT_SUCCESS);
}

void handle_event(int client_socket, struct epoll_event *event, int epoll)
{
    char buffer[1025];
    if (event->data.fd == STDIN_FILENO)
    {
        scanf("%s", buffer);
        if (!is_client_turn)
        {
            printf("Wait for your turn\n");
            return;
        }
        char message[100] = "MOVE: ";
        strcat(message, buffer);
        send(client_socket, message, strlen(message), 0);
        is_client_turn = !is_client_turn;
        should_change_player_turn = 0;
    }
    if (event->data.fd == client_socket)
    {
        int read_bytes_count = recv(client_socket, buffer, 1000, 0);
        if (read_bytes_count == 0)
            exit(EXIT_SUCCESS);
        buffer[read_bytes_count] = '\0';
        if (strncmp(buffer, "PING", 4) == 0)
        {
            char *message = "PONG";
            send(client_socket, message, strlen(message), 0);
        }
        else if (strncmp(buffer, "BOARD:\n", strlen("BOARD:\n")) == 0)
        {
            if (should_change_player_turn)
                is_client_turn = !is_client_turn;
            else
                should_change_player_turn = 1;
            printf("\n%s\n", buffer);
            if (playing_side == ' ')
            {
                if (strchr(buffer, 'X') == NULL)
                {
                    playing_side = 'O';
                    is_client_turn = 0;
                }
                else
                {
                    playing_side = 'X';
                    is_client_turn = 1;
                }
            }
        }
        else
            printf("\n%s\n", buffer);
    }
}

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }
    char *name = args[1];
    char *connection_mode = args[2];
    char *address = args[3];

    int client_socket;

    signal(SIGINT, safe_exit); // remove unix socket on CRTL + C

    if (strcmp(connection_mode, "network") == 0)
    {
        char *ip_address = strtok(address, ":");
        int port = atoi(strtok(NULL, ":"));
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1)
        {
            perror("socket");
            return EXIT_FAILURE;
        }

        int opt = 1;
        if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);

        if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0)
        {
            perror("inet_pton");
            return EXIT_FAILURE;
        }

        if (connect(client_socket, &server_address, sizeof(server_address)) == -1)
        {
            perror("connect");
            return EXIT_FAILURE;
        }
    }
    else if (strcmp(connection_mode, "socket") == 0)
    {
        client_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (client_socket == -1)
        {
            perror("unix socket");
            return EXIT_FAILURE;
        }
        struct sockaddr_un server_unix_address;
        server_unix_address.sun_family = AF_UNIX;
        strcpy(server_unix_address.sun_path, address);

        if (connect(client_socket, &server_unix_address, sizeof(server_unix_address)) == -1)
        {
            perror("connect");
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("Invalid connection mode\n");
        exit(EXIT_FAILURE);
    }

    int epoll = epoll_create1(0);
    struct epoll_event client_socket_event;
    client_socket_event.events = EPOLLIN;
    client_socket_event.data.fd = client_socket;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, client_socket, &client_socket_event) == -1)
    {
        perror("epoll_ctl client socket");
        exit(EXIT_FAILURE);
    }

    struct epoll_event io_event;
    io_event.events = EPOLLIN;
    io_event.data.fd = STDIN_FILENO;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, STDIN_FILENO, &io_event) == -1)
    {
        perror("epoll_ctl io");
        exit(EXIT_FAILURE);
    }

    char message[1000] = "NAME: ";
    strcat(message, name);
    send(client_socket, message, strlen(message), 0);

    struct epoll_event events[EPOLL_EVENTS_SIZE];
    while (1)
    {
        int events_count = epoll_wait(epoll, events, EPOLL_EVENTS_SIZE, -1);
        if (events_count == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < events_count; i++)
            handle_event(client_socket, &events[i], epoll);
    }
    
    return EXIT_SUCCESS;
}
