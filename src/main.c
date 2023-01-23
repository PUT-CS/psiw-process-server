#include "global_fn.h"
#include "listener.h"
#include "repl.h"

int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    signal(SIGINT, my_sigint_handler);

    if (argc != 2) {
        puts("Invalid usage: username required");
        my_exit();
    }

    if (!fork())
        listener_loop(argv[1]);
    else
        repl_loop(argv[1]);

    return 0;
}
