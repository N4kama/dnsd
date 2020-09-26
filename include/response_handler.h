#ifndef REPONSE_HANDLER_H
#define REPONSE_HANDLER_H

#include "common.h"
#include "zone_file_parser.h"

message *response_handle(message **response, message *request, zone_array *zones);
zone *response_lookup(question question, zone_array *zones);

#endif

// Functions used to respond to a request
