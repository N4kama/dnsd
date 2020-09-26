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
void test_parse_and_write(void);

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    printf("DNSD TEST SUITE START\n");

    test_display_header();
    test_parse_header();
    test_parse_and_write();

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

void test_parse_and_write(void)
{
    puts("\n ---BEGIN test_parse_and_write--- \n");
    FILE *packet = fopen("samples/dnsquery.raw", "r");
    char *buf = calloc(32, sizeof(char));
    fread(buf,sizeof(char), 32, packet);
    message *m = malloc(sizeof(message));
    parse_message(buf, m);
    display_header(&(m->header));
    display_question(m->question);

    char *result;
    size_t rsize = message_to_raw(*m, &result);
    if (rsize != 32)
    {
        printf("rsize is %lu instead of 32\n", rsize);
        return;
    }
    if (memcmp(buf, result, 32))
    {
        puts("result data did not match input data");
        for (size_t i = 0; i < 32; i++)
        {
            printf("0x%x     0x%x\n", buf[i], result[i]);
        }
        return;
    }
    puts("test_parse_and_write OK");
}
