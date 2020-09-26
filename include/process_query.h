#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

#include "common.h"
#include "zone_file_parser.h"

char *process_request(char* raw, uint64_t *out_size, zone_array *zones);

void response_handle(message *m, zone_array *zones);

int qname_cmp(char *qname, char *str2);

char *rdata_from_type(int type, zone *z, uint16_t *rsize);

#endif /* ! _PROCESS_QUERY_H_ */
