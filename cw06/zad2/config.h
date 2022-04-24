#include <stdlib.h>

#define T_STOP 010L
#define T_DISCONNECT 013L
#define T_LIST 016L
#define T_CHAT 021L
#define T_RELAY 024L
#define T_CONNECT 027L
#define T_INIT 032L
#define T_ERROR 035L
#define KEYPATH getenv("HOME")

#define ERR_TAKEN -6  // chosen client is take
#define ERR_SELF -7  // client asks to talk with itself
#define ERR_BUSY -8  // client who asks is busy
#define ERR_NOTFOUND -9  // no such client
#define ERR_NOCONN -10  // not connected
#define ERR_KEYTAKEN -11  // another client logged on that key

#define MAXMESLEN 256  // maximum length of messages
#define MAXCLINUM 128  // maximum number of clients

#define SERVERID 1  // it'll never collide with client IDs
#define CLIENTID (getpid() % 214) + 2  // always >= 2
//#define CLIENTID (atoi(argv[1]) % 214) + 2  // always >= 2

typedef struct message_t {
    long mtype;
    int  mint;
    int  mfrom;
    char mtext[MAXMESLEN];
} message_t;
