#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "options_cmd.h"
#include "options.h"
#include "error.h"

dnsd_err my_version(int argc, char **argv, int *i)
{
    if ((argc != 0 || argv[*i] != NULL))
        printf("Version %.1f\n", VERSION);
    exit(0); //TODO : change
}

dnsd_err my_help(int argc, char **argv, int *i)
{
    if ((argc != 0 || argv[*i] != NULL))
    {
        printf("dnsd, version %.1f\n", VERSION);
        printf("Usage:  dnsd [option] ...\n");
        printf("        dnsd [option] [file] ...\n");
        printf("Options:\n");
        printf("\t--help\n");
        printf("\t--version\n");
        printf("\t-v4 <IP>\n");
        printf("\t-v6 <IP>\n");
        printf("\t-pudp <port>\n");
        printf("\t-ptcp <port>\n");
        printf("\t-f <zone_file>\n");
    }
    exit(0); //TODO : change
}

void my_help_error(void)
{
    fprintf(stderr, "Usage: dnsd [option] ...\n");
    fprintf(stderr, "       dnsd [option] [file] ...\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t--help\n");
    fprintf(stderr, "\t--version\n");
    fprintf(stderr, "\t-v4 <IP>\n");
    fprintf(stderr, "\t-v6 <IP>\n");
    fprintf(stderr, "\t-pudp <port>\n");
    fprintf(stderr, "\t-ptcp <port>\n");
    fprintf(stderr, "\t-f <zone_file>\n");
}

dnsd_err my_file(int argc, char **argv, int *i)
{
    if (*i < argc)
    {
        (*i)++;
        if (*i < argc && (strncmp(argv[(*i)], "-", 1) == 0))
        {
            fprintf(stderr, "dnsd: -f %s: invalid argument\n", argv[*i]);
            return ERR_PARSE_BADOPT;
        }
        if (*i == argc)
        {
            fprintf(stderr, "dnsd: -f: option requires an argument\n");
            return ERR_PARSE_BADOPT;
        }
        else
        {
            set_file(argv[*i]);
            return ERR_OK;
        }
    }
    return ERR_PARSE_BADOPT;
}

dnsd_err my_pudp(int argc, char **argv, int *i)
{
    if (*i < argc)
    {
        (*i)++;
        if (*i < argc && (strncmp(argv[(*i)], "-", 1) == 0))
        {
            fprintf(stderr, "dnsd: -pudp %s: invalid argument\n", argv[*i]);
            return ERR_PARSE_BADOPT;
        }
        if (*i == argc)
        {
            fprintf(stderr, "dnsd: -pudp: option requires an argument\n");
            return ERR_PARSE_BADOPT;
        }
        else
        {
            for (size_t k = 0; k < strlen(argv[*i]); k++)
            {
                if (isdigit(argv[*i][k]) == 0)
                {
                    fprintf(stderr, "dnsd: -ptcp: Invalid UDP port : %s\n", argv[*i]);
                    return ERR_PARSE_BADOPT;
                }
            }
            set_port_udp(argv[*i]);
            return ERR_OK;
        }
    }
    return ERR_PARSE_BADOPT;
}

dnsd_err my_ptcp(int argc, char **argv, int *i)
{
    if (*i < argc)
    {
        (*i)++;
        if (*i < argc && (strncmp(argv[(*i)], "-", 1) == 0))
        {
            fprintf(stderr, "dnsd: -ptcp %s: invalid argument\n", argv[*i]);
            return ERR_PARSE_BADOPT;
        }
        if (*i == argc)
        {
            fprintf(stderr, "dnsd: -ptcp: option requires an argument\n");
            return ERR_PARSE_BADOPT;
        }
        else
        {
            for (size_t k = 0; k < strlen(argv[*i]); k++)
            {
                if (isdigit(argv[*i][k]) == 0)
                {
                    fprintf(stderr, "dnsd: -ptcp: Invalid TCP port : %s\n", argv[*i]);
                    return ERR_PARSE_BADOPT;
                }
            }
            set_port_tcp(argv[*i]);
            return ERR_OK;
        }
    }
    return ERR_PARSE_BADOPT;
}

dnsd_err my_v4(int argc, char **argv, int *i)
{
    if (*i < argc)
    {
        (*i)++;
        if (*i < argc && (strncmp(argv[(*i)], "-", 1) == 0))
        {
            fprintf(stderr, "dnsd: -v4 %s: invalid argument\n", argv[*i]);
            return ERR_PARSE_BADOPT;
        }
        if (*i == argc)
        {
            fprintf(stderr, "dnsd: -v4: option requires an argument\n");
            return ERR_PARSE_BADOPT;
        }
        else
        {
            struct in_addr inaddr;
            int rval;
            if ((rval = inet_pton(AF_INET, argv[*i], &inaddr)) == 1)
            {
                set_v4(inaddr);
                return ERR_OK;
            }
            else
            {
                fprintf(stderr, "dnsd: -v4: Invalid IP Address : %s\n", argv[*i]);
                return ERR_PARSE_BADOPT;
            }
        }
    }
    return ERR_PARSE_BADOPT;
}

dnsd_err my_v6(int argc, char **argv, int *i)
{
    if (*i < argc)
    {
        (*i)++;
        if (*i < argc && (strncmp(argv[(*i)], "-", 1) == 0))
        {
            fprintf(stderr, "dnsd: -v6 %s: invalid argument\n", argv[*i]);
            return ERR_PARSE_BADOPT;
        }
        if (*i == argc)
        {
            fprintf(stderr, "dnsd: -v6: option requires an argument\n");
            return ERR_PARSE_BADOPT;
        }
        else
        {
            struct in6_addr in6addr;
            int rval;
            if ((rval = inet_pton(AF_INET6, argv[*i], &in6addr)) == 1)
            {
                set_v6(in6addr);
                return ERR_OK;
            }
            else
            {
                fprintf(stderr, "dnsd: -v6: Invalid IP Address : %s\n", argv[*i]);
                return ERR_PARSE_BADOPT;
            }
        }
    }
    return ERR_PARSE_BADOPT;
}
