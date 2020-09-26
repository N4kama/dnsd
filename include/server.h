#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "zone_file_parser.h" //TODO: Should the structs be moved in common.h ?

#define TCP_MAX_CON 10

int start_server(zone_array *p_zones);
void handler(zone_array *p_zones, int clientSockfd);
int create_socket(int sin_family, int type);

#endif

// Server handling DNS queries and responses
// Calls request_handler functions
// to parse the request then proceed to respond 
// with response_handler

