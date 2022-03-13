#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "consts.h"

char symbols[2];

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

void print_board(int* board) {
    int p;
    printf("+---+\n");
    for(int y = 0; y < 3; y++) {
        printf("|");
        for(int x = 0; x < 3; x++) {
            p = board[y * 3 + x];
            if(p == 0 || p == 1) printf("%c", symbols[p]);
            else printf(" ");
        }
        printf("|\n");
    }
    printf("+---+\n");
}

bool verify_move(msg_t move, int* board) {
    if(move < 1 || move > 9) {
        printf("Has to be a number 1-9\n");
        return false;
    }
    else if(board[move - 1] != -1) {
        printf("This field is occupied\n");
        return false;
    }
    else return true;
}

int main(int argc, char *argv[])
{

    int sockfd = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 4)
    {
        printf("\n Usage: %s <nickname> <connection type> <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, argv[3], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    send(sockfd, argv[1], strlen(argv[1]), 0);

    printf("+---+\n|123|\n|456|\n|789|\n+---+\n");

    msg_t din, msg, move;
    int board[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

    bool quit = false;
    while(!quit) {
        if( recv(sockfd, &din, MSGSIZE, 0) > 0) {
            msg = din & MSGMASK;
            switch(msg) {
                case _NAMETAKEN:\
                    printf("This name is taken - choose a different one\n");
                    break;
                case _YOUREX:
                    printf("Your symbol is X\n");
                    symbols[0] = 'X'; symbols[1] = 'O';
                    break;
                case _YOUREO:
                    printf("Your symbol is O\n");
                    symbols[0] = 'O'; symbols[1] = 'X';
                    break;
                case _YOURTURN:
                    printf("Your turn!\n");
                    scanf("%d", &move);
                    while(!verify_move(move, board)) scanf("%d", &move);
                    board[move - 1] = 0;
                    print_board(board);
                    move = _MOVE | (move - 1);
                    send(sockfd, &move, MSGSIZE, 0);
                    break;
                case _ENEMYTURN:
                    printf("Opponent's turn.\n");
                    break;
                case _NOENEMY:
                    printf("Server is finding you an opponent\n");
                    break;
                case _MOVE:
                    move = din & DATAMASK;
                    board[move] = 1;
                    print_board(board);
                    break;
                case _YOUWIN:
                    printf("\nYOU WON!\n\n");
                    return 0;
                case _YOULOSE:
                    printf("\nYou lost...\n\n");
                    return 0;
                case _STOP:
                    printf("Server disconnected\n");
                    return 0;
            }
        }
    }

    return 0;
}