#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "config.h"

#define MAXLINE 512
#define C_NOSUCHCOMMAND -1
#define C_NOTKNOWNARG -2

mqd_t client_queue, server_queue;
int client_id = -1;
char client_name[MAXCLINAM];
pid_t pid = -1;
message_t in_message = {0L, 0, 0, 0, "", ""}, out_message = {0L, 0, 0, 0, "", ""};

void str_cut(char *res, char *line, int begin, int len)
{   
    int i=begin;
    for (; i<begin+len; i++)
        res[i-begin] = line[i];

    res[i-begin] = '\0';
    return;
}

void str_cut_to_char(char *res, char *line, int begin, int len, char last)
{
    if (line[begin] == ' ') begin++;
    int i=begin;
    for (; i<begin+len; i++)
    {
        if (line[i] == last)
            break;
        res[i-begin] = line[i];
    }

    res[i-begin] = '\0';
    return;
}

int handle(char *line)
{
    char cmd[5];
    str_cut(cmd, line, 0, 4);
    if (strcmp(cmd, "LIST") == 0)
        return T_LIST;
    else if (strcmp(cmd, "2ALL") == 0)
        return T_TOALL;
    else if (strcmp(cmd, "2ONE") == 0)
        return T_TOONE;
    else if (strcmp(cmd, "STOP") == 0)
        return T_STOP;
    return C_NOSUCHCOMMAND;
}

int sendint(mqd_t server_queue, long type, int mto, char *mtext)
{
    out_message.mtype = type;
    out_message.mto = mto;
    out_message.mfrom = client_id;
    strcpy(out_message.clina, client_name);
    strcpy(out_message.mtext, mtext);
    return mq_send(server_queue, (char*) &out_message, sizeof(out_message), type);
}

void stop_sigint(int signo)
{
    if (pid != -1)
        kill(pid, SIGKILL);
    
    sendint(server_queue, T_STOP, client_id, "");
    mq_close(client_queue);
    mq_unlink(__NAME_CLIENT);
    printf(" Exiting (SIGINT)...\n");
    exit(EXIT_SUCCESS);
}

void stop_terminal()
{
    sendint(server_queue, T_STOP, client_id, "");
    mq_close(client_queue);
    mq_unlink(__NAME_CLIENT);
    printf(" Exiting (terminal)...\n");
}


int main()
{
    char user_input[MAXLINE];
    int command, recieved;

    struct mq_attr attributes;
    attributes.mq_msgsize = sizeof(message_t);
    attributes.mq_maxmsg = MAXQUEMES;

    struct sigaction act;
    act.sa_handler = stop_sigint;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    sprintf(client_name, "%s%d", __NAME_CLIENT, getpid());
    server_queue = mq_open(__NAME_SERVER, O_RDWR, 0777, &attributes);
    client_queue = mq_open(client_name, O_RDWR | O_CREAT | O_EXCL, 0777, &attributes);

    sendint(server_queue, T_INIT, client_queue, "");
    printf("server_queue: %d\n", server_queue);
    printf("client_queue, client_name: %d\t%s\n", client_queue, client_name);
    mq_receive(client_queue, (char*) &in_message, sizeof(in_message), NULL);
    client_id = in_message.mto;
    
    printf("\nClientID: %d\n", client_id);

    if ((pid = fork()) == 0)
    {
        while (1)
        {
            fgets(user_input, MAXLINE, stdin);
            command = handle(user_input);
            if (command == C_NOTKNOWNARG)
            {
                printf("ERROR: No such command\n");
            }
            else if (command == T_LIST)
            {
                sendint(server_queue, T_LIST, 0, "");
            }
            else if (command == T_STOP)
            {   
                stop_terminal();
                if (pid != -1)
                    kill(pid, SIGKILL);
                exit(EXIT_SUCCESS);
            }
            else if (command == T_TOALL)
            {
                char mtext[MAXLINE];
                str_cut_to_char(mtext, user_input, 5, MAXLINE, '\0');
                sendint(server_queue, T_TOALL, -1, mtext);
            }
            else if (command == T_TOONE)
            {
                char client_to_str[MAXLINE], mtext[MAXLINE];
                int client_to;
                str_cut_to_char(client_to_str, user_input, 5, 3, ' ');
                client_to = atoi(client_to_str);
                str_cut_to_char(mtext, user_input, 5+strlen(client_to_str), MAXLINE, '\0');
                sendint(server_queue, T_TOONE, client_to, mtext);
            }
        }
    }
    else        // reciving messages
    {
        signal(SIGINT, stop_sigint);
        while (1)
        {
            recieved = mq_receive(client_queue, (char*) &in_message, sizeof(in_message), NULL);
            printf("waiting for heaven: …\n");
            if (recieved > 0)
            {
                switch (in_message.mtype)
                {
                case T_TOALL:
                    printf("message from %d to ALL at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
                    break;

                case T_TOONE:
                    printf("message from %d to you at %ld> %s", in_message.mfrom, in_message.mtime, in_message.mtext);
                    break;

                case T_LIST:
                    printf("\n%s\n", in_message.mtext);
                    break;

                case T_ERROR:
                    if (in_message.mto == ERR_NOTFOUND)
                    {
                        printf("WARNING: Client not found\n");
                    }
                    break;

                case T_STOP:
                    stop_sigint(SIGINT);
                }
            }
        }
    }
}
