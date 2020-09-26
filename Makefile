CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -I include/

SRC = \
	  src/common.c \
	  src/error.c \
	  src/zone_file_parser.c \
	  src/process_query.c \
	  src/server.c \
	  src/signal_handler.c \

OBJ = $(SRC:.c=.o) main.c

OBJ_TEST = $(SRC:.c=.o) unit_tests/unit.c

dnsd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

unit: $(OBJ_TEST)
	$(CC) $(CFLAGS) -o $@ $^
	./unit

test: dnsd
	./test.sh dnsd

install : dnsd
all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd unit
