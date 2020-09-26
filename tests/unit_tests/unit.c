#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "option_parser.h"
#include "process_query.h"
#include "server.h"
#include "zone_file_parser.h"
#include "common.h"
#include "colors.h"

void test_display_header(void);
void test_parse_header(void);

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    printf("DNSD TEST SUITE START\n");

    test_display_header();
    test_parse_header();

}

void test_display_header(void)
{
    header h;
    display_header(&h);
}

void test_parse_header(void)
{
    FILE *packet = fopen("samples/dnsquery.raw", "r");
    char *buf = calloc(32, sizeof(char));
    fread(buf,sizeof(char), 32, packet);
    message *m = malloc(sizeof(message));
    parse_message(buf, m);
    display_header(&(m->header));
    display_question(m->question);
}
