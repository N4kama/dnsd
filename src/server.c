#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <string.h>

#include "server.h"
#include "common.h"
#include "process_query.h"

/**
 * Create sockets for the server : Can be used to create
 * TCP/UDP sockets on IPv4/Ipv6
 * 
 * On UDP : same socket for every client
 * On TCP : create_socket forks and returns a new socket each time
 * 
 * @param sin_family: AF_INET or AF_INET6
 * @param type: SOCK_STREAM or SOCK_DGRAM 
 * @return: the socket file descriptor, -1 on error, -2 on server shutdown
*/
int create_socket(int sin_family, int type)
{
    int serverSockfd;               /* server socket */
    int clientSockfd;               /* client socket (TCP only) */
    int optval;		                /* flag value for setsockopt */
    struct sockaddr_in serveraddr;  /* server addr */
    struct sockaddr_in clientaddr;  /* client addr */
    socklen_t clientaddrlen;         /* client address len */

    /* Creating ipv4 udp server */
    serverSockfd = socket(sin_family, type, 0);
	if (serverSockfd < 0)
		exitOnError("ERROR: failed at opening socket");

	/**
     * setsockopt: Prevents "ERROR on binding: address already in use" error
     * while reruning the server immediatly after killing it
	 * otherwise we have to wait about few seconds. 
	 */
    optval = 1;
    setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR,
        (const void *)&optval, sizeof(int));
    
    /* Building the server address */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(53); //TODO: replace by variable

    /* Binding socket to serverAddress */
    if (bind(serverSockfd, &serveraddr, sizeof(serveraddr)) < 0)
        exitOnError("ERROR: failed to bind udp/ipv4 socket");

    if (type == SOCK_STREAM)
    {
        /* On TCP mode, fork and return every new client's socket */

        /* Placing new client connection in queue */
        if (listen(serverSockfd, TCP_MAX_CON) < 0)
            exitOnError("ERROR: tcp socket failed to listen");
        
        /* Accepting new connection, fork & return every new client socket */
        for(;;)
        {
            if ((clientSockfd = 
                accept(serverSockfd, &clientaddr, &clientaddrlen)) < 0)
                exitOnError("ERROR: tcp socket failed to accept");
            switch (fork())
            {
                case -1:
                    exitOnError("ERROR: failed to fork in create_socket");
                    return -1;
                case 0:
                    /* Child process - Client socket */
                    return clientSockfd;
                default:
                    /* Parent process */
                    continue;
            }
        }
        close(clientSockfd);
        return -1;
    }
    else
    {
        /* On UDP mode, simply return server socket */
        return serverSockfd;
    }
}


/**
 * Receive requests from client and respond
 * @param clientSockfd Client's socket to manage
 * @param is_stream Is the given socket a TCP socket ?
 */
void handler(zone_array *p_zones, int clientSockfd)
{
    (void) p_zones;

    char buf[1024];                 /* input buffer */
    int n;                          /* input byte size */
    struct sockaddr_in clientaddr;  /* client addr */
    socklen_t clientlen;		    /* byte size of clientaddr */

    /* request & response loop */
    clientlen = sizeof(clientaddr);
    while (1)
    {
        /* receiving data from from a client */
        n = recvfrom(clientSockfd, buf, 1024, 0, &clientaddr, &clientlen);
        if (n < 0)
            exitOnError("ERROR: recvfrom failed");
        buf[n] = '\0';
        printf(buf); //DEBUG

        //call handler
        //send(serverSockfd, process_request(buf, p_zones));
    }

    // CLOSE TCP SOCKET
    return;
}


/**
 * Start the DNSserver
 * 
 * @param p_zones the parsed zone file
 */
int start_server(zone_array *p_zones)
{
    (void) p_zones;

    int socketfd;       /* client socket returned by server */

    /* Handling incoming client connections */
    switch ((socketfd = create_socket(AF_INET, SOCK_STREAM)))
    {
        case -2:
            /* Server shut down */
            exitOnError("WARNING: server shutting down");
            return -2;
        case -1:
            /* Error with socket */
            break;
        default:
            /* Q/A with client */
            handler(p_zones, socketfd);
    }
    return 0;
}