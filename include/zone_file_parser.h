#ifndef ZONE_FILE_PARSER_H
#define ZONE_FILE_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "error.h"

#define MAX_NAME_LENGTH     253
#define MAX_LABEL_LENGTH    63

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

dnsd_err    zone_parse(char *filename, zone_array **zones);
void        zone_free(zone_array *zones);
void        zone_print(zone_array *zones);

#endif

// Functions used to parse the zone file 
