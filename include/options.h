#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdint.h>
#include <arpa/inet.h>

typedef struct
{
    struct in_addr  v4;
    struct in6_addr v6;
    uint16_t         p_udp;
    uint16_t         p_tcp;
    char            *file;
} opt;

void            set_file(char *path);
char            *get_file(void);

void            set_v4(struct in_addr ip_v4);
struct in_addr  get_v4(void);

void            set_v6(struct in6_addr ip_v6);
struct in6_addr get_v6(void);

void            set_port_udp(char *p_udp);
uint16_t        get_port_udp(void);

void            set_port_tcp(char *p_tcp);
uint16_t        get_port_tcp(void);

#endif /* ! OPTIONS_H */
