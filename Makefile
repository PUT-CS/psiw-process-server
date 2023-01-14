CC = gcc
CC_FLAGS = -g3 -O2 -D_FORTIFY_SOURCE=2 -pipe -Wall -Wextra -Werror=format-security
DEFINES = -DMAX_USERS=1000 -DMAX_USERNAME_LENGTH=64
LIBS = -lvecint -lreadline
SRC = src/*.c

.PHONY: clean

server:
	$(CC) $(CC_FLAGS) $(DEFINES) -o server $(SRC) $(LIBS)
clean:
	rm -f src/*.o server
