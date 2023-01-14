#include "ctype.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "vecint.h"
#include "sys/msg.h"
#include "repl.h"
#include "listener.h"

#define MAX_CONFIG_BUF_SIZE 4096

char* strstrip(char* s)
{
    size_t size;
    char* end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

void parse_command()
{
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    char str[] = "ls -la | tr A-Z | wc -l";

    for (str1 = str;; str1 = NULL) {
        token = strtok_r(str1, "|", &saveptr1);
        if (token == NULL) {
            break;
        }
        printf("%s:\n", strstrip(token));

        for (str2 = token;; str2 = NULL) {
            subtoken = strtok_r(str2, " ", &saveptr2);
            if (subtoken == NULL) {
                break;
            }
            printf("\t%s\n", strstrip(subtoken));
        }
    }
}

int get_requested_user_key(char* username)
{
    char config_buf[MAX_CONFIG_BUF_SIZE];
    int fd, n;
    int key = -1;

    if ((fd = open("config", O_RDONLY, MAX_CONFIG_BUF_SIZE)) == -1) {
        perror("Open config file");
        exit(1);
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
                return atoi(subtoken);
            }
            if (strcmp(subtoken, username) == 0) {
                printf("%s\n", strstrip(subtoken));
                conv_and_return = 1;
            }
        }
    }
    if (key == -1) {
        puts("User not found or configuration file misformatted.");
        exit(1);
    }
    return -1;
}



int main(int argc, char** argv)
{
    if (argc != 2) {
        puts("Invalid usage: username required");
        exit(EXIT_FAILURE);
    }
    int key = get_requested_user_key(argv[1]);
    printf("key: %d\n", key);

    int pid;
    
    if ((pid = fork()) == 0) {
        // child
        listener_loop();
    } else {
        // parent
        repl_loop(argv[1]);
    }
        

    /* Message msg; */
    /* msg.type = 1; */
    /* strcpy(msg.command, "Chuj"); */
    
    /* msgsnd(key, &msg, sizeof(Message)-sizeof(msg.type), 0); */

    /* msgrcv(key, &msg, sizeof(Message)-sizeof(msg.type), 1, 0); */
    
    return EXIT_SUCCESS;
}

