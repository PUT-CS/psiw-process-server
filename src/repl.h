#ifndef REPL
#define REPL
#include <sys/ipc.h>
#define MAX_COMMAND_LENGTH 1024
#define MAX_CONFIG_BUF_SIZE 4096
#define MAX_USERS 2048
#define MAX_USERNAME_LENGTH 64
#define MAX_FIFO_FILENAME_LENGTH 64
#define COLOR_BOLD "\e[1m"
#define COLOR_OFF "\e[m"

typedef enum {
    PipeRequest,
    PipeDone
} MessageType;

typedef struct {
    long int type;
    /* this is so that one struct can be used for both
       types of messages the listener has to handle */
    MessageType msg_type;
    char info[MAX_COMMAND_LENGTH];
    char fifo_name[MAX_FIFO_FILENAME_LENGTH];
} Message;

void repl_loop(char* username);
char* strip_char(char* s, char c);
key_t get_requested_user_key(char* username);
void message_print(Message* msg);

#endif
