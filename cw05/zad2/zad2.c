#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NADAWCA "nadawca"
#define DATA "data"
#define WRITE "w"
#define READ "r"
#define LNMAX 2048

typedef struct
{
    char *from;
    char *body;
    char *date;
} mail_struct;

char *get_from(char *ln)
{
    int cnt = 0, from_length = 0;
    char from[2048];
    for (int i = 0; ln[i] != '\0'; i++)
    {
        if (i > 0 && isspace(ln[i - 1]) && !isspace(ln[i]))
        {
            cnt++;
        }
        if (cnt == 2 && !isspace(ln[i]))
        {
            from[from_length] = ln[i];
            from_length++;
        }
    }
    char *res = calloc(strlen(from) + 1, sizeof(char));
    strcpy(res, from);
    return res;
}

void sort_from(mail_struct mails[], int mails_count)
{
    for (int i = 0; i < mails_count - 1; i++)
    {
        for (int j = i + 1; j < mails_count; j++)
        {
            if (strcmp(mails[i].from, mails[j].from) > 0)
            {
                mail_struct tmp = mails[i];
                mails[i] = mails[j];
                mails[j] = tmp;
            }
        }
    }
    return;
}

int send_mail(char *to_email, char *subject, char *body)
{
    char *mmm = "mail -s";
    char *cmd = calloc(strlen(mmm) + strlen(to_email) + strlen(subject) + 1, sizeof(char));
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

    char buf[LNMAX];
    mail_struct mail_messages[8192];
    int cnt = 0, idx = 0;
    while (fgets(buf, LNMAX, stream_mail) != NULL)
    {
        if (sort_by == 0) // sort by date
            printf("%s", buf);
        else // sort by from
        {
            if (idx != 0)
            {
                mail_struct new_mail;
                char *from = get_from(buf);
                new_mail.body = calloc(strlen(buf) + 1, sizeof(char));
                strcpy(new_mail.body, buf);
                new_mail.from = from;
                mail_messages[cnt] = new_mail;
                cnt++;
            }
            idx++;
        }
    }

    if (sort_by == 1)
    {
        sort_from(mail_messages, cnt);
        for (int i = 0; i < cnt; i++)
        {
            printf("%s", mail_messages[i].body);
        }
    }

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