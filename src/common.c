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

void display_rr(resource_record *rr)
{
    printf("name: %s\n", rr->name);
    printf("type: %u\n", rr->type);
    printf("clss: %u\n", rr->clss);
    printf("ttl: %u\n", rr->ttl);
    printf("rdlength: %u\n", rr->rdlength);
    printf("data: %s\n", rr->rdata);
}

void display_message(message *m)
{
    size_t i;

    puts("HEADER");
    display_header(&(m->header));
    puts("QUESTIONS");
    for (i = 0; i < m->header.qdcount; i++)
        display_question((m->question + i));
    puts("ANSWER RRs");
    for (i = 0; i < m->header.ancount; i++)
        display_rr((m->answer + i));
    puts("AUTHORITY RRs");
    for (i = 0; i < m->header.nscount; i++)
        display_rr((m->authority + i));
    puts("ADDITIONAL RRs");
    for (i = 0; i < m->header.arcount; i++)
        display_rr((m->additional + i));

}

// Parse a dns message (query or answer)
int parse_message(char *buffer, message *msg)
{
    size_t nlen;
    question *q = NULL;
    resource_record *an = NULL;
    resource_record *ns = NULL;
    resource_record *ar = NULL;
    char *content;
    header *head = (header *)buffer;

    msg->header = *head;
    msg->header.id = ntohs(head->id);
    msg->header.bytes = ntohs(head->bytes);
    msg->header.qdcount = ntohs(head->qdcount);
    msg->header.ancount = ntohs(head->ancount);
    msg->header.nscount = ntohs(head->nscount);
    msg->header.arcount = ntohs(head->arcount);

    q = calloc(msg->header.qdcount, sizeof(question));
    if (q == NULL)
        return ERR_NOMEM;

    content = (buffer + sizeof(header));

    // Clean this up
    for (int i = 0; i < msg->header.qdcount; i++)
    {
        nlen = name_length(content);
        q[i].qname = calloc(nlen, sizeof(char));
        if (q[i].qname == NULL)
            return ERR_NOMEM;
        memcpy(q[i].qname, content, nlen);
        content += nlen;
        q[i].qtype = ntohs(*(uint16_t *)content);
        content += sizeof(uint16_t);
        q[i].qclass = ntohs(*(uint16_t *)content);
        content += sizeof(uint16_t);
    }


    an = calloc(msg->header.ancount, sizeof(resource_record));
    if (an == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < msg->header.ancount; i++)
    {
        an[i] = parse_rr(&content);
    }

    ns = calloc(msg->header.nscount, sizeof(resource_record));
    if (ns == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < msg->header.nscount; i++)
    {
        ns[i] = parse_rr(&content);
    }

    ar = calloc(msg->header.arcount, sizeof(resource_record));
    if (ar == NULL)
        return ERR_NOMEM;

    for (int i = 0; i < msg->header.arcount; i++)
    {
        ar[i] = parse_rr(&content);
    }

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
    size_t nlen;

    nlen = read_name(*buffer, &name);
    rr.name = name;

    *buffer += nlen;

    rr.type = ntohs(*(uint16_t*)*buffer);
    *buffer += sizeof(uint16_t);
    rr.clss = ntohs(*(uint16_t*)*buffer);
    *buffer += sizeof(uint16_t);
    rr.ttl = ntohl(*(uint32_t*)*buffer);
    *buffer += sizeof(uint32_t);
    rr.rdlength = ntohs(*(uint16_t*)*buffer);
    *buffer += sizeof(uint16_t);

    data = calloc(rr.rdlength, sizeof(char));
    memcpy(data, *buffer, rr.rdlength);
    rr.rdata = data;
    *buffer += rr.rdlength;

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

    name = calloc(total_length + nlabels, sizeof(char));
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
    name[-1] = 0;
    name = name_base;
    qname = base;

    return name;
}

size_t name_length(char *name)
{
    size_t len = 0;
    size_t i;
    for (i = 0; name[i] != 0; i++)
    {
        if ((uint8_t)name[i] >= 0xc0)
        {
            len += 2;
            return len;
        }
        else
        {
            len++;
        }
    }
    len++;
    return len;
}

/**
 * In order to handle label compression, this function handles iterating
 * over a name and handling it properly
 * @param buffer: the raw data, pointing to the beginning of the name
 * @param name: pointer to a char* to get the value back
 * @return the size of the name
 */
size_t read_name(char *buffer, char **name)
{
    size_t len = name_length(buffer);
    *name = calloc(len, sizeof(char));
    if (*name == NULL)
        return 0;

    memcpy(*name, buffer, len);

    return len;
}

size_t write_name(char *name, char *buffer)
{
    size_t len = name_length(name);

    memcpy(buffer, name, len);

    return len;
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
    size_t nlen;
    nlen = write_name(rr.name, *out);
    if (nlen == 0)
        return ERR_PARSE_BADVAL;

    *out += nlen;
    *(uint16_t*)*out = htons(rr.type);
    *out += sizeof(uint16_t);
    *(uint16_t*)*out = htons(rr.clss);
    *out += sizeof(uint16_t);
    *(uint32_t*)*out = htonl(rr.ttl);
    *out += sizeof(uint32_t);
    *(uint16_t*)*out = htons(rr.rdlength);
    *out += sizeof(uint16_t);
    memcpy(*out, rr.rdata, rr.rdlength);
    *out += rr.rdlength;

    return 0;
}

uint64_t rr_length(resource_record rr)
{
    uint64_t rr_len = (3 * sizeof(uint16_t)) + sizeof(uint32_t) + rr.rdlength;
    rr_len += name_length(rr.name);
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
        len += (2 * sizeof(uint16_t)) + name_length(m.question[i].qname);

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
    size_t nlen;
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
        nlen = name_length(m.question[i].qname);
        memcpy(*out, m.question[i].qname, nlen);
        *out += nlen;
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
    //example.com

    char *qname = NULL;
    size_t i;
    size_t j;
    size_t nlabels = 0;
    size_t total_length = 0;
    char label_length = 0;

    for (i = 0; s[i] != 0; i++)
    {
        total_length++;
        if (s[i] == '.')
            nlabels++;
    }
    if (s[i - 1] != '.')
        nlabels++;

    qname = calloc(total_length + nlabels + 1, sizeof(char));

    for (i = 0; s[i]; i++)
    {
        if (s[i] != '.')
        {
            label_length++;
        }
        else
        {
            qname[i - label_length] = label_length;
            for (j = i - label_length; j < i; j++)
                qname[j + 1] = s[j];
            label_length = 0;
        }
    }
    qname[i - label_length] = label_length;
    for (j = i - label_length; j < i; j++)
        qname[j + 1] = s[j];
    qname[total_length + 1] = 0;
    return qname;
}
