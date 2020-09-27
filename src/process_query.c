#include <stdint.h>

#include "common.h"
#include "process_query.h"
#include "zone_file_parser.h"

/**
 * Takes raw bytes in input and builds and return a raw answer
 * Don't forget to free output_raw!!!
 * @param raw The raw data to process
 * @param out_size The size of output data
 * @param p_zones The zone file to answer the client with
 * @return Output buffer
 */
char *process_request(char* raw, uint64_t *out_size, zone_array *zones)
{
    // Raw to message struct
    message *input_msg = calloc(1, sizeof(message));
    int ret = parse_message(raw, input_msg);
    if (ret != 0) // An error happened
        input_msg->header.rcode = RCODE_FORM_ERROR;

    // TODO: Check if there is at least 1 question

    // Process
    response_handle(input_msg, zones);

    // message struct to raw
    char* output_raw;
    *out_size = message_to_raw(*input_msg, &output_raw);

    free_message_ptr(input_msg);

    return output_raw;
}

void init_answer(message *m)
{
    m->header.qr = 1;
    m->header.aa = 1;
    m->header.rd = 0;
    m->header.ra = 0;
    m->header.z = 0;
    m->header.tc = 0;
    // qdcount stays the same
    m->header.ancount = 0;
    m->header.nscount = 0;
    m->header.arcount = 0;
}

/**
 * nom présent: if name and type are good, return zone
 * nom présent mais type absent: if name is good but not type, return SOA
 * nom non-existant: check if zone name is substring of name, return SOA
 * empty non terminal: return SOA ?
 * zone inconnue: return NULL
 */
void response_handle(message *m, zone_array *zones)
{
    init_answer(m);

    if (m->header.rcode == RCODE_FORM_ERROR)
        return;

    char *name = m->question->qname;
    uint16_t type = m->question->qtype;
    zone *best_soa = NULL;
    int16_t soa_score = 0;

    for (uint32_t i = 0; i < zones->count; ++i)
    {
        zone *z = &zones->array[i];
        int16_t cmp_score = qname_cmp(name, z->name);

        if (cmp_score < 0 && type == z->type)
        {
            m->header.ancount += 1;
            m->header.rcode = RCODE_NO_ERROR;
            m->answer = malloc(sizeof(resource_record));
            m->answer->name = m->question->qname;
            m->answer->type = m->question->qtype;
            m->answer->clss = m->question->qclass;
            m->answer->ttl = z->ttl;
            //m->answer->rdata = ; //FIXME
            //m->answer->rdlength = ; //FIXME
            return;
        }

        // check if SOA is better than previous one
        if (type == TYPE_SOA && cmp_score > soa_score)
        {
            best_soa = z;
            soa_score = cmp_score;
        }
    }

    if (best_soa != NULL)
    {
        m->header.nscount += 1;
        m->header.rcode = RCODE_NO_ERROR; // RCODE_NO_ERROR si nom présent mais type absent
                                          // RCODE_NXDOMAIN si nom non-existant
        m->authority = malloc(sizeof(resource_record));
        m->authority->name = m->question->qname;
        m->authority->type = m->question->qtype;
        m->authority->clss = m->question->qclass;
        m->authority->ttl = best_soa->ttl;
        uint16_t rsize;
        char *rdata = rdata_from_type(TYPE_SOA, best_soa, &rsize);
        m->authority->rdata = rdata;
        m->authority->rdlength = rsize;
    }
    
    // TODO: Check tc:indique si la réponse dépasse la taille maximum d’un
    // message UDP pour le client,
    // if (XXX)
    //  m->header.tc = 1;
}


/*
 * Retrun nb of matching domains
 */
int qname_cmp(char *qname, char *str2)
{
    char *str1 = qname_to_string(qname);
    uint64_t l1 = strlen(str1) - 1;
    uint64_t l2 = strlen(str2) - 1;

    uint64_t matching = 0;
    for (;l1 != 0 && l2 != 0;)
    {
        if (str1[l1] != str2[l2])
            break;
        if (str1[l1] == '.' && str2[l2] == '.')
            matching += 1;

        l1--;
        l2--;
    }

    free(str1);
    if (l1 == 0 && l2 == 0)
        return -1;

    return matching;
}

char *soa_parse(zone *soa, uint16_t *rsize)
{
    char mname[MAX_NAME_LENGTH + 1];
    char rname[MAX_NAME_LENGTH + 1];
    soa_rdata sr;

    sscanf(soa->content, "%253s %253s %u %d %d %d %u",
           mname,
           rname,
           &sr.serial,
           &sr.refresh,
           &sr.retry,
           &sr.expire,
           &sr.minimum);

    printf("%s %s %u %d %d %d %u",
           mname,
           rname,
           sr.serial,
           sr.refresh,
           sr.retry,
           sr.expire,
           sr.minimum);

    uint16_t mname_len = strlen(mname);
    uint16_t rname_len = strlen(rname);
    uint16_t sum_len = mname_len + 1 + rname_len + 1;
    *rsize = sum_len + sizeof(soa_rdata);

    char *rdata = malloc(*rsize);
    strcpy(rdata, mname);
    strcpy(rdata + mname_len + 1, rname);
    memcpy(rdata + sum_len, &sr, sizeof(soa_rdata));

    return rdata;
}

/**
 * Modify *rdata and *rsize depending on type and zone given
 * Do not forget to free return pointer
 */
char *rdata_from_type(int type, zone *z, uint16_t *rsize)
{
    char *rdata = NULL;
    switch(type)
    {
        case TYPE_A:
            break;
        case TYPE_AAAA:
            break;
        case TYPE_CNAME:
            break;
        case TYPE_MX:
            break;
        case TYPE_NS:
            break;
        case TYPE_SOA:
            return soa_parse(z, rsize);
        case TYPE_TXT:
            *rsize = strlen(z->content);
            rdata = malloc(*rsize);
            strncpy(rdata, z->content, *rsize);
            return rdata;
    };

    // Not handled
    return NULL;
}
