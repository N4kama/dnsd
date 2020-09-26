#ifndef _OPTION_PARSER_H_
#define _OPTION_PARSER_H_

#include <stdlib.h>
#include "error.h"

typedef dnsd_err (*handler)(int argc, char **argv, int *i);

struct options
{
    handler     handle;         /*!< handle function*/
    const char  *cmd_name;      /*!< name of the command*/
};

dnsd_err    parse_options(int argc, char **argv);

#endif /* ! OPTION_PARSER_H */
