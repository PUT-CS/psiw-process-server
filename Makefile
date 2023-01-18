CC = gcc
CC_FLAGS = -g3 -O2 -D_FORTIFY_SOURCE=2 -pipe -Wall -Wextra -Werror=format-security
LIBS = -lreadline
SRC = src/*.c

.PHONY: clean

server:
	$(CC) $(CC_FLAGS) -o server $(SRC) $(LIBS)
clean:
	rm -f server
