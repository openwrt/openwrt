/*
 * Copyright (C) 2018 Marco d'Itri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* for getaddrinfo... */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>

#include "network.h"
#include "utils.h"
#include "log.h"

char *print_addr_port(const struct sockaddr *addr, socklen_t addrlen)
{
    static char buf[1100], address[1025], port[32];
    int err;

    err = getnameinfo(addr, addrlen, address, sizeof(address),
		      port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
    if (err == EAI_SYSTEM)
	err_sys("getnameinfo");
    else if (err)
	log_printf_exit(1, log_err, "getnameinfo: %s", gai_strerror(err));

    if (addr->sa_family == AF_INET6)
	snprintf(buf, sizeof(buf) - 1, "[%s]:%s", address, port);
    else
	snprintf(buf, sizeof(buf) - 1, "%s:%s", address, port);

    return buf;
}

static char *ai_print_addr_port(struct addrinfo *ai)
{
    return print_addr_port((struct sockaddr *) ai->ai_addr, ai->ai_addrlen);
}

/*
 * Try to parse anything that looks like:
 * - an IPv4 or IPv6 address or a domain, with an optional port number
 * - a port number
 *
 * address and/or port will be NULL if not found in the input.
 * If address and port are not NULL then they must be freed by the caller.
 */
static void parse_address_port(const char *input, char **address, char **port)
{
    const char *p;

    *address = NULL;
    *port = NULL;

    if (*input == '\0') {
	return;
    } else if (*input == '[' && (p = strchr(input, ']'))) {	/* IPv6 */
	char *s;
	int len = p - input - 1;

	*address = s = NOFAIL(malloc(len + 1));
	memcpy(s, input + 1, len);
	*(s + len) = '\0';

	p = strchr(p, ':');
	if (p && *(p + 1) != '\0')
	    *port = NOFAIL(strdup(p + 1));		/* IPv6 + port */
    } else if ((p = strchr(input, ':')) &&		/* IPv6, no port */
	       strchr(p + 1, ':')) {			/*   and no brackets */
	*address = NOFAIL(strdup(input));
    } else if ((p = strchr(input, ':'))) {		/* IPv4 + port */
	char *s;
	int len = p - input;

	if (len) {
	    *address = s = NOFAIL(malloc(len + 1));
	    memcpy(s, input, len);
	    *(s + len) = '\0';
	}

	p++;
	if (*p != '\0')
	    *port = NOFAIL(strdup(p));
    } else {
	for (p = input; *p; p++)
	    if (!isdigit(p[0]))
		break;
	if (*p)
	    *address = NOFAIL(strdup(input));		/* IPv4, no port */
	else
	    *port = NOFAIL(strdup(input));		/* just the port */
    }
}

int udp_listener(const char *s)
{
    char *address, *port;
    struct addrinfo hints, *res, *ai;
    int err, fd;

    parse_address_port(s, &address, &port);

    if (!port)
	log_printf_exit(2, log_err, "Missing port in '%s'!", s);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_IDN;

    err = getaddrinfo(address, port, &hints, &res);
    if (err == EAI_SYSTEM)
	err_sys("getaddrinfo(%s:%s)", address, port);
    else if (err)
	log_printf_exit(1, log_err, "Cannot resolve %s:%s: %s",
		address, port, gai_strerror(err));

    if (address)
	free(address);
    if (port)
	free(port);

    for (ai = res; ai; ai = ai->ai_next) {
	if ((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0)
	    continue;           /* ignore */
	if (bind(fd, (struct sockaddr *) ai->ai_addr, ai->ai_addrlen) == 0)
	    break;              /* success */
	close(fd);
    }

    if (!ai)
	err_sys("Cannot bind to %s", s);

    log_printf(log_info, "Listening for UDP connections on %s",
	    ai_print_addr_port(ai));

    freeaddrinfo(res);

    return fd;
}

int *tcp_listener(const char *s)
{
    char *address, *port;
    struct addrinfo hints, *res, *ai;
    int err, fd, opt;
    int fd_num = 0;
    int *fd_list = NULL;
    size_t allocated_fds = 0;

    parse_address_port(s, &address, &port);

    if (!port)
	log_printf_exit(2, log_err, "Missing port in '%s'!", s);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_IDN;

    err = getaddrinfo(address, port, &hints, &res);
    if (err == EAI_SYSTEM)
	err_sys("getaddrinfo(%s:%s)", address, port);
    else if (err)
	log_printf_exit(1, log_err, "Cannot resolve %s:%s: %s",
		address, port, gai_strerror(err));

    if (address)
	free(address);
    if (port)
	free(port);

    /* add to fd_list all the sockets which match ai_flags */
    for (ai = res; ai; ai = ai->ai_next) {
	if ((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0)
	    continue;		/* ignore */
	opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	    err_sys("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
	if (bind(fd, (struct sockaddr *) ai->ai_addr, ai->ai_addrlen) < 0)
	    err_sys("Cannot bind to %s", s);

	/* success */
	if (listen(fd, 128) < 0)
	    err_sys("listen");

	if (allocated_fds < fd_num + 1 + 1) {
	    allocated_fds += 8;
	    fd_list = realloc(fd_list, allocated_fds * sizeof(int));
	}
	fd_list[fd_num++] = fd;

	log_printf(log_info, "Listening for TCP connections on %s",
		ai_print_addr_port(ai));
    }

    /* and then add -1 as the list terminator */
    if (allocated_fds < fd_num + 1 + 1)
	fd_list = realloc(fd_list, ++allocated_fds * sizeof(int));
    fd_list[fd_num] = -1;

    if (!fd_list)
	err_sys("socket");

    freeaddrinfo(res);

    return fd_list;
}

/*
 * Accept new connections and return after forking for each one.
 */
int accept_connections(int listening_sockets[])
{
    while (1) {
	int max = 0;
	int i, fd;
	fd_set readfds;
	pid_t pid;

	FD_ZERO(&readfds);
	for (i = 0; listening_sockets[i] != -1; i++) {
	    int flags;

	    if ((flags = fcntl(listening_sockets[i], F_GETFL, 0)) < 0)
		err_sys("fcntl(F_GETFL)");
	    if (fcntl(listening_sockets[i], F_SETFL, flags | O_NONBLOCK) < 0)
		err_sys("fcntl(F_SETFL, O_NONBLOCK)");

	    FD_SET(listening_sockets[i], &readfds);
	    SET_MAX(listening_sockets[i]);
	}

	if (select(max, &readfds, NULL, NULL, NULL) < 0) {
	    if (errno == EINTR || errno == EAGAIN)
		continue;
	    err_sys("select");
	}

	for (i = 0; listening_sockets[i] != -1; i++) {
	    int listen_sock;
	    struct sockaddr_storage client_addr;
	    socklen_t addrlen = sizeof(client_addr);

	    if (!FD_ISSET(listening_sockets[i], &readfds))
		continue;
	    listen_sock = listening_sockets[i];

	    fd = accept(listen_sock, (struct sockaddr *) &client_addr, &addrlen);
	    if (fd < 0) {
		if (errno == EAGAIN)
		    continue;
		err_sys("accept");
	    }

	    log_printf(log_notice, "Received a TCP connection from %s",
		    print_addr_port((struct sockaddr *) &client_addr, addrlen));

#if 0
	    /* do not fork, for testing */
	    pid = 0;
#else
	    pid = fork();
#endif

	    if (pid < 0)
		err_sys("fork");

	    if (pid > 0) {
		close(fd);
	    } else {
		for (i = 0; listening_sockets[i] != -1; i++)
		    close(listening_sockets[i]);
		return fd;
	    }
	}
    }
}

int udp_client(const char *s, struct sockaddr_storage *remote_udpaddr)
{
    char *address, *port;
    struct addrinfo hints, *res, *ai;
    int err, fd;

    parse_address_port(s, &address, &port);

    if (!address || !port)
	log_printf_exit(2, log_err, "Missing address or port in '%s'!", s);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_IDN;

    err = getaddrinfo(address, port, &hints, &res);
    if (err == EAI_SYSTEM)
	err_sys("getaddrinfo(%s:%s)", address, port);
    else if (err)
	log_printf_exit(1, log_err, "Cannot resolve %s:%s: %s",
		address, port, gai_strerror(err));

    if (address)
	free(address);
    if (port)
	free(port);

    /* continue with the first socket which matches ai_flags */
    for (ai = res; ai; ai = ai->ai_next) {
	if ((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0)
	    continue;		/* ignore */
	break;			/* success */
    }

    if (!ai)
	err_sys("socket");

    log_printf(log_debug, "The UDP destination is %s", ai_print_addr_port(ai));

    /*
     * Return to the caller the resolved address, to be able to use it as the
     * destination address of the next UDP packet.
     */
    if (remote_udpaddr)
	memcpy(remote_udpaddr, ai->ai_addr, ai->ai_addrlen);

    freeaddrinfo(res);

    return fd;
}

int tcp_client(const char *s)
{
    char *address, *port;
    struct addrinfo hints, *res, *ai;
    int err, fd;

    parse_address_port(s, &address, &port);

    if (!address || !port)
	log_printf_exit(2, log_err, "Missing address or port in '%s'!", s);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG | AI_IDN;

    err = getaddrinfo(address, port, &hints, &res);
    if (err == EAI_SYSTEM)
	err_sys("getaddrinfo(%s:%s)", address, port);
    else if (err)
	log_printf_exit(1, log_err, "Cannot resolve %s:%s: %s",
		address, port, gai_strerror(err));

    if (address)
	free(address);
    if (port)
	free(port);

    for (ai = res; ai; ai = ai->ai_next) {
	if ((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0)
	    continue;		/* ignore */
	if (connect(fd, (struct sockaddr *) ai->ai_addr, ai->ai_addrlen) == 0)
	    break;		/* success */
	close(fd);
    }

    if (!ai)
	err_sys("Cannot connect to %s", s);

    log_printf(log_info, "TCP connection opened to %s",
	    ai_print_addr_port(ai));

    freeaddrinfo(res);

    return fd;
}

