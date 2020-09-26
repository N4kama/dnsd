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
    char buf[32];
    fread(buf,sizeof(char), 32, packet);
    fclose(packet);
    message *m = malloc(sizeof(message));
    parse_message(buf, m);
    display_header(&(m->header));
    display_question(m->question);
    free_message_ptr(m);
}

void test_parse_and_write(void)
{
    puts("\n--- BEGIN test_parse_and_write---\n");
    puts("\n -> Query packet\n");
    FILE *packet = fopen("samples/dnsquery.raw", "r");
    char buf[32];
    fread(buf,sizeof(char), 32, packet);
    fclose(packet);
    message *m = malloc(sizeof(message));
    parse_message(buf, m);
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
    free_message_ptr(m);
    free(result);
    puts("\n -> Response packet\n");
    packet = fopen("samples/dnsanswer.raw", "r");
    char buf2[125];
    fread(buf2, sizeof(char), 125, packet);
    fclose(packet);
    m = malloc(sizeof(message));
    parse_message(buf2, m);
    rsize = message_to_raw(*m, &result);
    if (rsize != 125)
    {
        printf("rsize is %lu instead of 125\n", rsize);
        display_header(&(m->header));
        display_question(m->question);
        return;
    }
    if (memcmp(buf2, result, 125))
    {
        puts("result data did not match input data");
        for (size_t i = 0; i < 125; i++)
        {
                printf("%lu: 0x%x     0x%x\n", i, buf2[i], result[i]);
        }
        return;
    }

    puts("test_parse_and_write OK");
}
