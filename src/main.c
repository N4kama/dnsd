#include <stdio.h>

#include "zone_file_parser.h"

int main(void) // int argc, char *argv[]) 
{
    zone_array *zones;

    zones = zone_parse("samples/dnsd.zonefile");
    zone_print(zones);

    printf("Starting DNS server\n");

    zone_free(zones);

    return 0;
}
