#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "options.h"

static opt g_opt = {
    .v4 = { .s_addr = 0 },
    .v6 = { 0 },
    .p_udp = 53,
    .p_tcp = 53,
    .file = NULL
};

void set_file(char *path)
{
    g_opt.file = path;
}

char *get_file(void)
{
    return g_opt.file;
}

void set_v4(struct in_addr ip_v4)
{
    g_opt.v4 = ip_v4;
}

struct in_addr get_v4(void)
{
    return g_opt.v4;
}

void set_v6(struct in6_addr ip_v6)
{
    g_opt.v6 = ip_v6;
}

struct in6_addr get_v6(void)
{
    return g_opt.v6;
}

void set_port_udp(char *p_udp)
{
    g_opt.p_udp = atoi(p_udp);
}

uint16_t get_port_udp(void)
{
    return g_opt.p_udp;
}

void set_port_tcp(char *p_tcp)
{
    g_opt.p_tcp = atoi(p_tcp);
}

uint16_t get_port_tcp(void)
{
    return g_opt.p_tcp;
}
