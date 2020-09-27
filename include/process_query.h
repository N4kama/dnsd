#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

#include "common.h"
#include "zone_file_parser.h"

#define RR_ANSWER       1
#define RR_AUTHORITY    2
#define RR_ADDITIONAL   3

#define NAME_EQUAL 128

typedef struct __packed
{
    uint32_t serial;
    int32_t refresh;
    int32_t retry;
    int32_t expire;
    uint32_t minimum;
} soa_rdata;

char *process_request(char* raw, uint64_t *out_size, zone_array *zones);

void init_answer(message *m);

char *soa_parse(zone *soa, uint16_t *rsize);

char *rdata_from_type(int type, zone *z, uint16_t *rsize);

int qname_cmp(char *str1, char *str2);

int count_dom(char *str);

void response_handle(message *m, zone_array *zones);

void build_ans(message *m, zone *z, int rr_type, int rcode);

#endif /* ! _PROCESS_QUERY_H_ */
