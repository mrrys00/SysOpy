#include <stdlib.h>
#include <time.h>
#include <mqueue.h>

#ifndef CONFIG_H
#define CONFIG_H

#define T_INIT 01L
#define T_STOP 02L
#define T_LIST 03L
#define T_TOALL 04L
#define T_TOONE 05L
#define T_ERROR 06L

#define ERR_NOTFOUND -9

#define MAXMESLEN 500
#define MAXQUEMES 10
#define MAXCLINUM 128
#define MAXCLINAM 16

#define __NAME_SERVER "/server"
#define __NAME_CLIENT "/client"

#define LOGNAME "logs.log"

typedef struct {
    long mtype;
    int  mto;
    int  mfrom;
    time_t mtime;
    char mtext[MAXMESLEN];
    char clina[MAXCLINAM];
} message_t;

typedef struct {
    mqd_t queue_id;
    char client_name[MAXCLINAM];
} client_t;

#endif