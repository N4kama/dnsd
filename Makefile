CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -I include/

SRC = $(wildcard ./src/*.c) \
$(wildcard ./src/server/*.c) \
$(wildcard ./src/request_handler/*.c) \
$(wildcard ./src/zone_file_parser/*.c) \
$(wildcard ./src/memory/*.c)
# PLACE OTHER C FILE PATHS HERE

OBJ = $(SRC:.c=.o)

dnsd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

install : dnsd
all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd
