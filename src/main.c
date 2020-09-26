#include <stdio.h>

#include "error.h"
#include "zone_file_parser.h"

int main(void) // int argc, char *argv[]) 
{
    dnsd_err code;

    zone_array *zones;
    code = zone_parse("samples/dnsd.zone", &zones);
    if (code != ERR_OK)
    {
        printf("zone_parse failed with: %s\n", dnsd_strerror(code));
        return code;
    }

    zone_print(zones);

    printf("Starting DNS server\n");

    zone_free(zones);

    return code;
}
