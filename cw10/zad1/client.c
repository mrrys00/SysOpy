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

void event_handling(int cli_socket, struct epoll_event *event, int epoll, int *is_cli_turn, int *change_play_turn, char *whos_play)
{
    char buf[1025];
    if (event->data.fd == STDIN_FILENO)
    {
        scanf("%s", buf);
        if (!*is_cli_turn)
        {
            printf("Your turn\n");
            return;
        }
        char mes[100] = "turn: ";
        strcat(mes, buf);
        send(cli_socket, mes, strlen(mes), 0);
        *is_cli_turn = !*is_cli_turn;
        *change_play_turn = 0;
    }
    if (event->data.fd == cli_socket)
    {
        int read_bytes_count = recv(cli_socket, buf, 1000, 0);
        if (read_bytes_count == 0)
            exit(EXIT_SUCCESS);

        buf[read_bytes_count] = '\0';
        if (strncmp(buf, "requ_", 4) == 0)
        {
            char *mes = "resp_";
            send(cli_socket, mes, strlen(mes), 0);
        }
        else if (strncmp(buf, "STATE:\n", strlen("STATE:\n")) == 0)
        {
            if (*change_play_turn)
                *is_cli_turn = !*is_cli_turn;
            else
                *change_play_turn = 1;

            printf("\n%s\n", buf);
            if (*whos_play == ' ')
            {
                if (strchr(buf, 'X') == NULL)
                {
                    *whos_play = 'O';
                    *is_cli_turn = 0;
                }
                else
                {
                    *whos_play = 'X';
                    *is_cli_turn = 1;
                }
            }
        }
        else
            printf("\n%s\n", buf);
    }
}

int full_init(char *name, char *connection_mode, char *address, int *cli_socket, int *epoll)
{
    if (strcmp(connection_mode, "network") == 0)
    {
        char *ip_address = strtok(address, ":");
        int port = atoi(strtok(NULL, ":"));
        *cli_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (*cli_socket == -1)
        {
            perror("socket error");
            return EXIT_FAILURE;
        }

        int opt = 1;
        if (setsockopt(*cli_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        {
            perror("set sock opt error");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0)
        {
            perror("inet_pton error");
            return EXIT_FAILURE;
        }

        if (connect(*cli_socket, &serv_addr, sizeof(serv_addr)) == -1)
        {
            perror("connection error");
            return EXIT_FAILURE;
        }
    }
    else if (strcmp(connection_mode, "socket") == 0)
    {
        *cli_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (*cli_socket == -1)
        {
            perror("unix socket");
            return EXIT_FAILURE;
        }
        struct sockaddr_un server_unix_address;
        server_unix_address.sun_family = AF_UNIX;
        strcpy(server_unix_address.sun_path, address);

        if (connect(*cli_socket, &server_unix_address, sizeof(server_unix_address)) == -1)
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
    struct epoll_event cli_socket_event;
    cli_socket_event.events = EPOLLIN;
    cli_socket_event.data.fd = *cli_socket;
    if (epoll_ctl(*epoll, EPOLL_CTL_ADD, *cli_socket, &cli_socket_event) == -1)
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

    char mes[1000] = "CLI_NAME: ";
    strcat(mes, name);
    send(*cli_socket, mes, strlen(mes), 0);

    return 0;
}

int main(int argc, char *args[])
{
    if (argc < 4)
    {
        perror("not enough arguments!");
        exit(EXIT_FAILURE);
    }

    char *name = args[1], *connection_mode = args[2], *address = args[3], whos_play = ' ';
    int is_cli_turn = 1, change_play_turn = 0, cli_socket, epoll;

    signal(SIGINT, safe_exit);

    if (full_init(name, connection_mode, address, &cli_socket, &epoll) == EXIT_FAILURE)
        return EXIT_FAILURE;

    struct epoll_event events[EPOLL_EV_SIZE];
    while (1)
    {
        int ev_cnt = epoll_wait(epoll, events, EPOLL_EV_SIZE, -1);
        if (ev_cnt == -1)
        {
            perror("epoll_wait error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < ev_cnt; i++)
            event_handling(cli_socket, &events[i], epoll, &is_cli_turn, &change_play_turn, &whos_play);
    }

    return EXIT_SUCCESS;
}
