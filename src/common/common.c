#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

// As this is not supposed to be used except for some testing, this should not
// be declared in a header
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

void display_question(question *q)
{
    printf("qname: %p\n", q->qname);
    printf("%d\n", ntohs(q->qtype));
    printf("%d\n", ntohs(q->qclass));
}


// Parse a dns message (query or answer)
message parse_message(char *buffer)
{
    message msg;
    question *q;
    header *head = (header *)buffer;

    display_header(head);

    //TODO clean this up
    q = (question *)(buffer + sizeof(header));

    display_question(q);

    msg.header = *head;
    msg.question = q;
    return msg;
}


// Translate a qname (series of label length, label)
char *qname_to_string(char *qname)
{
    int label_length = 0;
    int total_length = 0;
    int nlabels = 0;
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
