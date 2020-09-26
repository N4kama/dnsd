#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

#include "common.h"
#include "zone_file_parser.h"

char *process_request(char* raw, uint64_t *out_size, zone_array *zones);

void response_handle(message *m, zone_array *zones);

zone *response_lookup(question *question, zone_array *zones);

#endif /* ! _PROCESS_QUERY_H_ */
