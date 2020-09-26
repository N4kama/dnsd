#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "zone_file_parser.h" //TODO: Should the structs be moved in common.h ?

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int start_server(zone_array *p_zones);

#endif

// Server handling DNS queries and responses
// Calls request_handler functions
// to parse the request then proceed to respond 
// with response_handler