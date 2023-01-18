#include "global_fn.h"

void my_exit(){
    kill(-getpgid(getpid()), SIGINT);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void my_sigint_handler(int signum) {
    pid_t parent_pid = getpid();
    pid_t pgid = getpgid(parent_pid);
    kill(-pgid, SIGTERM);
}
