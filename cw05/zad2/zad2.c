#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NADAWCA "nadawca"
#define DATA "data"
#define WRITE "w"
#define READ "r"

typedef struct
{
    char* from;
    char* body;
    char* date;
} mail_struct;

int send_mail(char* to_email, char* subject, char* body)
{
    char* mmm = "mail -s";
    char* cmd = calloc(strlen(mmm) + strlen(to_email) + strlen(subject) + 1, sizeof(char));
    sprintf(cmd, "%s %s %s", mmm, subject, to_email);

    FILE *stream_mail = popen(cmd, WRITE);
    if (stream_mail == NULL)
    {
        perror("cannot popen");
        return 1;
    }
    fwrite(body, sizeof(char), strlen(body), stream_mail);
    pclose(stream_mail);

    free(cmd);

    return 0;
}

int display_emails(int sort_by)
{
    FILE *stream_mail = popen("echo exit | mail", READ);
    if (stream_mail == NULL)
    {
        perror("cannot popen");
        return 1;
    }
    
    char buf[_SC_LINE_MAX];
    mail_struct mail_messages[8192];
    int cnt = 0, idx = 0;
    while (fgets(buf, _SC_LINE_MAX, stream_mail) != NULL)
    {
        if (sort_by == 0)       // sort by date
        {


        }
        else                    // sort by from
        {

        }
    }
    // ...
    pclose(stream_mail);
    return 0;
}

int main(int argc, char *args[])
{
    if (argc == 2)
    {
        if (strcmp(args[1], NADAWCA) == 0)
        {
            if (display_emails(0)) 
                exit(EXIT_FAILURE);
        }
        else if (strcmp(args[1], DATA) == 0)
        {
            if (display_emails(1)) 
                exit(EXIT_FAILURE);
        }
        else
        {
            printf("Invalid argument");
            exit(EXIT_FAILURE);
        }
    }
    else if (argc == 4)
    {
        if (send_mail(args[1], args[2], args[3])) 
            exit(EXIT_FAILURE);
    }
    else
    {
        printf("Invalid args number");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}