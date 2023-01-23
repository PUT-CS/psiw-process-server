#include "global_fn.h"

void my_exit(){
    kill(-getpgid(getpid()), SIGINT);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void my_sigint_handler(int signum) {
    kill(-getpgid(getpid()), SIGTERM);
}
