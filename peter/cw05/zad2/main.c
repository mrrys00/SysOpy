#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* sender;
    char* mail_preview_line;
} mail_t;

void send_mail(char* target, char* subject, char* body) {
    char* command = calloc(strlen(target) + strlen(subject) + strlen("mail -s  ") + 1, sizeof(char));
    strcpy(command, "mail -s ");
    strcat(command, subject);
    strcat(command, " ");
    strcat(command, target);
    FILE* mail_io_stream = popen(command, "w");
    if (mail_io_stream == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    fwrite(body, sizeof(char), strlen(body), mail_io_stream);
    free(command);
    pclose(mail_io_stream);
}

char* get_sender(char* line) {
    int word_count = 0;
    char sender[1000];
    int sender_length = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (i > 0 && isspace(line[i - 1]) && !isspace(line[i])) {
            ++word_count;
        }
        if (word_count == 2 && !isspace(line[i])) {
            sender[sender_length] = line[i];
            ++sender_length;
        }
    }
    char* result = calloc(strlen(sender) + 1, sizeof(char));
    strcpy(result, sender);
    return result;
}

void sort_mails_by_sender(mail_t mails[], int mails_count) {
    for (int i = 0; i < mails_count - 1; ++i) {
        for (int j = i + 1; j < mails_count; ++j) {
            if (strcmp(mails[i].sender, mails[j].sender) > 0) {
                mail_t tmp = mails[i];
                mails[i] = mails[j];
                mails[j] = tmp;
            }
        }
    }
}

void display_sorted_mails(int use_date_sorting) {
    FILE* mail_io_stream = popen("echo exit | mail", "r");
    if (mail_io_stream == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    char buffer[1000];
    mail_t mails[10000];
    int mails_length = 0;
    int line_idx = 0;
    while (fgets(buffer, 1000, mail_io_stream) != NULL) {
        if (use_date_sorting) {
            printf("%s", buffer);
        } else {
            if (line_idx != 0) {
                mail_t new_mail;
                char* sender = get_sender(buffer);
                new_mail.mail_preview_line = calloc(strlen(buffer) + 1, sizeof(char));
                strcpy(new_mail.mail_preview_line, buffer);
                new_mail.sender = sender;
                mails[mails_length] = new_mail;
                ++mails_length;
            }
            ++line_idx;
        }
    }
    if (!use_date_sorting) {
        sort_mails_by_sender(mails, mails_length);
        for (int i = 0; i < mails_length; ++i) {
            printf("%s", mails[i].mail_preview_line);
        }
    }
    pclose(mail_io_stream);
}

int main(int argc, char** argv) {
    if (argc == 4) {
        send_mail(argv[1], argv[2], argv[3]);
    } else if (argc == 2) {
        if (strcmp(argv[1], "nadawca") == 0) {
            display_sorted_mails(0);
        } else if (strcmp(argv[1], "data") == 0) {
            display_sorted_mails(1);
        } else { 
            printf("Invalid argument");
            return EXIT_FAILURE;
        }
    } else {
        printf("please provide 1 or 3 arguments");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}