#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "common.h"

//FIXME Main issue with this code right now is it assumes that all the
// strings in the buffer have valid strings ending with a '\0'

// As this is not supposed to be used except for some testing, this should not
// be declared in a header

resource_record parse_rr(char **buffer);

void display_header(header *h)
{
    printf("id: 0x%x\n", h->id);
    printf("qr %x\n", h->qr);
    printf("opcode %x\n", h->opcode);
    printf("aa %x\n", h->aa);
    printf("tc %x\n", h->tc);
    printf("rd %x\n", h->rd);
    printf("ra %x\n", h->ra);
    printf("z %x\n", h->z);
    printf("rcode %x\n", h->rcode);
    printf("qdcount: %d\n", h->qdcount);
    printf("ancount: %d\n", h->ancount);
    printf("nscount: %d\n", h->nscount);
    printf("arcount: %d\n", h->arcount);
}

// Displays a single question
void display_question(question *q)
{

    char *name = qname_to_string(q->qname);

    printf("qname: %s\n", q->qname);
    printf("printable name: %s\n", name);
    printf("qtype: %d\n", q->qtype);
    printf("qclass: %d\n", q->qclass);

    free(name);
}


// Parse a dns message (query or answer)
int parse_message(char *buffer, message *msg)
{
    question *q = NULL;
    resource_record *an = NULL;
    resource_record *ns = NULL;
    resource_record *ar = NULL;
    char *content;
    header *head = (header *)buffer;

    q = calloc(head->qdcount, sizeof(question));
    if (q == NULL)
        return ERR_NOMEM;

    content = (buffer + sizeof(header));

    // Clean this up
    for (int i = 0; i < head->qdcount; i++)
    {
        q[i].qname = calloc(strlen(content), sizeof(char));
        if (q[i].qname == NULL)
            return ERR_NOMEM;
        strncpy(q[i].qname, content, strlen(content));
        content += strlen(content) + 1;
        q[i].qtype = ntohs(*(uint16_t *)content);
        content += sizeof(uint16_t);
        q[i].qclass = ntohs(*(uint16_t *)content);
        content += sizeof(uint16_t);
    }


    an = calloc(head->ancount, sizeof(resource_record));
    if (an == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < head->ancount; i++)
    {
        an[i] = parse_rr(&content);
    }

    ns = calloc(head->nscount, sizeof(resource_record));
    if (ns == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < head->nscount; i++)
    {
        ns[i] = parse_rr(&content);
    }

    ar = calloc(head->arcount, sizeof(resource_record));
    if (ar == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < head->arcount; i++)
    {
        ar[i] = parse_rr(&content);
    }

    msg->header = *head;
    msg->header.id = ntohs(head->id);
    msg->header.bytes = ntohs(head->bytes);
    msg->header.qdcount = ntohs(head->qdcount);
    msg->header.ancount = ntohs(head->ancount);
    msg->header.nscount = ntohs(head->nscount);
    msg->header.arcount = ntohs(head->arcount);
    msg->question = q;
    msg->answer = an;
    msg->authority = ns;
    msg->additional = ar;
    return ERR_OK;
}

resource_record parse_rr(char **buffer)
{
    resource_record rr;
    char *name = NULL;
    char *data = NULL;

    name = calloc(strlen(*buffer), sizeof(char));

    strncpy(name, *buffer, strlen(*buffer));
    rr.name = name;
    *buffer += strlen(*buffer) + 1;

    rr.type = ntohs((uint16_t)**buffer);
    rr.clss = ntohs((uint16_t)**buffer);
    rr.ttl = ntohl((uint32_t)**buffer);
    rr.rdlength = ntohs((uint16_t)**buffer);

    data = calloc(strlen(*buffer), sizeof(char));
    strncpy(data, *buffer, strlen(*buffer));
    rr.rdata = data;

    return rr;
}


/**
 * Translate a qname (series of field length and fields) to a domain name.
 * @param qname: the qname to translate.
 * @return the translated name, free it after use.
 */
char *qname_to_string(char *qname)
{
    size_t label_length = 0;
    size_t total_length = 0;
    size_t nlabels = 0;
    char *base = qname;

    char *name = NULL;
    char *name_base = NULL;

    if (qname == NULL)
    {
        return NULL;
    }
    while (*qname != 0)
    {
        total_length += qname[0];
        nlabels++;
        qname += qname[0] + 1;
    }

    name = calloc(total_length + nlabels, 1);
    name_base = name;
    qname = base;
    while (*qname != 0)
    {
        label_length = qname[0];
        strncpy(name, qname + 1, label_length);
        name[label_length] = '.';
        name += label_length + 1;
        qname += label_length + 1;
    }
    name[0] = '\0';
    name = name_base;

    return name;
}


/**
 * Free multiple resource records rr
 * @param rr The resource records to free
 * @param nrecords, the number of records
 */
void free_rr(resource_record *rr, size_t nrecords)
{
    size_t i = 0;
    for (i = 0; i < nrecords; i++)
    {
        free(rr[i].name);
        free(rr[i].rdata);
    }
    free(rr);
}

/**
 * Free a message m
 * @param m The message to free
 */
void free_message(message m)
{
    size_t i;
    for (i = 0; i < m.header.qdcount; i++)
    {
        free(m.question[i].qname);
    }
    free(m.question);

    free_rr(m.answer, m.header.ancount);
    free_rr(m.authority, m.header.nscount);
    free_rr(m.additional, m.header.arcount);
    return;
}

/**
 * Free a message pointer m
 * @param m The message pointer to free
 */
void free_message_ptr(message *m)
{
    free_message(*m);
    free(m);
}

int copy_rr(char **out, resource_record rr)
{
    memcpy(*out, rr.name, strlen(rr.name));
    *out += strlen(rr.name) + 1;
    **out = htons(rr.type);
    *out += sizeof(uint16_t);
    **out = htons(rr.clss);
    *out += sizeof(uint16_t);
    **out = htonl(rr.ttl);
    *out += sizeof(uint32_t);
    **out = htons(rr.rdlength);
    *out += sizeof(uint16_t);
    memcpy(*out, rr.rdata, rr.rdlength);
    *out += rr.rdlength;

    return 0;
}

uint64_t rr_length(resource_record rr)
{
    uint64_t rr_len = 3 * sizeof(uint16_t) + sizeof(uint32_t) + rr.rdlength;
    rr_len += strlen(rr.name) + 1;
    return rr_len;
}

/**
 * Compute the total length of a message
 * Allows to get the size of the memory to alloc for the raw buffer
 * @param: m message structure to compute the size of
 * @return the size of the message
 */
uint64_t message_length(message m)
{
    size_t i;
    uint64_t len = sizeof(header);

    for (i = 0; i < m.header.qdcount; i++)
        len += 2 * sizeof(uint16_t) + strlen(m.question[i].qname) + 1;

    for (i = 0; i < m.header.ancount; i++)
        len += rr_length(m.answer[i]);
    for (i = 0; i < m.header.nscount; i++)
        len += rr_length(m.authority[i]);
    for (i = 0; i < m.header.arcount; i++)
        len += rr_length(m.additional[i]);
    return len;
}

/**
 * Inverse funciton of parse message
 * Don't forget to free me!
 * @param m The message to RAWify
 * @param out pointer to the RAW message
 * @return Size of the *out buffer
 */
uint64_t message_to_raw(message m, char **out)
{
    size_t i;
    size_t len = message_length(m);
    uint16_t qdc = m.header.qdcount;
    uint16_t anc = m.header.ancount;
    uint16_t nsc = m.header.nscount;
    uint16_t arc = m.header.arcount;

    *out = calloc(len, sizeof(char));
    char *base = *out;

    m.header.id = htons(m.header.id);
    m.header.bytes = htons(m.header.bytes);
    m.header.qdcount = htons(m.header.qdcount);
    m.header.ancount = htons(m.header.ancount);
    m.header.nscount = htons(m.header.nscount);
    m.header.arcount = htons(m.header.arcount);

    memcpy(*out, &(m.header), sizeof(header));
    *out += sizeof(header);

    for (i = 0; i < qdc; i++)
    {
        memcpy(*out, m.question[i].qname, strlen(m.question[i].qname));
        *out += strlen(m.question[i].qname) + 1;
        *(uint16_t*)*out = htons(m.question[i].qtype);;
        *out += sizeof(uint16_t);
        *(uint16_t*)*out = htons(m.question[i].qclass);
        *out += sizeof(uint16_t);
    }

    for (i = 0; i < anc; i++)
    {
        copy_rr(out, m.answer[i]);
    }

    for (i = 0; i < nsc; i++)
    {
        copy_rr(out, m.authority[i]);
    }

    for (i = 0; i < arc; i++)
    {
        copy_rr(out, m.additional[i]);
    }
    *out = base;

    return len;
}


/**
 * Takes a string and returns a qname string
 * @param s The string to transform
 * @return The requested qname string
 */
char *string_to_qname(char *s)
{
    char *qname = NULL;
    size_t i;
    size_t j;
    size_t total_length = 0;
    char label_length = 0;

    for (i = 0; s[i]; i++)
    {
        if (s[i] != '.')
        {
            label_length++;
        }
        else
        {
            qname = realloc(qname, sizeof(char) * label_length + 1);
            qname[i - label_length] = label_length;
            for (j = i - label_length; j < i; j++)
                qname[j + 1] = s[j];
            label_length = 0;
        }
        total_length++;
    }
    return qname;
}
