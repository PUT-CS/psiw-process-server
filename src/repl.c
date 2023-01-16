#include "repl.h"
#include "stdio.h"
#include "string.h"
#include "readline/history.h"
#include "readline/readline.h"
#include <sys/msg.h>
#include <sys/types.h>
#include "stdlib.h"
#include "ctype.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "vecint.h"
#include "sys/msg.h"
#include "sys/stat.h"
#include "global_fn.h"

void message_print(Message* msg) {
    printf("type: %ld\n", msg->type);
    printf("command: %s\n", msg->info);
    printf("fifo_name: %s\n", msg->fifo_name);
}

char* strip_char(char* s, char c)
{
    size_t size;
    char* end;
    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && *end == c)
        end--;
    *(end + 1) = '\0';

    while (*s && *s == c)
        s++;

    return s;
}

key_t get_requested_user_key(char* username)
{
    char config_buf[MAX_CONFIG_BUF_SIZE];
    int fd, n;
    int key = -1;

    if ((fd = open("config", O_RDONLY, MAX_CONFIG_BUF_SIZE)) == -1) {
        perror("Open config file");
        my_exit();
    }

    while ((n = read(fd, &config_buf, MAX_CONFIG_BUF_SIZE)) > 0) { }

    char *str1, *str2, *saveptr1, *saveptr2, *token, *subtoken;
    int conv_and_return = 0;

    for (str1 = config_buf;; str1 = NULL) {
        token = strtok_r(str1, "\n", &saveptr1);
        if (token == NULL)
            break;
        for (str2 = token;; str2 = NULL) {
            subtoken = strtok_r(str2, ":", &saveptr2);
            if (subtoken == NULL)
                break;
            if (conv_and_return == 1) {
                return (key_t)atoi(subtoken);
            }
            if (strcmp(subtoken, username) == 0) {
                conv_and_return = 1;
            }
        }
    }
    if (key == -1) {
        puts("User not found or configuration file misformatted.");
        my_exit();
    }
    return -1;
}

void repl_loop(char* login_username)
{
    char input[MAX_COMMAND_LENGTH + 1];
    char prompt[MAX_USERNAME_LENGTH + 30];
    int msgid;
    Message message;

    sprintf(prompt, COLOR_BOLD "[%s@process-server]$ " COLOR_OFF, login_username);
    
    while (1) {
        strcpy(input, readline(prompt));
        add_history(input);
        
        if (!strlen(input))
            continue;

        if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0)
            my_exit();

        char** tokens = history_tokenize(input);

        if (!tokens[0] | !tokens[1] | !tokens[2]) {
            puts("Invalid command format.\n"
                 "Usage: TARGET-USERNAME COMMAND FIFO_NAME");
            continue;
        }

        message.type = 1;
        message.msg_type = PipeRequest;
        strcpy(message.info, strip_char(tokens[1], '"'));
        strcpy(message.fifo_name, tokens[2]);
        
        /* if (mkfifo(message.fifo_name, 0666) == -1) { */
        /*     perror("Creating FIFO"); */
        /*     continue; */
        /* } */

        msgid = msgget(get_requested_user_key(tokens[0]), 0666);
        msgsnd(msgid, &message, sizeof(Message)-sizeof(long int), 0);
    }
}
