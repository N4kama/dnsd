CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -I include/

SRC = \
	  src/common.c \
	  src/error.c \
	  src/main.c \
	  src/process_query.c \
	  src/server.c \
	  src/signal_handler.c \
	  src/zone_file_parser.c

OBJ = $(SRC:.c=.o)

dnsd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

install : dnsd
all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd
