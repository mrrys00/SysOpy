#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <signal.h>
#include "consts.h"

msg_t NAMETAKEN = _NAMETAKEN;
msg_t YOURTURN = _YOURTURN;
msg_t ENEMYTURN = _ENEMYTURN;
msg_t NOENEMY = _NOENEMY;
msg_t YOUWIN = _YOUWIN;
msg_t YOULOSE = _YOULOSE;
msg_t DRAW = _DRAW;
msg_t MOVE = _MOVE;
msg_t YOUREX = _YOUREX;
msg_t YOUREO = _YOUREO;
msg_t STOP = _STOP;

char names[NAMELEN][MAX_CLI];
int connfd1 = 0, connfd2 = 0, id1, id2;

int name_slot(char* nick) {
    for(int i = 0; i < MAX_CLI; i++) {
        if(strcmp(names[i], nick) == 0) return -1;
    }
    for(int i = 0; i < MAX_CLI; i++) {
        if(names[i][0] == '\0') return i;
    }
    return -2;
}

int rand01() {
    return rand() % 2 == 1;
}

bool eq3(int a, int b, int c) {
    return (a == b && b == c);
}

int not(int x) {
    if(x == 0) return 1;
    else return 0;
}

void print_board(int* board) {
    int p;
    printf("+---+\n");
    for(int y = 0; y < 3; y++) {
        printf("|");
        for(int x = 0; x < 3; x++) {
            p = board[y * 3 + x];
            if(p == 0) printf("O");
            else if(p == 1) printf("X");
            else printf(" ");
        }
        printf("|\n");
    }
    printf("+---+\n");
}

int win(int* b) {
    if(
        b[4] != -1 && (
        eq3(b[0], b[4], b[8]) ||
        eq3(b[2], b[4], b[6]) ||
        eq3(b[1], b[4], b[7]) ||
        eq3(b[3], b[4], b[5])
    )) return b[4];
    else if(
        b[0] != -1 && (
        eq3(b[0], b[1], b[2]) ||
        eq3(b[0], b[3], b[6])
    )) return b[0];
    else if(
        b[8] != -1 && (
        eq3(b[6], b[7], b[8]) ||
        eq3(b[2], b[5], b[8])
    )) return b[8];
    else return -1;
}

void fayrant(int signo) {
    if(connfd1 > 0) {
        send(connfd1, &STOP, MSGSIZE, 0);
        close(connfd1);
    }
    if(connfd2 > 0) {
        send(connfd2, &STOP, MSGSIZE, 0);
        close(connfd2);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    signal(SIGINT, fayrant);

    int listenfd = 0;
    struct sockaddr_in serv_addr; 

    char sendBuff[MESBUF], nickbuf[NAMELEN + 1];
    pid_t pid;

    for(int i = 0; i < MAX_CLI; i++) names[i][0] = '\0';

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(true) {
        id1 = -1; id2 = -1;
        while(id1 < 0) {
            connfd1 = accept(listenfd, (struct sockaddr*)NULL, NULL); 
            recv(connfd1, nickbuf, NAMELEN, 0);
            id1 = name_slot(nickbuf);
            if(id1 < 0) {
                send(connfd1, &NAMETAKEN, MSGSIZE, 0);
                close(connfd1);
            }
        }
        strcpy(names[id1], nickbuf);
        send(connfd1, &NOENEMY, MSGSIZE, 0);

        while(id2 < 0) {
            connfd2 = accept(listenfd, (struct sockaddr*)NULL, NULL); 
            recv(connfd2, nickbuf, NAMELEN, 0);
            id2 = name_slot(nickbuf);
            if(id2 < 0) {
                send(connfd2, &NAMETAKEN, MSGSIZE, 0);
                close(connfd2);
            }
        }
        strcpy(names[id2], nickbuf);
        
        printf("%s\n%s\n\n", names[0], names[1]);

        pid = fork();
        if(pid == 0) break;

        close(connfd1);
        close(connfd2);

        sleep(1);
    }

    if(pid == 0) {
        int board[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
        msg_t move = 0;
        int turn = rand01();
        int fds[] = {connfd1, connfd2};
        send(fds[turn], &YOUREO, MSGSIZE, 0);
        send(fds[not(turn)], &YOUREX, MSGSIZE, 0);
        while(win(board) < 0) {
            send(fds[turn], &YOURTURN, MSGSIZE, 0);
            send(fds[not(turn)], &ENEMYTURN, MSGSIZE, 0);
            recv(fds[turn], &move, MSGSIZE, 0);
            board[move & DATAMASK] = turn;
            send(fds[not(turn)], &move, MSGSIZE, 0);
            turn = not(turn);
        }
        send(fds[win(board)], &YOUWIN, MSGSIZE, 0);
        send(fds[not(win(board))], &YOULOSE, MSGSIZE, 0);
        names[id1][0] = '\0';
        names[id2][0] = '\0';
        close(connfd1);
        close(connfd2);
    }
}