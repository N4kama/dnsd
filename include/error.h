#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef enum {
    ERR_OK = 0,         // No error
    ERR_NOMEM,          // Out-of-memory error
    ERR_NOENT,          // No such file or directory
    ERR_PARSE_BADCNT,   // Bad value count
    ERR_PARSE_BADVAL,   // Bad value format
} dnsd_err;

char *dnsd_strerror(dnsd_err code);

#endif

// Error related functions
