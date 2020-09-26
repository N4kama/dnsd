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
    printf("id: 0x%x\n", ntohs(h->id));
    printf("qr %x\n", ntohs(h->qr));
    printf("opcode %x\n", ntohs(h->opcode));
    printf("aa %x\n", ntohs(h->aa));
    printf("tc %x\n", ntohs(h->tc));
    printf("rd %x\n", ntohs(h->rd));
    printf("ra %x\n", ntohs(h->ra));
    printf("z %x\n", ntohs(h->z));
    printf("rcode %x\n", ntohs(h->rcode));
    printf("qdcount: %d\n", ntohs(h->qdcount));
    printf("ancount: %d\n", ntohs(h->ancount));
    printf("nscount: %d\n", ntohs(h->nscount));
    printf("arcount: %d\n", ntohs(h->arcount));
}

// Displays a single question
void display_question(question *q)
{

    char *name = qname_to_string(q->qname);

    printf("qname: %s\n", q->qname);
    printf("printable name: %s\n", name);
    printf("qtype: %d\n", ntohs(q->qtype));
    printf("qclass: %d\n", ntohs(q->qclass));

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

    display_header(head);
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
        q[i].qtype = *(uint16_t *)content;
        content += sizeof(uint16_t) + 1;
        q[i].qclass = *(uint16_t *)content;
    }

    display_question(q);

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

    rr.type = (uint16_t)**buffer;
    rr.clss = (uint16_t)**buffer;
    rr.ttl = (uint32_t)**buffer;
    rr.rdlength = (uint16_t)**buffer;

    data = calloc(strlen(*buffer), sizeof(char));
    strncpy(data, *buffer, strlen(*buffer));
    rr.rdata = data;

    return rr;
}


// Translate a qname (series of label length, label)
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
 * Free a mesaage m
 * @param m The message to free
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

int copy_rr(char **out, resource_record rr)
{
    memcpy(*out, rr.name, strlen(rr.name));
    *out += strlen(rr.name) + 1;
    memcpy(*out, &rr.type, sizeof(uint16_t));
    *out += sizeof(uint16_t);
    memcpy(*out, &rr.clss, sizeof(uint16_t));
    *out += sizeof(uint16_t);
    memcpy(*out, &rr.ttl, sizeof(uint32_t));
    *out += sizeof(uint32_t);
    memcpy(*out, &rr.rdlength, sizeof(uint16_t));
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

    *out = calloc(len, sizeof(char));
    memcpy(*out, &(m.header), sizeof(header));

    *out += sizeof(header);

   for (i = 0; i < m.header.qdcount; i++)
   {
        memcpy(*out, m.question[i].qname, strlen(m.question[i].qname));
        *out += strlen(m.question[i].qname) + 1;
        memcpy(*out, &m.question[i].qtype, sizeof(uint16_t));
        *out += sizeof(uint16_t);
        memcpy(*out, &m.question[i].qclass, sizeof(uint16_t));
        *out += sizeof(uint16_t);
   }

   for (i = 0; i < m.header.ancount; i++)
   {
        copy_rr(out, m.answer[i]);
   }

   for (i = 0; i < m.header.nscount; i++)
   {
        copy_rr(out, m.authority[i]);
   }

   for (i = 0; i < m.header.arcount; i++)
   {
        copy_rr(out, m.additional[i]);
   }

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
