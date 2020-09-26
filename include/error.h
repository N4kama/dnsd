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
} dnsd_err;

char *dnsd_strerror(dnsd_err code);
void exitOnError(char *msg);

#endif

// Error related functions
