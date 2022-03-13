#define MAX_CLI 16
#define NAMELEN 20
#define PORT 8080
#define MESBUF 257

#define _NAMETAKEN 0100
#define _YOURTURN 0200
#define _ENEMYTURN 0300
#define _NOENEMY 0400
#define _YOUWIN 0500
#define _YOULOSE 0600
#define _DRAW 0700
#define _MOVE 01000
#define _YOUREX 01100
#define _YOUREO 01200
#define _STOP 01300

#define msg_t int
#define MSGSIZE sizeof(int)
#define MSGMASK 077700
#define DATAMASK 077

#define bool short
#define true 1
#define false 0
