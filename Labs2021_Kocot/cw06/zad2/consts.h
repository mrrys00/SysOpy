#include <stdlib.h>

#define bool short
#define true 1
#define false 0
#define T_STOP 32L
#define T_DISCONNECT 30L
#define T_LIST 28L
#define T_CHAT 26L
#define T_RELAY 24L
#define T_CONNECT 22L
#define T_INIT 20L
#define T_ERROR 18L
#define HASHFILE getenv("HOME")

#define ERR_TAKEN -6  // chosen client is take
#define ERR_SELF -7  // client asks to talk with itself
#define ERR_BUSY -8  // client who asks is busy
#define ERR_NOTFOUND -9  // no such client

#define MAXMESLEN 100  // maximum length of messages
#define MAXCLINUM 10  // maximum number of clients

#define SERVID 1  // it'll never collide with client IDs
#define CLIENTID (getpid() % 214) + 2  // always >= 2

typedef struct msgbuf {
    long mtype;
    int  mint;
    int  mfrom;
    char mtext[MAXMESLEN];
} msgbuf;
