#include <stdio.h>

#include "error.h"
#include "zone_file_parser.h"
#include "server.h"
#include "option_parser.h"
#include "options.h"

int main(int argc, char **argv)
{
    dnsd_err code;

    if (argc >= 2)
    {
        code = parse_options(argc, argv);
        if (code != ERR_OK)
            return code;
    }

    printf("-------------------\nParsing zone file\n-------------------\n");
    zone_array *zones;
    code = zone_parse(get_file(), &zones);
    if (code != ERR_OK)
    {
        fprintf(stderr, "zone file parsing failed with: %s\n", dnsd_strerror(code));
        return code;
    }

    zone_print(zones);

    printf("-------------------\nStarting DNS server\n-------------------\n");
    code = start_server(zones);
    if (code != ERR_OK)
    {
        fprintf(stderr, "start server failed with: %s\n", dnsd_strerror(code));
        return code;
    }

    zone_free(zones);

    return code;
}
