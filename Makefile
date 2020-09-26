CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -I include/

SRC = \
	  src/error.c \
	  src/main.c \
	  src/zone_file_parser.c

OBJ = $(SRC:.c=.o)

dnsd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

install : dnsd
all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd
