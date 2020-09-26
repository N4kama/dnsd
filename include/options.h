#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdint.h>
#include <arpa/inet.h>

typedef struct
{
    struct in_addr  v4;
    struct in6_addr v6;
    uint16_t        p_udp;
    uint16_t        p_tcp;
    char            *file;
} opt;

/**
 * Fonction that set the file path
 * inside an opt struct
 *
 * @param path: path of the file
*/
void            set_file(char *path);

/**
 * Fonction that get the file path
 * from an opt struct
 * 
 * @return: char *, path of the file
*/
char            *get_file(void);

/**
 * Fonction that set the IPv4 address
 * inside an opt struct
 * 
 * @param ip_v4: struct get by converting an IP string
*/
void            set_v4(struct in_addr ip_v4);

/**
 * Fonction that get the IPv4 path
 * from an opt struct
 * 
 * @return: struct in_addr, return IPv4 Address inside a struct in_addr
*/
struct in_addr  get_v4(void);

/**
 * Fonction that set the IPv6 address
 * inside an opt struct
 * 
 * @param ip_v6: struct get by converting an IP string
*/
void            set_v6(struct in6_addr ip_v6);

/**
 * Fonction that get the IPv6 path
 * from an opt struct
 * 
 * @return: struct in_addr, return IPv6 Address inside a struct in_addr
*/
struct in6_addr get_v6(void);

/**
 * Fonction that set the UDP port
 * inside an opt struct
 * 
 * @param p_udp: the port number
*/
void            set_port_udp(char *p_udp);

/**
 * Fonction that get the UDP port
 * from an opt struct
 * 
 * @return uint16_t, the port number
*/
uint16_t        get_port_udp(void);

/**
 * Fonction that set the TCP port
 * inside an opt struct
 * 
 * @param p_tcp: the port number
*/
void            set_port_tcp(char *p_tcp);

/**
 * Fonction that get the TCP port
 * from an opt struct
 * 
 * @return uint16_t, the port number
*/
uint16_t        get_port_tcp(void);

#endif /* ! OPTIONS_H */
