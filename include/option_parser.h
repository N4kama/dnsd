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

dnsd_err    parse_options(int argc, char **argv);

#endif /* ! OPTION_PARSER_H */
