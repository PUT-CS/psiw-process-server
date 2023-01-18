#include "repl.h"

char* strip_char(char* s, char c)
{
    size_t size;
    char* end;
    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && *end == c)
        end--;
    *(end + 1) = '\0';

    while (*s && *s == c)
        s++;

    return s;
}

key_t get_requested_user_key(char* username)
{
    char config_buf[MAX_CONFIG_BUF_SIZE];
    int fd, n;
    int key = -1;

    if ((fd = open("config", O_RDONLY, MAX_CONFIG_BUF_SIZE)) == -1) {

        perror("Open config file");
        char* path = malloc(200);
        sprintf(path, "%s/.config/psiw-process-server/config", getenv("HOME"));
        printf("Checking in %s...\n", path);
        if ((fd = open(path, O_RDONLY, MAX_CONFIG_BUF_SIZE)) == -1) {
            perror("Open config in ~/.config");
            free(path);
            my_exit();
        }
        free(path);
    }

    while ((n = read(fd, &config_buf, MAX_CONFIG_BUF_SIZE)) > 0) { }
    close(fd);

    char *str1, *str2, *saveptr1, *saveptr2, *token, *subtoken;
    int conv_and_return = 0;

    for (str1 = config_buf;; str1 = NULL) {
        token = strtok_r(str1, "\n", &saveptr1);
        if (token == NULL)
            break;
        for (str2 = token;; str2 = NULL) {
            subtoken = strtok_r(str2, ":", &saveptr2);
            if (subtoken == NULL)
                break;
            if (conv_and_return == 1)
                return (key_t)atoi(subtoken);
            if (strcmp(subtoken, username) == 0)
                conv_and_return = 1;
        }
    }
    if (key == -1) {
        puts("User not found or configuration file misformatted.");
        my_exit();
    }
    return -1;
}

void spawn_reader_worker(char* fifo_name)
{
    int n;
    char* buf = (char*)(calloc(sizeof(char), MAX_OUTPUT_SIZE));

    int dirfd = open("/tmp", O_RDONLY | O_DIRECTORY);
    if (dirfd == -1) {
        perror("Open /tmp");
        my_exit();
    }

    int fd = openat(dirfd, fifo_name, O_RDONLY);
    if (fd == -1) {
        perror("Open FIFO in Reader Worker");
        my_exit();
    }

    while ((n = read(fd, buf, MAX_OUTPUT_SIZE)) > 0) { }
    printf("\n%s", buf);
    printf("\nPress RET to continue...\n");

    if (unlinkat(dirfd, fifo_name, 0) == -1) {
        perror("Delete FIFO in /tmp");
        my_exit();
    }

    free(buf);
    close(fd);
    close(dirfd);

    exit(0);
}

void repl_loop(char* login_username)
{
    char input[MAX_COMMAND_LENGTH + 1];
    char prompt[MAX_USERNAME_LENGTH + 30];
    int msgid;
    Message message;

    sprintf(prompt, COLOR_BOLD "[%s@process-server]$ " COLOR_OFF, login_username);

    while (1) {
        strcpy(input, readline(prompt));
        add_history(input);

        if (!strlen(input))
            continue;

        if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0)
            my_exit();

        char** tokens = history_tokenize(input);

        if (!tokens[0] | !tokens[1] | !tokens[2]) {
            puts("Invalid command format.\n"
                 "Usage: TARGET-USERNAME COMMAND FIFO_NAME");
            continue;
        }

        message.type = 1;
        strcpy(message.info, strip_char(tokens[1], '"'));
        strcpy(message.fifo_name, tokens[2]);

        // create a fifo, only then send the request (if it succedes)
        int dirfd;
        if ((dirfd = open("/tmp", O_RDONLY | O_DIRECTORY)) == -1) {
            perror("Open /tmp");
            continue;
        }
        
        if (mkfifoat(dirfd, message.fifo_name, S_IRUSR | S_IWUSR) == -1) {
            perror("mkfifoat");
            continue;
        }

        msgid = msgget(get_requested_user_key(tokens[0]), 0666);
        msgsnd(msgid, &message, sizeof(Message) - sizeof(long int), 0);

        if (fork() == 0)
            spawn_reader_worker(message.fifo_name);

        close(dirfd);
    }
}
