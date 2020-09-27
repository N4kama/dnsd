#include <stdint.h>
#include <arpa/inet.h>

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
 *
 *
 *
 */
int qname_cmp(char *str1, char *str2)
{
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

    if (l1 == 0 && l2 == 0)
        return NAME_EQUAL;

    return matching;
}

int count_dom(char *str)
{
    if (str[0] == 0)
        return 0;
    int count = 0;
    for (;*str; str++)
        if (*str == '.')
            count++;
    if (*(str - 1) == '.')
        count--;

    return count;
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
    char *name = m->question->qname;
    uint16_t type = m->question->qtype;
    zone *soa = NULL;
    zone *best = NULL;
    int best_score = 0;
    int found = 0;

    for (uint32_t i = 0; i < zones->count; ++i)
    {
        zone *z = &zones->array[i];
        int16_t cmp_score = qname_cmp(name, z->name);

        if (cmp_score == NAME_EQUAL && z->type == type)
        {
            found = 1;
            break;
        }

        if (cmp_score > best_score)
        {
            best_score = cmp_score;
            best = z;
        }

        if (type == TYPE_SOA) // There MUST be one
            soa = z;
    }

    int soa_len = count_dom(soa->name);

    if (found)
    {
        //CASE: Name exists                     example.com
        build_ans(m, best, RR_ANSWER, RCODE_NO_ERROR);
    }
    else if (best_score == NAME_EQUAL)
    {
        //CASE: Name exists but type is absent  example.com
        build_ans(m, soa, RR_AUTHORITY, RCODE_NO_ERROR);
    }
    else if (best_score < soa_len)
    {
        //CASE: Unknown zone,               not-example.com
       m->header.rcode = RCODE_REFUSED;
    }
    else if (best_score == soa_len)
    {
        //CASE: Name does not exist          nx.example.com
        build_ans(m, soa, RR_AUTHORITY, RCODE_NXDOMAIN);
    }
    else if (best_score > soa_len)
    {
        //CASE: Empty non-terminal          sub.example.com
        build_ans(m, soa, RR_AUTHORITY, RCODE_NO_ERROR);
    }

    // TODO: Check tc:indique si la réponse dépasse la taille maximum d’un
    // message UDP pour le client,
    // if (XXX)
    //  m->header.tc = 1;

    // TODO: Do CHAOS
}


void build_ans(message *m, zone *z, int rr_type, int rcode)
{

    resource_record *ans = malloc(sizeof(resource_record));
    ans->name = m->question->qname;
    ans->type = m->question->qtype;
    ans->clss = m->question->qclass;
    ans->ttl = z->ttl;

    uint16_t rsize;
    char *rdata = rdata_from_type(z->type, z, &rsize);
    ans->rdata = rdata;
    ans->rdlength = rsize;

    switch(rr_type)
    {
        case RR_ANSWER:
            m->answer = ans;
            m->header.ancount += 1;
            m->header.rcode = rcode;
            break;
        case RR_AUTHORITY:
            m->authority = ans;
            m->header.nscount += 1;
            break;
        case RR_ADDITIONAL:
            m->additional = ans;
            m->header.arcount += 1;
            break;
    };
}

/**
 * Parse SOA RDATA
 */
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
 * Parse A RDATA
 */
char *a_parse(zone *a, uint16_t *rsize)
{
    *rsize = sizeof(uint32_t);
    char *rdata = malloc(*rsize);
    inet_pton(AF_INET, a->content, rdata);

    return rdata;
}

/**
 * Parse AAAA RDATA
 */
char *aaaa_parse(zone *aaaa, uint16_t *rsize)
{
    *rsize = 16 * sizeof(uint8_t);
    char *rdata = malloc(*rsize);
    inet_pton(AF_INET6, aaaa->content, rdata);

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
            return a_parse(z, rsize);
        case TYPE_AAAA:
            return aaaa_parse(z, rsize);
        case TYPE_SOA:
            return soa_parse(z, rsize);
        case TYPE_CNAME:
        case TYPE_TXT:
            *rsize = strlen(z->content);
            rdata = malloc(*rsize);
            strncpy(rdata, z->content, *rsize);
            return rdata;
    };

    // Not handled
    return NULL;
}

