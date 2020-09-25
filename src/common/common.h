#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define __packed __attribute__((__packed__))

// Macros for QTYPE
#define TYPE_A      1
#define TYPE_NS     2
#define TYPE_CNAME  5
#define TYPE_SOA    6
#define TYPE_MX     15
#define TYPE_AAAA   28

struct header
{
    uint16_t    id;
    uint8_t     qr :1;
    uint8_t     opcode :4;
    uint8_t     aa :1;
    uint8_t     tc :1;
    uint8_t     rd :1;
    uint8_t     ra :1;
    uint8_t     z :3;
    uint8_t     rcode :4;
    uint16_t    qdcount;
    uint16_t    ancount;
    uint16_t    nscount;
    uint16_t    arcount;
}__packed;

struct question
{
    char        *qname;
    uint16_t    qtype;
    uint16_t    qclass;
}__packed;

struct rr
{
    char        *name;
    uint16_t    type;
    uint16_t    clss;
    uint32_t    ttl;
    uint16_t    rdlength;

    /*
     * Since the format of this changes depending on the TYPE, we might
     *  want to define a struct for rdata
     */
    char        *rdata;
}__packed;

struct message
{
    struct      header header;
    struct      question question;
    struct      rr answer;
    struct      rr authority;
    struct      rr additional;
}__packed;

#endif

// Structures shared by mutliple modules
