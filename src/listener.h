#ifndef LISTENER
#define LISTENER

#include "global_fn.h"
#include "repl.h"
#include "string.h"
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MAX_PIPES 50
#define MAX_ARGS 100
#define MAX_ARG_LEN 200

void listener_loop(char*);
void parse_command(char*, char***);
void handle_pipe_request(Message*);
char*** alloc_cmd(int x, int y, int z);
void init_cmd(char*** cmd, int x, int y, int z);
void free_cmd(char*** cmd, int x, int y);

#endif
