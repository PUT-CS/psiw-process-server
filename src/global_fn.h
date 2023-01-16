#ifndef GLOBAL_FN
#define GLOBAL_FN

#include "signal.h"
#include "unistd.h"

void my_exit();
void my_sigint_handler(int signum);

#endif
