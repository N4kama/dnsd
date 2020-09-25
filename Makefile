CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99

SRC = $(wildcard ./src/*.c) \
$(wildcard ./src/server/*.c) \
$(wildcard ./src/request_handler/*.c) \
$(wildcard ./src/zone_file_parser/*.c)
# PLACE OTHER C FILE PATHS HERE

OBJ = $(SRC:.c=.o)

dnsd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd