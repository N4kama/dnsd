#include <stdio.h>

#include "error.h"
#include "option_parser.h"
#include "process_query.h"
#include "server.h"
#include "zone_file_parser.h"
#include "common.h"
#include "colors.h"

void test_display_header(void);

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    printf("DNSD TEST SUITE START\n");

    test_display_header();

}

void test_display_header(void)
{
    header h;
    display_header(&h);
}
