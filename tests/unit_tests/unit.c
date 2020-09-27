#include <stdio.h>
#include <stdlib.h>

#include "colors.h"
#include "common.h"
#include "error.h"
#include "option_parser.h"
#include "zone_file_parser.h"
#include "process_query.h"
#include "server.h"

void test_display_header(void);
void test_parse_header(void);
void test_parse_and_write(void);

void test_qname_cmp2(void);
void test_soa_parser(void);

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    printf("\n\n================ DNSD UNIT TESTS START ================\n\n");

    //test_display_header();
    test_parse_header();
    test_parse_and_write();

    test_qname_cmp2();
    test_soa_parser();

    printf("================  DNSD UNIT TEST END   ================\n\n\n");
}

void test_display_header(void)
{
    printf("-------- BEGIN test_display_header\n\n");
    header h;
    display_header(&h);
    printf("\n-------- END test_display_header\n\n");
}

void test_parse_header(void)
{
    printf("-------- BEGIN test_parse_header\n\n");

    printf("\t>> Query packet\n");
    char buf[32];
    FILE *packet = fopen("samples/dnsquery.raw", "r");
    fread(buf, sizeof(char), 32, packet);
    fclose(packet);
    message *m = malloc(sizeof(message));
    parse_message(buf, m);
    display_message(m);
    free_message_ptr(m);

    printf("\n\t>> Response packet\n");
    char buf2[125];
    packet = fopen("samples/dnsanswer.raw", "r");
    fread(buf2, sizeof(char), 125, packet);
    fclose(packet);
    m = malloc(sizeof(message));
    parse_message(buf2, m);
    display_message(m);
    free_message_ptr(m);

    printf("\n\t>> SOA response\n");
    char buf3[322];
    packet = fopen("samples/killme.raw", "r");
    fread(buf3, sizeof(char), 322, packet);
    fclose(packet);
    m = malloc(sizeof(message));
    parse_message(buf3, m);
    display_message(m);
    free_message_ptr(m);

    printf("\n-------- END test_parse_header\n\n");
}

void test_parse_and_write(void)
{
    printf("-------- BEGIN test_parse_and_write\n\n");

    printf("\t>> Query packet\n");
    FILE *packet = fopen("samples/dnsquery.raw", "r");
    char buf[32];
    fread(buf, sizeof(char), 32, packet);
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
        printf("result data did not match input data\n");
        for (size_t i = 0; i < 32; i++)
            printf("0x%x     0x%x\n", buf[i], result[i]);
        return;
    }
    free_message_ptr(m);
    free(result);

    printf("\n\t>> Response packet\n");
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
        return;
    }
    if (memcmp(buf2, result, 125))
    {
        printf("result data did not match input data\n");
        for (size_t i = 0; i < 125; i++)
            printf("%lu: 0x%x     0x%x\n", i, buf2[i], result[i]);
        return;
    }
    free_message_ptr(m);
    free(result);

    printf("\n\t>> SOA response\n");
    char buf3[322];
    packet = fopen("samples/killme.raw", "r");
    fread(buf3, sizeof(char), 322, packet);
    fclose(packet);
    m = malloc(sizeof(message));
    parse_message(buf3, m);
    rsize = message_to_raw(*m, &result);
    if (rsize != 322)
    {
        printf("rsize is %lu instead of 125\n", rsize);
        return;
    }
    if (memcmp(buf3, result, 322))
    {
        printf("result data did not match input data\n");
        for (size_t i = 0; i < 322; i++)
            printf("%lu: 0x%x     0x%x\n", i, buf3[i], result[i]);
        return;
    }
    free_message_ptr(m);
    free(result);

    printf("\n-------- END test_parse_and_write\n\n");
}

void test_qname_cmp2(void)
{
    printf("-------- BEGIN test_qname_cmp2\n\n");

    char *qname;
    char *str1;
    char *str2 = "example.com";

    printf("\t>>Case matches exactly... ");
    qname = string_to_qname("example.com");
    if (qname_cmp2(qname, str2) == NAME_EQUAL)
    printf("\t>>Case equal... ");
    qname = string_to_qname("example.com.");
    if (qname_cmp2(qname, str2) == -1)
        printf("OK\n");
    else
    {
        str1 = qname_to_string(qname);
        printf("FAIL\n");
        printf("\t\tqname: %s\n", qname);
        printf("\t\tqname_to_string: %s\n", str1);
        printf("\t\tstr2: %s\n", str2);
        printf("\t\tqname_cmp = %d\n", qname_cmp2(qname, str2));
        free(str1);
    }
    free(qname);

    printf("\t>>Case matching 2... ");
    qname = string_to_qname("nx.example.com");
    printf("\t>>Case matching == 2... ");
    qname = string_to_qname("nx.example.com.");
    if (qname_cmp2(qname, str2) == 2)
         printf("OK\n");
    else
    {
        str1 = qname_to_string(qname);
        printf("FAIL\n");
        printf("\t\tqname: %s\n", qname);
        printf("\t\tqname_to_string: %s\n", str1);
        printf("\t\tstr2: %s\n", str2);
        printf("\t\tqname_cmp = %d\n", qname_cmp2(qname, str2));
        free(str1);
    }
    free(qname);

    printf("\t>>Case matching 1... ");
    qname = string_to_qname("not-example.com");
    if (qname_cmp2(qname, str2) == 1)
          printf("OK\n");
    else
    {
        str1 = qname_to_string(qname);
        printf("FAIL\n");
        printf("\t\tqname: %s\n", qname);
        printf("\t\tqname_to_string: %s\n", str1);
        printf("\t\tstr2: %s\n", str2);
        printf("\t\tqname_cmp = %d\n", qname_cmp2(qname, str2));
        free(str1);
    }
    free(qname);

    printf("\t>>Case no match... ");
    qname = string_to_qname("not-examplecom");
    printf("\t>>Case no match... ");
    qname = string_to_qname("not-example.com.");
    if (qname_cmp2(qname, str2) == 0)
          printf("OK\n");
    else
    {
        str1 = qname_to_string(qname);
        printf("FAIL\n");
        printf("\t\tqname: %s\n", qname);
        printf("\t\tqname_to_string: %s\n", str1);
        printf("\t\tstr2: %s\n", str2);
        printf("\t\tqname_cmp = %d\n", qname_cmp2(qname, str2));
        free(str1);
    }
    free(qname);

    printf("\n-------- END test_qname_cmp2\n\n");
}

void test_soa_parser(void)
{
    printf("-------- BEGIN test_soa_parser\n\n");

    uint16_t rsize;
    zone z = {
        .name       = "example.com",
        .type       = TYPE_SOA,
        .ttl        = 3600,
        .content    = "ns0.example.com. postmaster.example.com. 2020092501 86400 3600 604800 3600"
    };

    char *rdata = soa_parse(&z, &rsize);
    char *rdata_base = rdata;

    printf("rsize: %u\n", rsize);
    printf("mname: '%s'\n", rdata);
    rdata += strlen(rdata) + 1;
    printf("rname: '%s'\n", rdata);
    rdata += strlen(rdata) + 1;
    soa_rdata *sr = (soa_rdata *)rdata;
    printf("serial: %u\n",  sr->serial);
    printf("refresh: %d\n", sr->refresh);
    printf("retry: %d\n",   sr->retry);
    printf("expire: %d\n",  sr->expire);
    printf("minimum: %u\n", sr->minimum);

    free(rdata_base);
    printf("\n-------- END test_soa_parser\n\n");
}
