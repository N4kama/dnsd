#include <stdint.h>

#include "common.h"
#include "process_query.h"
#include "zone_file_parser.h"

/**
 * Takes raw bytes in input and builds and return a raw answer
 * Don't forget to free !!!
 */
uint64_t process_request(char* raw, char** out_buf, zone_array *p_zones);
{
    (void) p_zones;

    // Raw to message struct
    message input_msg = parse_message(char *buffer);

    // Process
    // Do the magic
   

    // Build answer
    message output_msg = build_answer();

    // message struct to raw
    char* output_raw = message_to_raw(output_msg);

    free_message(input_msg);
    free_message(output_msg);

    return output_raw;
}

message build_answer(void)
{
    //FIXME
    message m;
    return m;
}
