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

bool quit = false;
msgbuf mes = {0L, 0, 0, ""};
msgbuf mes_out = {0L, 0, 0, ""};

int findempty(int* cliQ) {  // find next empty client room
    for(int i=0; i<MAXCLINUM; i++) {
        if(cliQ[i] == -1) {
            return i;
        }
    }
    return -1;
}

bool keytaken(int checked, int* cliQ, int* keys) {
    for(int i=0; i<MAXCLINUM; i++) {
        if(cliQ[i] != -1 && keys[i] == checked) {
            return true;
        }
    }
    return false;
}

int sendint(int msqid, long type, int mint) {
    mes_out.mtype = type;
    mes_out.mint = mint;
    mes_out.mfrom = -1;
    return msgsnd(msqid, &mes_out, sizeof(mes_out), 0);
}

int sendpacket(int msqid, long type, int mint, char* mtext) {
    mes_out.mtype = type;
    mes_out.mint = mint;
    mes_out.mfrom = -1;
    strcpy(mes_out.mtext, mtext);
    return msgsnd(msqid, &mes_out, sizeof(mes_out), 0);
}

void stopserver(int signo) {
    quit = true;
}

int main(int argc, char * argv[]) {
    key_t key;
    int servQ, cid, tpos;
    int p1, p2;
    ssize_t rec;

    signal(SIGINT, stopserver);

    int cliQ[MAXCLINUM];
    int partner[MAXCLINUM];
    key_t keys[MAXCLINUM];
    for(int i=0; i<MAXCLINUM; i++) {
        cliQ[i] = -1;
    }

    key = ftok(HASHFILE, SERVID);

    servQ = msgget(key, 0666 | IPC_CREAT);

    while(!quit) {
        rec = msgrcv(servQ, &mes, sizeof(mes), -1000, IPC_NOWAIT);
        if(rec > 0) switch(mes.mtype) {
            case T_INIT:
                cid = findempty(cliQ);
                printf("INIT - Assigning ID = %d\n", cid);
                keys[cid] = mes.mint;
                cliQ[cid] = msgget(mes.mint, 0666);
                partner[cid] = -1;  // set as non-talking
                sendint(cliQ[cid], T_INIT, cid);
                cid++;
                break;
            case T_STOP:
                printf("STOP - Client %d is exiting\n", mes.mfrom);
                if(partner[mes.mfrom] != -1) {
                    sendint(cliQ[partner[mes.mfrom]], T_DISCONNECT, 0);
                    partner[partner[mes.mfrom]] = -1;
                }
                cliQ[mes.mfrom] = -1;
                partner[mes.mfrom] = -1;
                break;
            case T_LIST:
                printf("LIST - Client %d asks for a list\n", mes.mfrom);
                mes.mtype = T_LIST;
                tpos = 0;
                for(int i=0; i<MAXCLINUM; i++) {
                    if(cliQ[i] != -1) {
                        tpos += sprintf(&mes.mtext[tpos], "\t%d -> ", i);
                        if(partner[i] == -1) tpos += sprintf(&mes.mtext[tpos], "free\n");
                        else tpos += sprintf(&mes.mtext[tpos], "talking\n");
                    }
                }
                msgsnd(cliQ[mes.mfrom], &mes, sizeof(mes), MSG_NOERROR);
                break;
            case T_CONNECT:
                printf("CONNECT - Client %d tries to connect with client %d\n", mes.mfrom, mes.mint);
                p1 = mes.mfrom;
                p2 = mes.mint;
                if(partner[p1] != -1) {
                   sendint(cliQ[p1], T_ERROR, ERR_BUSY);
                } else if(p1 == p2) {
                    sendint(cliQ[p1], T_ERROR, ERR_SELF);
                } else if(p2 >= MAXCLINUM || p2 < 0 || cliQ[p2] == -1) {
                    sendint(cliQ[p1], T_ERROR, ERR_NOTFOUND);
                } else if(partner[p2] != -1) {
                    sendint(cliQ[p1], T_ERROR, ERR_TAKEN);
                } else {
                    partner[p1] = p2;
                    partner[p2] = p1;
                    sendint(cliQ[p1], T_CONNECT, keys[p2]);
                    sendint(cliQ[p2], T_CONNECT, keys[p1]);
                }
                break;
            case T_DISCONNECT:
                printf("DISCONNECT - Client %d tries to disconnect\n", mes.mfrom);
                p1 = mes.mfrom;
                p2 = partner[p1];
                if(p2 == -1) {
                    sendint(cliQ[p1], T_ERROR, ERR_NOCONN);
                }
                else{
                    partner[p1] = -1;
                    partner[p2] = -1;
                    sendint(cliQ[p1], T_DISCONNECT, 0);
                    sendint(cliQ[p2], T_DISCONNECT, 0);
                }
                break;
        }
    }
    int running = 0;
    for(int i=0; i<MAXCLINUM; i++) {
        if(cliQ[i] != -1) {
            sendint(cliQ[i], T_STOP, 0);
            running++;
        }
    }
    while(running > 0) {
        rec = msgrcv(servQ, &mes, sizeof(mes), T_STOP, IPC_NOWAIT);
        if(rec > 0) {
            printf("STOP - Client %d is exiting\n", mes.mfrom);
            cliQ[mes.mfrom] = -1;
            running--;
        }
    }
    printf("All clients closed. Exiting...\n");
    msgctl(servQ, IPC_RMID, NULL);
}
