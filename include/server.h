#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "signal_handler.h"
#include "process_query.h"
#include "options.h"
#include "zone_file_parser.h" //TODO: Should the structs be moved in common.h ?

#define TCP_MAX_CON 10

dnsd_err bind_socket(int serverSockfd, int sin_family, int type);
dnsd_err accept_socket(int serverSockfd, int *clientSockfd, int sin_family);
dnsd_err start_server(zone_array *p_zones);
dnsd_err handler(zone_array *p_zones, int clientSockfd);
dnsd_err create_socket(int *clientSockfd, int sin_family, int type);

#endif

// Server handling DNS queries and responses
// Calls request_handler functions
// to parse the request then proceed to respond 
// with response_handler

