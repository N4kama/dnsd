#include "server.h"

int start_server(zone_array *p_zones)
{
    int serverSockfd;       /* server udp/ipv4 socket */
    int optval;		        /* flag value for setsockopt */
             

    /* Creating ipv4 udp server */
    serverSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSockfd < 0)
		exitOnError("ERROR: failed at opening udp/ipv4 socket");

	/**
     * setsockopt: Prevents "ERROR on binding: address already in use" error
     * while reruning the server immediatly after killing it
	 * otherwise we have to wait about few seconds. 
	 */
    optval = 1;
	setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR,
		   (const void *)&optval, sizeof(int));

    //TODO: rework on server.c => use select or poll !

    printf("DEBUG: %d", p_zones->count);
    return 0;
}