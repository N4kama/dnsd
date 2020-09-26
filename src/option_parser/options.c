#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "options.h"

static opt *g_opt = NULL;

static void init_opt(void)
{
    if (g_opt != NULL)
        return;
   g_opt = calloc(1, sizeof(opt));
   g_opt->file = NULL;
   //FIX : Define values : if -f ... and no -v4 there is no IPv4 by default but if you give 0 options there is
}

void set_file(char *path)
{
    init_opt();
    if (g_opt != NULL && g_opt->file == NULL)
    {
        g_opt->file = malloc(strlen(path) + 1);
        strcpy(g_opt->file, path);
    }

    //TODO : Redefine path ?
}

char *get_file(void)
{
    if (g_opt->file != NULL)
        return g_opt->file;
    return NULL;
}

void set_v4(struct in_addr ip_v4)
{
    init_opt();
    if (g_opt != NULL)
        g_opt->v4 = ip_v4;
    return;
}

struct in_addr get_v4(void)
{
    if (g_opt != NULL)
        return g_opt->v4;

    struct in_addr inaddr;
    inet_pton(AF_INET, "127.0.0.1", &inaddr);
    return inaddr;
}

void set_v6(struct in6_addr ip_v6)
{
    init_opt();
    if (g_opt != NULL)
       g_opt->v6 = ip_v6;
    return;
}

struct in6_addr get_v6(void)
{
    if (g_opt != NULL)
        return g_opt->v6;

    struct in6_addr in6addr;
    inet_pton(AF_INET6, "::1", &in6addr);
    return in6addr;
}

void set_port_udp(char *p_udp)
{
    init_opt();
    if (g_opt != NULL)
    {    
        g_opt->p_udp = atoi(p_udp); //FIX ME: May break
        return;
    }
    g_opt->p_udp = 53;
}

uint16_t get_port_udp(void)
{
    if (g_opt != NULL)
    {
        if (g_opt->p_udp == 0)
            return 53;
        return g_opt->p_udp;
    }
    return 53;
}

void set_port_tcp(char *p_tcp)
{
    init_opt();
    if (g_opt != NULL)
    {
        g_opt->p_tcp = atoi(p_tcp); //FIX ME: May break
        return;
    }
    g_opt->p_tcp = 53;
}

uint16_t get_port_tcp(void)
{
    if (g_opt != NULL)
    {
        if (g_opt->p_tcp == 0)
            return 53;
        return g_opt->p_tcp;
    }
    return 53;
}
