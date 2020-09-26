#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef enum {
    ERR_OK = 0,         // No error
    ERR_NOMEM,          // Out-of-memory error
    ERR_NOENT,          // No such file or directory

    ERR_PARSE_BADCNT,   // Bad value count
    ERR_PARSE_BADVAL,   // Bad value format
    ERR_PARSE_BADOPT,   // Bad options for cmd

    ERR_SOCK_OPEN,      // Opening socket failed
    ERR_SOCK_BIND,      // Binding socket failed
    ERR_SOCK_LISTEN,    // Listening socket failed
    ERR_SOCK_ACCEPT,    // Accepting socket failed
    ERR_SOCK_RECV,      // Receiving data on socket failed

    ERR_FORK,           // Fork failed

    ERR_FORCED_SHUTDOWN,// Server shutdown
} dnsd_err;

char *dnsd_strerror(dnsd_err code);
void exitOnError(char *msg);

#endif

// Error related functions
