#include "ctype.h"
#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "vecint.h"
#include "repl.h"
#include "listener.h"
#include "signal.h"
#include "global_fn.h"

int main(int argc, char** argv)
{
    // turn off stdout buffering for easier debugging
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGINT, my_sigint_handler);
    
    if (argc != 2) {
        puts("Invalid usage: username required");
        my_exit();
    }
    
    int pid;
    if ((pid = fork()) == 0) {
        listener_loop(argv[1]);
    } else {
        repl_loop(argv[1]);
    }
    
    return EXIT_SUCCESS;
}

