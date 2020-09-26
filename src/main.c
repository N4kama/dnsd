#include <stdio.h>

#include "error.h"
#include "zone_file_parser.h"
#include "server.h"

int main(void) // int argc, char *argv[]) 
{
    dnsd_err code;

    zone_array *zones;
    code = zone_parse("samples/dnsd.zone", &zones);
    if (code != ERR_OK)
    {
        printf("zone file parsing: failed with: %s\n", dnsd_strerror(code));
        return code;
    }

    zone_print(zones);

    printf("-------------------\nStarting DNS server\n-------------------\n");
    start_server(zones);

    zone_free(zones);

    return code;
}
