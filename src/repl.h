#ifndef REPL
#define REPL

#include "stdio.h"
#include "readline/history.h"
#include "readline/readline.h"
#include "stdlib.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/msg.h"
#include "sys/stat.h"
#include "global_fn.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_CONFIG_BUF_SIZE 4096
#define MAX_USERS 2048
#define MAX_USERNAME_LENGTH 64
#define MAX_FIFO_FILENAME_LENGTH 64
#define MAX_OUTPUT_SIZE 8192

#define COLOR_BOLD "\e[1m"
#define COLOR_OFF "\e[m"

typedef struct {
    long int type;
    char info[MAX_COMMAND_LENGTH];
    char fifo_name[MAX_FIFO_FILENAME_LENGTH];
} Message;

void repl_loop(char* username);
char* strip_char(char* s, char c);
key_t get_requested_user_key(char* username);
void message_print(Message* msg);

#endif
