#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

#include "common.h"
#include "zone_file_parser.h"

uint64_t process_request(char* raw, char** out_buf, zone_array *zones);

void response_handle(message **response, message request, zone_array *zones);

zone *response_lookup(question question, zone_array *zones);

#endif /* ! _PROCESS_QUERY_H_ */
