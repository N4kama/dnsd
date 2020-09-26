#include <stdio.h>

#include "error.h"
#include "zone_file_parser.h"
#include "server.h"
#include "option_parser.h"
#include "options.h"

int main(int argc, char **argv)
{
    dnsd_err code;

    // TODO: init opt struct even if no options : to default opts

    if (argc >= 2)
    {
        code = parse_options(argc, argv);
        if (code != ERR_OK)
            return code;
    }

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
