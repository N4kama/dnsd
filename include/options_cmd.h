#ifndef _OPTIONS_CMD_H_
#define _OPTIONS_CMD_H_

#include <stdlib.h>
#include "error.h"

#define VERSION 1.0

void         my_help_error(void);
dnsd_err     my_help(int argc, char **argv, int *i);
dnsd_err     my_version(int argc, char **argv, int *i);
dnsd_err     my_file(int argc, char **argv, int *i);
dnsd_err     my_pudp(int argc, char **argv, int *i);
dnsd_err     my_ptcp(int argc, char **argv, int *i);
dnsd_err     my_v4(int argc, char **argv, int *i);
dnsd_err     my_v6(int argc, char **argv, int *i);

#endif /* ! OPTION_CMD_H */
