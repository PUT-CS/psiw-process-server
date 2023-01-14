#include "repl.h"
#include "stdio.h"
#include "string.h"
#include "readline/readline.h"
#include "readline/history.h"

#define MAX_COMMAND_LENGTH 1024

#define MAX_USERS 2048
#define MAX_USERNAME_LENGTH 64
#define MAX_FIFO_FILENAME_LENGTH 64

#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"

typedef struct {
    long type;
    char command[MAX_COMMAND_LENGTH];
    char fifo_name[MAX_FIFO_FILENAME_LENGTH];
} Request;

/* typedef struct { */
/*     char username[MAX_USERNAME_LENGTH]; */
/*     char command[MAX_COMMAND_LENGTH]; */
/*     char fifo_name[MAX_FIFO_FILENAME_LENGTH]; */
/* } ParseResult; */

/* ParseResult parse_input(char* input) { */
/*     // 1. read username until you encounter " */
/*     // 2. read command until you encounter " again */
/*     // 3. read fifo_name when you encounter " */
/*     // 4. end reading when you encounter " again. */

/*     ParseResult res; */

/*     char* pipe_start = strchr(input, '"'); */
/*     char* pipe_end = strchr(pipe_start+1, '"'); */
    
/*     strncpy(res.username, input, pipe_start-input); */
/*     printf("username: %s\n", res.username); */

/*     strncpy(res.command, pipe_start, pipe_end-pipe_start); */
/*     printf("command: %s\n", res.command); */
    
    
/*     return res; */
/* } */

void repl_loop(char* login_username)
{
    char input[MAX_COMMAND_LENGTH + 1];
    char prompt[MAX_USERNAME_LENGTH + 30];
    char target_username[MAX_USERNAME_LENGTH+1];
    char command[MAX_COMMAND_LENGTH+1];
    char fifo_name[MAX_FIFO_FILENAME_LENGTH+1];

    sprintf(prompt, COLOR_BOLD "[%s@process-server]$ " COLOR_OFF, login_username);
    while (1) {
        strcpy(input,readline(prompt));
        char** tokens = history_tokenize(input);
        
        if (!tokens[0] | !tokens[1] | !tokens[2]) {
            puts("Invalid command format");
            continue;
        }
        
        printf("%s\n", tokens[0]);
        printf("%s\n", tokens[1]);
        printf("%s\n", tokens[2]);
        
        strcpy(target_username, tokens[0]);
        strcpy(command, tokens[1]);
        strcpy(fifo_name, tokens[2]);
    }
}
