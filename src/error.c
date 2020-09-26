#include "error.h"

char *dnsd_strerror(dnsd_err code)
{
    switch (code)
    {
        case ERR_OK:
            return "OK";
        case ERR_NOMEM:
            return "Out-of-memory error";
        case ERR_NOENT:
            return "No such file or directory";
        case ERR_PARSE_BADCNT:
            return "Bad value count in zone file";
        case ERR_PARSE_BADVAL:
            return "Bad value format in zone file";
        default:
            return "Unknown error code";
    }
}

/**
 * exitOnError - wrapper for perror
 * @param msg a pointer to the error message
 */
void exitOnError(char *msg)
{
	perror(msg);
	exit(1);
}