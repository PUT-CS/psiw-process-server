#include "listener.h"
#include "global_fn.h"
#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "string.h"

void parse_command(char* msg)
{
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;

    for (str1 = msg;; str1 = NULL) {
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

void handle_pipe_request(Message* msg)
{
    // msg info is a system command like "ls | wc"
    // the task is to execute the command and write
    // the output to specified fifo
    puts("Handling pipe request");
    parse_command(msg->info);
}

void handle_pipe_done(Message* msg)
{
    // msg info is none, the task is to read the requested
    puts("Handling a done pipe");
    msg->type--;
}

void listener_loop(char* username)
{
    key_t key = get_requested_user_key(username);
    // create a listening msg queue
    int msgid = msgget(key, 0666 | IPC_CREAT);
    printf("Listening on %d", msgid);
    Message received_message;

    while (1) {
        msgrcv(msgid, &received_message,
            sizeof(Message) - sizeof(long int), 1, 0);
        message_print(&received_message);

        switch (received_message.msg_type) {
        case PipeRequest:
            printf("\nReceived a pipe request\n");
            handle_pipe_request(&received_message);
            break;
        case PipeDone:
            printf("\nReceived a pipe done info");
            handle_pipe_done(&received_message);
            break;
        default:
            printf("\nUnknown message type\n");
            break;
        }
    }
    printf("Listener exiting!");
}
