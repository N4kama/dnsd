CC=gcc
CFLAGS= -g -Wall -Wextra -Werror -pedantic -std=c11 -I include/

SRC = src/common.c \
	  src/error.c \
	  src/zone_file_parser.c \
	  src/process_query.c \
	  src/server.c \
	  src/signal_handler.c \
      src/option_parser/option_parser.c \
      src/option_parser/options_cmd.c \
      src/option_parser/options.c


OBJ = $(SRC:.c=.o)

dnsd: $(OBJ) src/main.o
	$(CC) $(CFLAGS) -o $@ $^

unit: $(OBJ) src/unit_tests/unit.o
	$(CC) $(CFLAGS) -o $@ $^
	./unit

test: dnsd
	./test.sh dnsd

install : dnsd
all: dnsd

.PHONY: clean
clean:
	rm -f $(OBJ) dnsd unit
