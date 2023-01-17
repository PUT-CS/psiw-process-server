#include "listener.h"
#include "global_fn.h"
#include "repl.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

void loop_pipe(char*** cmd)
{
    int p[2];
    pid_t pid;
    int fd_in = 0;

    while (*cmd != NULL) {
        pipe(p);
        if ((pid = fork()) == -1) {
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            dup2(fd_in, 0); // change the input according to the old one
            if (*(cmd + 1) != NULL)
                dup2(p[1], 1);
            close(p[0]);
            execvp((*cmd)[0], *cmd);
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            close(p[1]);
            fd_in = p[0]; // save the input for the next command
            cmd++;
        }
    }
}

int parse_command(char* msg, char*** save_cmd)
{
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int i = 0, j = 0;

    for (str1 = msg;; str1 = NULL) {
        token = strtok_r(str1, "|", &saveptr1);
        if (token == NULL) {
            break;
        }
        if (i > MAX_PIPES) {
            puts("Exceeded the pipe number limit. Please compile the program with"
                 "different values in the flags that define the limit.");
            my_exit();
        }

        for (str2 = token;; str2 = NULL) {
            subtoken = strtok_r(str2, " ", &saveptr2);
            if (subtoken == NULL)
                break;
            strcpy(save_cmd[i][j], strip_char(subtoken, ' '));
            j++;
        }
        save_cmd[i][j] = NULL;
        j = 0;
        i++;
    }
    save_cmd[i] = NULL;
    return i;
}

void free_cmd(char*** cmd, int x, int y)
{
    size_t i, j;
    for (i = 0; i < x; ++i) {
        if (cmd[i] != NULL) {
            for (j = 0; j < y; ++j)
                free(cmd[i][j]);
            free(cmd[i]);
        }
    }
    free(cmd);
}

char*** alloc_cmd(int x, int y, int z)
{
    char*** cmd = (char***)(malloc(x * sizeof(char**)));
    for (int i = 0; i < x; i++) {
        cmd[i] = (char**)(malloc(y * sizeof(char*)));
        for (int j = 0; j < y; j++) {
            cmd[i][j] = (char*)(malloc(z * sizeof(char)));
        }
    }
    return cmd;
}

void init_cmd(char*** cmd, int x, int y, int z)
{
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z; k++) {
                cmd[i][j][k] = '\0';
            }
        }
    }
}

void handle_pipe_request(Message* msg)
{
    char*** cmd = alloc_cmd(MAX_PIPES, MAX_ARGS, MAX_ARG_LEN);
    init_cmd(cmd, MAX_PIPES, MAX_ARGS, MAX_ARG_LEN);
    int pipes = parse_command(msg->info, cmd);
    loop_pipe(cmd);
    free_cmd(cmd, MAX_PIPES, MAX_ARGS);
}

void handle_pipe_done(Message* msg)
{
    msg->type--;
}

void listener_loop(char* username)
{
    key_t key = get_requested_user_key(username);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    Message received_message;

    while (1) {
        msgrcv(msgid, &received_message,
            sizeof(Message) - sizeof(long int), 1, 0);
        message_print(&received_message);

        switch (received_message.msg_type) {
        case PipeRequest:
            handle_pipe_request(&received_message);
            break;
        case PipeDone:
            handle_pipe_done(&received_message);
            break;
        default:
            printf("\nUnknown message type\n");
            break;
        }
    }
}
