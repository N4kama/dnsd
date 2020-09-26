#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define __packed __attribute__((__packed__))

// Macros for QTYPE
#define TYPE_A      1
#define TYPE_AAAA   28
#define TYPE_CNAME  5
#define TYPE_MX     15
#define TYPE_NS     2
#define TYPE_SOA    6
#define TYPE_TXT    16


// RCODE
#define RCODE_NO_ERROR      0
#define RCODE_FORM_ERROR    1
#define RCODE_SERVER_FAIL   2
#define RCODE_NXDOMAIN      3
#define RCODE_NOTIMP        4
#define RCODE_REFUSED       5

typedef struct __packed
{
    uint16_t    id;
    union
    {
        uint16_t bytes;
        struct
        {
            uint8_t     qr      :1;
            uint8_t     opcode  :4;
            uint8_t     aa      :1;
            uint8_t     tc      :1;
            uint8_t     rd      :1;
            uint8_t     ra      :1;
            uint8_t     z       :3;
            uint8_t     rcode   :4;
        };
    };
    uint16_t    qdcount;
    uint16_t    ancount;
    uint16_t    nscount;
    uint16_t    arcount;
} header;

typedef struct __packed
{
    char        *qname;
    uint16_t    qtype;
    uint16_t    qclass;
} question;

typedef struct __packed
{
    char        *name;
    uint16_t    type;
    uint16_t    clss;
    uint32_t    ttl;
    uint16_t    rdlength;
    char        *rdata;
} resource_record;

typedef struct __packed
{
    header          header;
    question        *question;
    resource_record *answer;
    resource_record *authority;
    resource_record *additional;
} message;

int parse_message(char *buffer, message *msg);
uint64_t message_to_raw(message m, char **out);
void free_message(message m);
void free_message_ptr(message *m);
char *qname_to_string(char *qname);
char *string_to_qname(char *s);
void display_header(header *h);
void display_question(question *q);

#endif

// Structures shared by mutliple modules
