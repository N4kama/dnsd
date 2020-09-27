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

dnsd_err handle_communication(zone_array *p_zones, dns_sock clientSock)
{
    (void)p_zones;
    struct sockaddr_in6 client;
    socklen_t len = sizeof(client);
    char request[512];       /* input buffer */
    char *response;          /* request buffer after parsing */
    uint64_t response_size;  /* request buffer length after parsing */

    if (clientSock.type == SOCK_DGRAM)
    {
        if (recvfrom(clientSock.socketfd, request, sizeof(request), 0, &client, &len) < 0)
        {
            return ERR_SOCK_RECV;
        }
    }
    else
    {
        if (recv(clientSock.socketfd, request, sizeof(request), 0) < 0)
        {
            return ERR_SOCK_RECV;
        }
    }

    response = process_request(request, &response_size, p_zones);

    if (clientSock.type == SOCK_DGRAM)
    {
        if (sendto(clientSock.socketfd, response, response_size, 0, &client, len) < 0)
        {
            return ERR_SOCK_RECV;
        }
    }
    else
    {
        if (send(clientSock.socketfd, response, response_size, 0) < 0)
        {
            return ERR_SOCK_RECV;
        }
    }

    free(response);

    return ERR_OK;
}

/**
 * Binds the server address to the socket in IPv4 or IPv6
 * 
 * @param serverSockfd: server socket file descriptor
 * @param sin_family: AF_INET or AF_INET6
 * @param type: SOCK_STREAM or SOCK_DGRAM
 * @return: the dnsd_err code
*/
dnsd_err bind_socket(int serverSockfd, int sin_family, int type)
{
    uint16_t port;                          /* dns port number */

    if (sin_family == AF_INET)
    {
        struct sockaddr_in  serveraddr;     /* server addr in IPv4 */

        /* Building the server address */
        bzero((char *)&serveraddr, sizeof(serveraddr));
        serveraddr.sin_family = sin_family;
        serveraddr.sin_addr = get_v4();
        port = get_port(type);
        serveraddr.sin_port = htons(port);

        /* Binding socket to serverAddress */
        if (bind(serverSockfd, &serveraddr, sizeof(serveraddr)) < 0)
        {
            close(serverSockfd);
            return ERR_SOCK_BIND;
        }
    }
    else
    {
        struct sockaddr_in6 serveraddr6;    /* server addr in IPv6 */

        /* Building the server address */
        bzero((char *)&serveraddr6, sizeof(serveraddr6));
        serveraddr6.sin6_family = sin_family;
        serveraddr6.sin6_addr = get_v6();
        port = get_port(type);
        serveraddr6.sin6_port = htons(port);

        /* Binding socket to serverAddress */
        if (bind(serverSockfd, &serveraddr6, sizeof(serveraddr6)) < 0)
        {
            close(serverSockfd);
            return ERR_SOCK_BIND;
        }
    }
    return ERR_OK;
}

/**
 * Accepts new sockets in IPv4 or IPv6 (TCP only)
 * 
 * @param serverSockfd: server socket file descriptor
 * @param clientSockfd: client socket file descriptor
 * @param sin_family: AF_INET or AF_INET6
 * @return: the dnsd_err code
*/
dnsd_err accept_socket(int serverSockfd, int *clientSockfd, int sin_family)
{
    socklen_t clientaddrlen;            /* client address len */

    if (sin_family == AF_INET)
    {
        struct sockaddr_in clientaddr4;  /* client addr */

        if ((*clientSockfd = 
        accept(serverSockfd, &clientaddr4, &clientaddrlen)) < 0)
        {
            close(serverSockfd);
            return ERR_SOCK_ACCEPT;
        }
    }
    else
    {
        struct sockaddr_in6 clientaddr6;  /* client addr */

        if ((*clientSockfd = 
        accept(serverSockfd, &clientaddr6, &clientaddrlen)) < 0)
        {
            close(serverSockfd);
            return ERR_SOCK_ACCEPT;
        }
    }

    return ERR_OK;
}

/**
 * Create sockets for the server : Can be used to create
 * TCP/UDP sockets on IPv4/Ipv6
 * 
 * On UDP : same socket for every client
 * On TCP : create_socket forks and returns a new socket each time
 * 
 * @param clientSockfd: pointer to the client socket created in this function
 * @param sin_family: AF_INET or AF_INET6
 * @param type: SOCK_STREAM or SOCK_DGRAM 
 * @return: the dnsd_err code
*/
dnsd_err create_socket(dns_sock *clientSock, int sin_family, int type)
{
    int errcode;                    /* error code */
    int serverSockfd;               /* server socket */
    int optval;		                /* flag value for setsockopt */

    /* Creating server socket : IPv4 or IPv6 / TCP or UDP */
    serverSockfd = socket(sin_family, type, 0);
	if (serverSockfd < 0)
		return ERR_SOCK_OPEN;

    clientSock->socketfd    = serverSockfd;
    clientSock->sin_family  = sin_family;
    clientSock->type        = type;

	/**
     * setsockopt: Prevents "ERROR on binding: address already in use" error
     * while reruning the server immediatly after killing it
	 * otherwise we have to wait about few seconds. 
	 */
    optval = 1;
    setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR,
        (const void *)&optval, sizeof(int));
    
    if ((errcode = bind_socket(serverSockfd, sin_family, type)) != ERR_OK)
        return errcode;

    if (type == SOCK_STREAM)
    {
        /* On TCP mode, fork and return every new client's socket */

        /* Placing new client connection in queue */
        if (listen(serverSockfd, TCP_MAX_CON) < 0)
        {
            close(serverSockfd);
            return ERR_SOCK_LISTEN;
        }
        
        /* Accepting new connection, fork & return every new client socket */
        while (!g_sigint)
        {
            if ((errcode = 
            accept_socket(serverSockfd, &(clientSock->socketfd), sin_family)) != ERR_OK)
                return errcode;

            switch (fork())
            {
                case -1:
                    close(clientSock->socketfd);
                    close(serverSockfd);
                    return ERR_FORK;
                case 0:
                    /* Child process - Client socket */
                    return ERR_OK;
                default:
                    /* Parent process */
                    continue;
            }
        }
        close(clientSock->socketfd);
        close(serverSockfd);
        return ERR_FORCED_SHUTDOWN;
    }
    else
    {
        /* On UDP mode, simply return server socket */
        clientSock->socketfd = serverSockfd;
        return ERR_OK;
    }
}

/**
 * Receive requests from client and respond
 * @param clientSockfd Client's socket to manage
 * @param is_stream Is the given socket a TCP socket ?
 * @return: the dnsd_err code
 */
dnsd_err handler_tcp_udp(zone_array *p_zones, dns_sock clientSock)
{
    dnsd_err code;                      /* error code */

    /* request & response */
    if (clientSock.type == SOCK_DGRAM)
    {
        while (1)
        {
            code = handle_communication(p_zones, clientSock);
        }
    }
    else
    {
        code = handle_communication(p_zones, clientSock);
    }


    close(clientSock.socketfd);

    return code;
}

/**
 * Start the DNSserver
 * 
 * @param p_zones the parsed zone file
 */
dnsd_err start_server(zone_array *p_zones)
{
    dnsd_err errcode;           /* error code from create_socket */
    dns_sock clientSock;           /* client socket returned by server */

    /* Creating socket for TCP/UDP connections on IPv4/IPv6 */
    switch (fork())
    {
        case -1:
            return ERR_FORK;
        case 0:
            /* Child process    - UDP */
            switch (fork())
            {
                case -1:
                    return ERR_FORK;
                case 0:
                    /* Child process UDP    - IPv4 */
                    errcode = 
                    create_socket(&clientSock, AF_INET, SOCK_DGRAM);
                    break;
                default:
                    /* Parent process UDP   - IPv6 */
                    errcode = 
                    create_socket(&clientSock, AF_INET6, SOCK_DGRAM);
                    break;
            }
            break;
        default:
            /* Parent process   - TCP */
            switch (fork())
            {
                case -1:
                    return ERR_FORK;
                case 0:
                    /* Child process TCP    - IPv4 */
                    errcode =
                    create_socket(&clientSock, AF_INET, SOCK_STREAM);
                    break;
                default:
                    /* Parent process TCP   - IPv6 */
                    errcode =
                    create_socket(&clientSock, AF_INET6, SOCK_STREAM);
                    break;
            }
            break;
    }

    /* Handling connections */
    if (errcode == ERR_OK)
    {
        /* Q/A with client */
        handler_tcp_udp(p_zones, clientSock);
        return ERR_OK;
    }
    return errcode;
}
