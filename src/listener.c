#include "listener.h"
#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

void handle_pipe_request(Message* msg) {
    // msg info is a system command like "ls | wc"
    // the task is to execute the command and write
    // the output to specified fifo
    puts("Handling pipe request");
    msg->type--;
}

void handle_pipe_done(Message* msg) {
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

        /* switch (received_message.msg_type) { */
        /* case PipeRequest: */
        /*     printf("\nReceived a pipe request\n"); */
        /*     handle_pipe_request(&received_message); */
        /*     break; */
        /* case PipeDone: */
        /*     printf("\nReceived a pipe done info"); */
        /*     handle_pipe_done(&received_message); */
        /*     break; */
        /* default: */
        /*     printf("\nUnknown message type\n"); */
        /*     break; */
        /* } */
    }
    printf("Listener exiting!");
}
