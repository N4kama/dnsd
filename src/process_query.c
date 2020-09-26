#include <stdint.h>

#include "common.h"
#include "process_query.h"
#include "zone_file_parser.h"

static
uint8_t is_name_good(char *name, char *zone_name)
{
    return strcmp(name, zone_name) == 0 ? 1 : 0;
}

static
uint8_t is_type_good(uint16_t type, uint16_t zone_type)
{
    return type == zone_type;
}

/**
 * Takes raw bytes in input and builds and return a raw answer
 * Don't forget to free output_raw!!!
 * @param raw The raw data to process
 * @param out_buf The processed output data
 * @param p_zones The zone file to answer the client with
 * @return Size of *out_buf
 */
char *process_request(char* raw, int *out_size, zone_array *zones)
{
    // Raw to message struct
    message *input_msg = calloc(1, sizeof(message));
    int ret = parse_message(raw, input_msg);
    if (ret != 0) // An error happened
        input_msg->header.rcode = 1

    // Process
    response_handle(input_msg, zones);

    // message struct to raw
    char* output_raw;
    uint64_t out_size = message_to_raw(*input_msg, &output_raw);

    free_message(*input_msg);
    free(input_msg);

    *out_size = out_size;
    return output_raw;
}

void response_handle(message *m, zone_array *zones)
{
    m->header.qr =        1;
    m->header.aa =        1;
    m->header.rd =        0;
    m->header.ra =        0;
    m->header.z =         0;
    if (m->header.rcode != 0)
        return;

    m->header.tc =        0; //FIXME

    zone *zone = response_lookup(m->question, zones);
}


/**
 * Builds a message struct out of the given parameters
 * @return A new response message
 */
zone *response_lookup(question question, zone_array *zones)
{
    uint8_t isName = 0;
    uint8_t isType = 0;

    for (uint32_t i = 0; i < zones->count; ++i)
    {
        // nom présent: if name and type are good, return zone
        // nom présent mais type absent: if name is good but not type, return SOA
        // nom non-existant: check if zone name is substring of name, return SOA
        // empty non terminal: return SOA ?
        // zone inconnue: return NULL
        isName = is_name_good(question.qname, zones->array[i].name);
        isType = is_type_good(question.qtype, zones->array[i].type);

        if (isName && isType)
            return &zones->array[i];
    }

    return NULL;
}
