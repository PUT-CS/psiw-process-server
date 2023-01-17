#ifndef LISTENER
#define LISTENER

#include "repl.h"

#define MAX_PIPES 50
#define MAX_ARGS 100
#define MAX_ARG_LEN 200

void listener_loop(char*);
int parse_command(char*, char***);
void handle_pipe_request(Message*);
void handle_pipe_done(Message*);

#endif
