#include "listener.h"

void loop_pipe(char*** cmd, Message* msg)
{
    int fds[2];
    int pid;
    int fd_in = 0;

    int dirfd = open("/tmp", O_RDONLY | O_DIRECTORY);
    int fd_fifo = openat(dirfd, msg->fifo_name, O_WRONLY);
    printf("\n");

    while (*cmd != NULL) {
        if (pipe(fds) == -1) {
            perror("Make pipe");
            my_exit();
        }
        if ((pid = fork()) == -1) {
            perror("Forking");
            my_exit();
        } else if (pid == 0) {
            dup2(fd_in, 0); // change the input according to the old one
            if (*(cmd + 1) != NULL)
                dup2(fds[1], 1);
            if (*(cmd + 1) == NULL) {
                // Last command
                dup2(fd_fifo, 1);
            }
            close(fds[0]);
            execvp(*(cmd)[0], *cmd);
            close(fd_fifo);
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            close(fds[1]);
            fd_in = fds[0]; // save the input for the next command
            cmd++;
        }
    }
    close(fd_fifo);
    close(dirfd);
}

void parse_command(char* msg, char*** save_cmd)
{
    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int i = 0, j = 0;

    for (str1 = msg;; str1 = NULL) {
        token = strtok_r(str1, "|", &saveptr1);
        if (token == NULL)
            break;
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
}

void handle_pipe_request(Message* msg)
{
    char*** cmd = alloc_cmd(MAX_PIPES, MAX_ARGS, MAX_ARG_LEN);
    parse_command(msg->info, cmd);
    loop_pipe(cmd, msg);
    free_cmd(cmd, MAX_PIPES, MAX_ARGS);
    exit(0);
}

void listener_loop(char* username)
{
    key_t key = get_requested_user_key(username);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    Message received_message;

    while (1) {
        msgrcv(msgid, &received_message,
            sizeof(Message) - sizeof(long int), 1, 0);

        if (fork() == 0)
            handle_pipe_request(&received_message);
    }
}

void free_cmd(char*** cmd, int x, int y)
{
    for (int i = 0; i < x; ++i)
        if (cmd[i] != NULL) {
            for (int j = 0; j < y; ++j)
                free(cmd[i][j]);
            free(cmd[i]);
        }
    free(cmd);
}

char*** alloc_cmd(int x, int y, int z)
{
    char*** cmd = (char***)(calloc(x, sizeof(char**)));
    for (int i = 0; i < x; i++) {
        cmd[i] = (char**)(calloc(y, sizeof(char*)));
        for (int j = 0; j < y; j++)
            cmd[i][j] = (char*)(calloc(z, sizeof(char)));
    }
    return cmd;
}
