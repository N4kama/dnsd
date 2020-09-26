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
 * Don't forget to free !!!
 */
uint64_t process_request(char* raw, char** out_buf, zone_array *zones)
{
    // Raw to message struct
    message input_msg = parse_message(raw);

    // Process
    message *output_msg = malloc(1); 
    response_handle(&output_msg, input_msg, zones);

    // message struct to raw
    char* output_raw = malloc(1);
    uint64_t out_size = message_to_raw(*output_msg, &output_raw);

    free_message(input_msg);
    free_message(*output_msg);

    *out_buf = output_raw;

    return out_size;
}

void response_handle(message **response, message request, zone_array *zones)
{

    question question = *request.question;
    zone *zone = response_lookup(question, zones);
    (void) zone;

    //TODO: create response message
    (*response)->header.id = request.header.id;
    (*response)->header.qr = 1;
    (*response)->header.opcode = 0;
    (*response)->header.aa = 1; //FIXME
    (*response)->header.tc = 0; //FIXME
    (*response)->header.rd = 0; //FIXME
    (*response)->header.ra = 0;
    (*response)->header.z = 0;
    (*response)->header.rcode = 0; //FIXME

    return;
}

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
