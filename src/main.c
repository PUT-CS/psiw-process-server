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
#include "signal.h"
#include <signal.h>

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
        printf("%s:\n", strip_char(token, ' '));

        for (str2 = token;; str2 = NULL) {
            subtoken = strtok_r(str2, " ", &saveptr2);
            if (subtoken == NULL) {
                break;
            }
            printf("\t%s\n", strip_char(subtoken, ' '));
        }
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void my_sigint_handler(int signum) {
    pid_t parent_pid = getpid();
    pid_t pgid = getpgid(parent_pid);
    kill(-pgid, SIGKILL);
}

int main(int argc, char** argv)
{
    // turn off stdout buffering for easier debugging
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGINT, my_sigint_handler);
    
    if (argc != 2) {
        puts("Invalid usage: username required");
        exit(EXIT_FAILURE);
    }
    
    int pid;
    if ((pid = fork()) == 0) {
        listener_loop(argv[1]);
    } else {
        repl_loop(argv[1]);
    }
    
    return EXIT_SUCCESS;
}

