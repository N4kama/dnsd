#include "zone_file_parser.h"

// strchr() that support escaped searched char
char *strechr(char *s, char c)
{
    for (uint64_t i = 0; s[i]; i++)
        if (s[i] == c && (i == 0 || s[i - 1] != '\\'))
            return s + i;

    return NULL;
}

// count occurence of char c in s
uint32_t strcount(char *s, char c)
{
    uint32_t count = 0;
    for (uint64_t i = 0; s[i]; i++)
        if (s[i] == c && (i == 0 || s[i - 1] != '\\'))
            count++;
    return count;
}

uint16_t str2type(char *str)
{
    if (strcmp(str, "A") == 0)
        return TYPE_A;
    if (strcmp(str, "AAAA") == 0)
        return TYPE_AAAA;
    if (strcmp(str, "NS") == 0)
        return TYPE_NS;
    if (strcmp(str, "SOA") == 0)
        return TYPE_SOA;
    if (strcmp(str, "TXT") == 0)
        return TYPE_TXT;
    return 0;
}

dnsd_err zone_parse(char *filename, zone_array **zones)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return ERR_NOENT;
    
    *zones = malloc(sizeof(zone_array));
    if (*zones == NULL)
    {
        fclose(file);
        return ERR_NOMEM;
    }
    (*zones)->count = 0;
    (*zones)->array = NULL;

    fseek(file, 0, SEEK_END);
    uint64_t file_size = ftell(file);
    rewind(file);
    if (file_size == 0)
    {
        fclose(file);
        return ERR_OK;
    }

    char *file_content = malloc(file_size + 1);
    if (file_content == NULL)
    {
        fclose(file);
        return ERR_NOMEM;
    }

    uint64_t count = fread(file_content, sizeof(char), file_size, file);
    file_content[count] = 0;

    uint32_t line_count = strcount(file_content, '\n');
    (*zones)->array = malloc(line_count * sizeof(zone));
    if ((*zones)->array == NULL)
    {
        zone_free(*zones);
        free(file_content);
        fclose(file);
        return ERR_NOMEM;
    }

    char *cursor = file_content;
    for (uint32_t i = 0; i < line_count; i++)
    {
        char *endl = strechr(cursor, '\n');
        *endl = 0;

        // note: escaped char in other than the 4th value breaks everything

        if (strcount(cursor, ';') != 3)
        {
            zone_free(*zones);
            free(file_content);
            fclose(file);
            return ERR_PARSE_BADCNT;
        }

        // parse name
        char *value_end = strechr(cursor, ';');
        *value_end = 0;
        (*zones)->array[i].name = malloc(value_end - cursor + 1);
        if ((*zones)->array[i].name == NULL)
        {
            zone_free(*zones);
            free(file_content);
            fclose(file);
            return ERR_NOMEM;
        }
        strcpy((*zones)->array[i].name, cursor);
        cursor = value_end + 1;

        // parse type
        value_end = strechr(cursor, ';');
        *value_end = 0;
        (*zones)->array[i].type = str2type(cursor);
        cursor = value_end + 1;

        // parse TTL
        value_end = strechr(cursor, ';');
        *value_end = 0;
        (*zones)->array[i].ttl = atoi(cursor);
        cursor = value_end + 1;

        // parse content
        (*zones)->array[i].content = malloc(endl - cursor + 1);
        if ((*zones)->array[i].content == NULL)
        {
            free((*zones)->array[i].name);
            zone_free(*zones);
            free(file_content);
            fclose(file);
            return ERR_NOMEM;
        }
        strcpy((*zones)->array[i].content, cursor);
        cursor = endl + 1;

        (*zones)->count++;
    }

    free(file_content);
    fclose(file);

    return ERR_OK;
}

void zone_free(zone_array *zones)
{
    for (uint32_t i = 0; i < zones->count; i++)
    {
        free(zones->array[i].name);
        free(zones->array[i].content);
    }

    free(zones);
}

void zone_print(zone_array *zones)
{
    if (zones != NULL)
    {
        for (uint32_t i = 0; i < zones->count; i++)
            printf("%02u: name=%-32s\ttype=%04u\tTTL=%08u\tcontent=%-64s\n",
                   i,
                   zones->array[i].name,
                   zones->array[i].type,
                   zones->array[i].ttl,
                   zones->array[i].content);
    }
}
