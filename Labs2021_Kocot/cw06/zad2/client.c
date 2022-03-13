#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <limits.h>
#include <time.h>
#include "consts.h"
#include <errno.h>

#define MAXLINE 128
#define C_NOCOMMAND -1
#define C_DISCONNECT -2
#define C_LIST -3
#define C_BADNUM -4

int cliQ, servQ, cid = -1, partnerQ = -1;
bool quit = false;
pid_t pid = -1;

int parse(char* line) {
    if(line[0] == '/') {
        if(line[1] == 'c') {
            if(line[2] == '\0' || line[3] < '0' || line[3] > '9') return C_BADNUM;
            return atoi(&line[3]);
        }
        else if(line[1] == 'd') return C_DISCONNECT;
        else if(line[1] == 'l') return C_LIST;
    }
    return C_NOCOMMAND;
}

int sendint(int msqid, long type, int mint) {
    msgbuf mes;
    mes.mtype = type;
    mes.mint = mint;
    mes.mfrom = cid;
    return msgsnd(msqid, &mes, sizeof(mes), 0);
}

int sendpacket(int msqid, long type, int mint, char* mtext) {
    msgbuf mes;
    mes.mtype = type;
    mes.mint = mint;
    mes.mfrom = cid;
    strcpy(mes.mtext, mtext);
    return msgsnd(msqid, &mes, sizeof(mes), 0);
}

void stop(int signo) {
    if(pid != -1) kill(pid, SIGKILL);
    printf(" exiting...\n");
    sendint(servQ, T_STOP, cid);
    msgctl(cliQ, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char * argv[]) {
    key_t key, servkey;
    msgbuf mes;
    char inp[MAXLINE];
    int comm, rec;

    key = ftok(HASHFILE, CLIENTID);printf("%d -> %d (%d)\n", CLIENTID, key, errno);
    servkey = ftok(HASHFILE, SERVID);

    servQ = msgget(servkey, 0); 
    cliQ = msgget(key, 0666 | IPC_CREAT);

    sendint(servQ, T_INIT, key);
    msgrcv(cliQ, &mes, sizeof(mes), T_INIT, 0);
    cid = mes.mint;
    printf("ClientID: %d\n", cid);


    if((pid = fork()) == 0) {
        while(true) {
            fgets(inp, MAXLINE, stdin);
            comm = parse(inp);
            if(comm == C_DISCONNECT) {
                sendint(servQ, T_DISCONNECT, 0);
            } else if(comm == C_BADNUM) {
                printf("Incorrect argument\n");
            } else if(comm == C_LIST) {
                sendint(servQ, T_LIST, 0);
            } else if(comm >= 0) {
                sendint(servQ, T_CONNECT, comm);
            } else {
                sendpacket(cliQ, T_RELAY, cid, inp);
            }
        }
    }
    else {
        signal(SIGINT, stop);
        while(true) {
            rec = msgrcv(cliQ, &mes, sizeof(mes), -1000, IPC_NOWAIT & 0);
            if(rec > 0) {
                switch(mes.mtype) {
                    case T_CONNECT:
                        printf("connect\n");
                        partnerQ = msgget(mes.mint, 0);
                        break;
                    case T_CHAT:
                        printf(" chat> %s", mes.mtext);
                        break;
                    case T_DISCONNECT:
                        printf("disconnect\n");
                        partnerQ = -1;
                        break;
                    case T_RELAY:
                        if(partnerQ != -1) sendpacket(partnerQ, T_CHAT, mes.mint, mes.mtext);
                        break;
                    case T_LIST:
                        printf("\n%s\n", mes.mtext);
                        break;
                    case T_ERROR:
                        if(mes.mint == ERR_SELF) {
                            printf("ERR_SELF: Trying to talk with oneself\n");
                        } else if(mes.mint == ERR_TAKEN) {
                            printf("ERR_SELF: Chosen client is taken\n");
                        } else if(mes.mint == ERR_BUSY) {
                            printf("ERR_BUSY: Another connection already established\n");
                        } else if(mes.mint == ERR_NOTFOUND) {
                            printf("ERR_NOTFOUND: Chosen client not found\n");
                        }
                        break;
                    case T_STOP:
                        stop(SIGINT);
                }
            }
        }
    }
}
