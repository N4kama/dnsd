#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "options.h"

static opt g_opt = {
    .v4 = { .s_addr = 16777343 }, // 127.0.0.1
    .v6 = IN6ADDR_LOOPBACK_INIT,    // ::1
    .p_udp = 53,
    .p_tcp = 53,
    .file = "/etc/dnsd/dnsd.zone"
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

uint16_t get_port(int protocol)
{
    switch (protocol)
    {
    case SOCK_STREAM:
        return get_port_tcp();
    case SOCK_DGRAM:
        return get_port_udp();
    default:
        /* ERROR */
        return 0;
    }
}
