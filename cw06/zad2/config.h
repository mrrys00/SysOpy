#include <stdlib.h>
#include <time.h>

#ifndef CONFIG_H
#define CONFIG_H

#define T_INIT 01L
#define T_STOP 02L
#define T_LIST 03L
#define T_TOALL 04L
#define T_TOONE 05L
#define T_ERROR 06L
#define KEYPATH getenv("HOME")

#define ERR_NOTFOUND -9

#define MAXMESLEN 500
#define MAXCLINUM 128

#define SERVERID 128  // unique ID
#define CLIENTID (getpid() % 256) + 2  // always >= 2

#define LOGNAME "logs.log"

typedef struct {
    long mtype;
    int  mto;
    int  mfrom;
    time_t mtime;
    char mtext[MAXMESLEN];
} message_t;

#endif