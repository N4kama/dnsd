#ifndef ZONE_FILE_PARSER_H
#define ZONE_FILE_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct {
    uint8_t     *name;
    uint16_t    type;
    uint32_t    ttl;
    uint8_t     *content;
} zone;

typedef struct {
    uint32_t    count;
    zone        *array;
} zone_array;

//typedef uint32_t parser_err;

zone_array      *zone_parse(uint8_t *filename);
void            zone_free(zone_array *zones);
void            zone_print(zone_array *zones);
//const uint8_t   *zone_strerror(parser_err);

#endif

// Functions used to parse the zone file 
