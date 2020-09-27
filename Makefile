CC = gcc
CFLAGS = -g -std=c11 -Wall -Werror -Wextra -pedantic -I include/

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
BIN = dnsd

$(BIN): src/main.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

unit: tests/unit_tests/unit.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	./unit

test: $(BIN)
	./tests/test.sh $(BIN)

install: $(BIN)
all: $(BIN)

.PHONY: clean

clean:
	$(RM) $(OBJ)
	$(RM) src/main.o $(BIN)
	$(RM) tests/unit_tests/unit.o unit
