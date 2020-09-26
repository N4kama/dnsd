#include "response_handler.h"

message *response_handle(message **response, message *request, zone_array *zones)
{
    question question = request->question;
    zone *zone = response_lookup(question, zones);

    //TODO: create response message
    (*response)->header.id = request->header.id;
    (*response)->header.qr = 1;
    (*response)->header.opcode = 0;
    (*response)->header.aa = 1; //FIXME
    (*response)->header.tc = 0; //FIXME
    (*response)->header.rd = 0; //FIXME
    (*response)->header.ra = 0;
    (*response)->header.z = 0;
    (*response)->header.rcode = 0; //FIXME

    return NULL; //FIXME: return response message
}

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
