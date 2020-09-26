#ifndef _OPTION_PARSER_H_
#define _OPTION_PARSER_H_

#include <stdlib.h>
#include "error.h"

typedef dnsd_err (*handler_cmd)(int argc, char **argv, int *i);

struct options
{
    handler_cmd     handle;         /*!< handle function*/
    const char      *cmd_name;      /*!< name of the command*/
};

/**
 * Parse options from the command line
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err    parse_options(int argc, char **argv);

#endif /* ! OPTION_PARSER_H */
