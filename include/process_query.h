#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

#include "common.h"
#include "zone_file_parser.h"

typedef struct __packed
{
    uint32_t serial;
    int32_t refresh;
    int32_t retry;
    int32_t expire;
    uint32_t minimum;
} soa_rdata;

char *process_request(char* raw, uint64_t *out_size, zone_array *zones);

void response_handle(message *m, zone_array *zones);

int qname_cmp(char *qname, char *str2);

char *soa_parse(zone *soa, uint16_t *rsize);

char *rdata_from_type(int type, zone *z, uint16_t *rsize);

#endif /* ! _PROCESS_QUERY_H_ */
