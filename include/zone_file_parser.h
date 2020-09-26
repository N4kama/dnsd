#ifndef ZONE_FILE_PARSER_H
#define ZONE_FILE_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct {
    char        *name;
    uint16_t    type;
    uint32_t    ttl;
    char        *content;
} zone;

typedef struct {
    uint32_t    count;
    zone        *array;
} zone_array;

zone_array      *zone_parse(char *filename);
void            zone_free(zone_array *zones);
void            zone_print(zone_array *zones);

#endif

// Functions used to parse the zone file 
