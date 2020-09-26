#ifndef _OPTIONS_CMD_H_
#define _OPTIONS_CMD_H_

#include <stdlib.h>
#include "error.h"

#define VERSION 1.0

/**
 * Fonction that display a help message
*/
void         my_help_error(void);

/**
 * Fonction that display a help message
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_help(int argc, char **argv, int *i);

/**
 * Fonction that display the version of the binary
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_version(int argc, char **argv, int *i);

/**
 * Fonction that check if there is an argument after
 * the -f option, then it set the file inside an opt struct
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_file(int argc, char **argv, int *i);

/**
 * Fonction that check if there is a numeric argument after
 * the -pudp option, then it set the UDP port
 * inside an opt struct
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_pudp(int argc, char **argv, int *i);

/**
 * Fonction that check if there is a numeric argument after
 * the -ptcp option, then it set the TCP port
 * inside an opt struct
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_ptcp(int argc, char **argv, int *i);

/**
 * Fonction that check if there is a valid IP address after
 * the -v4 option, then it set the IPv4 address
 * inside an opt struct
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_v4(int argc, char **argv, int *i);

/**
 * Fonction that check if there is a valid IP address after
 * the -v6 option, then it set the IPv6 address
 * inside an opt struct
 * 
 * @param argc: number of arguments from the command line
 * @param argv: arguments from the command line
 * @param i   : argument at position i
 * @return: dnsd_err, either ERR_OK or ERR_BADOPT
*/
dnsd_err     my_v6(int argc, char **argv, int *i);

#endif /* ! OPTION_CMD_H */
