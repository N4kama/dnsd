#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "option_parser.h"
#include "options_cmd.h"
#include "error.h"

#define OPT_NUMBER 7

static struct options opts[] =
{
    { .handle = my_help     , .cmd_name = "--help"     },
    { .handle = my_version  , .cmd_name = "--version"  },
    { .handle = my_file     , .cmd_name = "-f"         },
    { .handle = my_pudp     , .cmd_name = "-pudp"      },
    { .handle = my_ptcp     , .cmd_name = "-ptcp"      },
    { .handle = my_v4       , .cmd_name = "-v4"        },
    { .handle = my_v6       , .cmd_name = "-v6"        },
    { 0 },
};

dnsd_err parse_options(int argc, char **argv)
{
    dnsd_err code;

    for (int i = 1; i < argc; i++)
    {
        code = ERR_PARSE_BADOPT;
        // printf("\n%s\n", argv[i]);

        /*if (strncmp(argv[i], "-", 1) != 0)
        {
            int len  = argc - i;
            int i_initial = i;
            set_arg_zero(argv[i]);
            char **args_path = s_calloc(sizeof(char *), len);
            for (int k = 0; k < len && i < argc; k++, i++)
                args_path[k] = my_strcpy(argv[i]);
            for (int k = 0; k < len; k++)
                free(args_path[k]);
            free(args_path);
        }*/

        for (int j = 0; j < OPT_NUMBER; j++)
        {
            if (i < argc && strcmp(argv[i], opts[j].cmd_name) == 0)
                code = opts[j].handle(argc, argv, &i);
        }

        if (code != ERR_OK)
        {
            my_help_error();
            return code;
        }
    }
    return code;
}
