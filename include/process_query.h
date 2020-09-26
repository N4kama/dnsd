#ifndef _PROCESS_QUERY_H_
#define _PROCESS_QUERY_H_

#include <stdint.h>

uint64_t process_request(char* raw, char* out_buf, zone_array *p_zones);

message build_answer(void);

#endif /* ! _PROCESS_QUERY_H_ */
