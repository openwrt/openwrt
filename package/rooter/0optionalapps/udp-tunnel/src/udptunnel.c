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
 *
 *
 * Parts of this program are derived from udptunnel.c by Jonathan Lennox.
 * This is the license of the original code:
 *
 * Copyright 1999, 2001 by Columbia University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* for sigaction... */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_SYSTEMD_SD_DAEMON_H
#include <systemd/sd-daemon.h>
#endif

#include "utils.h"
#include "log.h"
#include "network.h"

#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif

#define TCPBUFFERSIZE 65536
#define UDPBUFFERSIZE (TCPBUFFERSIZE - 2)	/* TCP packet - length */

struct out_packet {
    uint16_t length;
    char buf[UDPBUFFERSIZE];
};

struct opts {
    const char *udpaddr, *tcpaddr;

    int is_server;
    int use_inetd;
    char *handshake;
    int timeout;
};

struct relay {
    struct sockaddr_storage remote_udpaddr;

    int udp_sock, tcp_sock;

    int expect_handshake;
    char handshake[32];
    int udp_timeout, tcp_timeout;
    char buf[TCPBUFFERSIZE];
    char *buf_ptr, *packet_start;
    int packet_length;
    enum {
	uninitialized = 0,
	reading_handshake,
	reading_length,
	reading_packet,
    } state;
};

static void usage(int status)
{
    FILE *fp = status == 0 ? stdout : stderr;

    fprintf(fp, "Usage: udptunnel [OPTION]... [[SOURCE:]PORT] DESTINATION:PORT\n\n");
    fprintf(fp, "-s    --server         listen for TCP connections\n");
    fprintf(fp, "-i    --inetd          expect to be started by inetd\n");
    fprintf(fp, "-T N  --timeout N      close the source connection after N seconds\n");
    fprintf(fp, "                       where no data was received\n");
    fprintf(fp, "-S    --syslog         log to syslog instead of standard error\n");
    fprintf(fp, "-v    --verbose        explain what is being done\n");
    fprintf(fp, "-h    --help           display this help and exit\n");
    fprintf(fp, "\nSOURCE:PORT must not be specified when using inetd or socket activation.\n\n");
    fprintf(fp, "If the -s option is used then the program will listen on SOURCE:PORT for TCP\n");
    fprintf(fp, "connections and relay the encapsulated packets with UDP to DESTINATION:PORT.\n");
    fprintf(fp, "Otherwise it will listen on SOURCE:PORT for UDP packets and encapsulate\n");
    fprintf(fp, "them in a TCP connection to DESTINATION:PORT.\n");

    exit(status);
}

static void parse_args(int argc, char *argv[], struct opts *opts)
{
#ifdef HAVE_GETOPT_LONG
    const struct option longopts[] = {
	{"inetd",			no_argument,		NULL, 'i' },
	{"server",			no_argument,		NULL, 's' },
	{"syslog",			no_argument,		NULL, 'S' },
	{"timeout",			required_argument,	NULL, 'T' },
	{"help",			no_argument,		NULL, 'h' },
	{"verbose",			no_argument,		NULL, 'v' },
	{NULL,				0,			NULL, 0   },
    };
    int longindex;
#endif
    int c;
    int expected_args;
    int verbose = 0;
    int use_syslog = 0;

    /* defaults */
    opts->handshake = NOFAIL(malloc(32));
    memcpy(opts->handshake, "udptunnel by md.\0\0\0\x01\x03\x06\x10\x15\x21\x28\x36\x45\x55\x66\x78\x91", 32);

    while ((c = GETOPT_LONGISH(argc, argv, "ihsvST:",
		    longopts, &longindex)) > 0) {
	switch (c) {
	case 'i':
	    opts->use_inetd = 1;
	    break;
	case 's':
	    opts->is_server = 1;
	    break;
	case 'S':
	    use_syslog = 1;
	    break;
	case 'T':
	    opts->timeout = atol(optarg);
	    break;
	case 'v':
	    verbose++;
	    break;
	case 'h':
	    usage(0);
	    break;
	default:
	    usage(2);
	    break;
	}
    }

    /*
     * Look for 2 command line arguments (source and destination)
     * if used in standalone mode or only 1 argument (destination)
     * if used in inetd or socket activated modes.
     */
    expected_args = (sd_listen_fds(0) || opts->use_inetd) ? 1 : 2;

    if (argc - optind == 0)
	usage(2);
    if (argc - optind != expected_args) {
	fprintf(stderr, "Expected %d argument(s)!\n\n", expected_args);
	usage(2);
    }

    /* the source and destination addresses */
    if (opts->is_server) {
	if (expected_args == 2)
	    opts->tcpaddr = NOFAIL(strdup(argv[optind++]));
	opts->udpaddr = NOFAIL(strdup(argv[optind++]));
    } else {
	if (expected_args == 2)
	    opts->udpaddr = NOFAIL(strdup(argv[optind++]));
	opts->tcpaddr = NOFAIL(strdup(argv[optind++]));
    }

    if (!verbose)
	log_set_options(log_warning);
    else if (verbose == 1)
	log_set_options(log_notice);
    else if (verbose == 2)
	log_set_options(log_info);
    else
	log_set_options(log_debug);

    if (use_syslog)
	log_set_options(log_get_filter_level() | log_syslog);
}

int udp_listener_sa(const int num)
{
    int fd = SD_LISTEN_FDS_START;

    if (num != 1)
	log_printf_exit(2, log_err,
		"UDP socket activation supports a single socket.");

    if (sd_is_socket(fd, AF_UNSPEC, SOCK_DGRAM, -1) <= 0)
	log_printf_exit(2, log_err,
		"UDP socket activation fd %d is not valid.", fd);

    return fd;
}

int *tcp_listener_sa(const int num)
{
    int fd;
    int *fds;
    int fd_num = 0;

    fds = NOFAIL(malloc((num + 1) * sizeof(int)));

    for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + num; fd++) {
	if (sd_is_socket(fd, AF_UNSPEC, SOCK_STREAM, 1) <= 0)
	    log_printf_exit(2, log_err,
		    "TCP socket activation fd %d is not valid.", fd);
	fds[fd_num++] = fd;
    }

    fds[fd_num] = -1;

    return fds;
}

static void udp_to_tcp(struct relay *relay)
{
    struct out_packet p;
    int buflen;
    struct sockaddr_storage remote_udpaddr;
    socklen_t addrlen = sizeof(remote_udpaddr);

    buflen = recvfrom(relay->udp_sock, p.buf, UDPBUFFERSIZE, 0,
		      (struct sockaddr *) &remote_udpaddr, &addrlen);
    if (buflen < 0)
	err_sys("recvfrom(udp)");
    if (buflen == 0)
	return;	/* ignore empty packets */

    /*
     * Store the source address of the received UDP packet, to be able to use
     * it in send_udp_packet as the destination address of the next UDP reply.
     */
    memcpy(&(relay->remote_udpaddr), &remote_udpaddr, addrlen);

#ifdef DEBUG
    log_printf(log_debug, "Received a %d bytes UDP packet from %s", buflen,
	    print_addr_port((struct sockaddr *) &remote_udpaddr, addrlen));
#endif

    p.length = htons(buflen);
    if (send(relay->tcp_sock, &p, buflen + sizeof(p.length), 0) < 0)
	err_sys("send(tcp)");
}

static void send_udp_packet(struct relay *relay)
{
    int opt = 0;
    socklen_t len = sizeof(opt);

    if (relay->remote_udpaddr.ss_family == 0) {
	log_printf(log_info,
		"Ignoring a packet for a still unknown UDP destination!");
	return;
    }

    if (sendto(relay->udp_sock, relay->packet_start, relay->packet_length, 0,
	       (struct sockaddr *) &relay->remote_udpaddr,
	       sizeof(relay->remote_udpaddr)) >= 0)
	return;

    /* this is the error path */
    if (errno != ECONNREFUSED)
	err_sys("sendto(udp)");

    /* clear the error if there is no UDP listener yet on the other end */
    log_printf(log_info, "sendto(udp) returned ECONNREFUSED: ignored");
    if (getsockopt(relay->udp_sock, SOL_SOCKET, SO_ERROR, &opt, &len) < 0)
	err_sys("getsockopt(udp, SOL_SOCKET, SO_ERROR)");

    return;
}

static void tcp_to_udp(struct relay *relay)
{
    int read_len;

    if (relay->state == uninitialized) {
	if (relay->expect_handshake) {
	    relay->state = reading_handshake;
	    relay->packet_length = sizeof(relay->handshake);
	} else {
	    relay->state = reading_length;
	    relay->packet_length = sizeof(uint16_t);
	}
	relay->buf_ptr = relay->buf;
	relay->packet_start = relay->buf;
    }

    read_len = read(relay->tcp_sock, relay->buf_ptr,
		    (relay->buf + TCPBUFFERSIZE - relay->buf_ptr));
    if (read_len < 0)
	err_sys("read(tcp)");

    if (read_len == 0)
	log_printf_exit(0, log_notice, "Remote closed the connection");

    relay->buf_ptr += read_len;

    while (relay->buf_ptr - relay->packet_start >= relay->packet_length) {
	if (relay->state == reading_handshake) {
	    /* check the handshake string */
	    if (memcmp(relay->packet_start, &(relay->handshake),
		       sizeof(relay->handshake)) != 0)
		log_printf_exit(0, log_info,
			"Received a bad handshake, exiting");
	    log_printf(log_debug, "Received a good handshake");
	    relay->packet_start += sizeof(relay->handshake);
	    relay->state = reading_length;
	    relay->packet_length = sizeof(uint16_t);
	} else if (relay->state == reading_length) {
	    /* read the lenght of the next packet */
	    relay->packet_length = ntohs(*(uint16_t *) relay->packet_start);
	    relay->packet_start += sizeof(uint16_t);
	    relay->state = reading_packet;
	} else if (relay->state == reading_packet) {
	    /* read an encapsulated packet and send it as UDP */
#ifdef DEBUG
	    log_printf(log_debug, "Received a %u bytes TCP packet",
			relay->packet_length);
#endif

	    send_udp_packet(relay);

	    memmove(relay->buf, relay->packet_start + relay->packet_length,
		    relay->buf_ptr -
		    (relay->packet_start + relay->packet_length));
	    relay->buf_ptr -=
		relay->packet_length + (relay->packet_start - relay->buf);
	    relay->packet_start = relay->buf;
	    relay->state = reading_length;
	    relay->packet_length = sizeof(uint16_t);
	}
    }
}

static void send_handshake(struct relay *relay)
{
    if (sendto(relay->tcp_sock, relay->handshake, sizeof(relay->handshake), 0,
	       (struct sockaddr *) &relay->remote_udpaddr,
	       sizeof(relay->remote_udpaddr)) < 0)
	err_sys("sendto(tcp, handshake)");
}

static void wait_for_child(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

static void main_loop(struct relay *relay)
{
    time_t last_udp_input, last_tcp_input;

    last_udp_input = relay->udp_timeout ? time(NULL) : 0;
    last_tcp_input = relay->tcp_timeout ? time(NULL) : 0;

    while (1) {
	int ready_fds;
	int max = 0;
	fd_set readfds;
	struct timeval tv, *ptv;

	FD_ZERO(&readfds);
	FD_SET(relay->tcp_sock, &readfds);
	SET_MAX(relay->tcp_sock);
	FD_SET(relay->udp_sock, &readfds);
	SET_MAX(relay->udp_sock);

	/*
	 * If a data timeout was configured then set a 10s timeout for
	 * select.
	 */
	if (last_udp_input || last_tcp_input) {
	    tv.tv_usec = 0;
	    tv.tv_sec = 10;
	    ptv = &tv;
	} else {
	    ptv = NULL;
	}

	ready_fds = select(max, &readfds, NULL, NULL, ptv);
	if (ready_fds < 0) {
	    if (errno == EINTR || errno == EAGAIN)
		continue;
	    err_sys("select");
	}

	/* check the timeout if one was configured and select(2) timed out */
	if (last_udp_input && !ready_fds) {	/* timeout */
	    if (time(NULL) - last_udp_input > relay->udp_timeout)
		log_printf_exit(0, log_notice,
			"Exiting after a %ds timeout for UDP input",
			relay->udp_timeout);
	}
	if (last_tcp_input && !ready_fds) {	/* timeout */
	    if (time(NULL) - last_tcp_input > relay->tcp_timeout)
		log_printf_exit(0, log_notice,
			"Exiting after a %ds timeout for TCP input",
			relay->tcp_timeout);
	}

	if (FD_ISSET(relay->tcp_sock, &readfds)) {
	    tcp_to_udp(relay);
	    if (last_tcp_input)
		last_tcp_input = time(NULL);
	}
	if (FD_ISSET(relay->udp_sock, &readfds)) {
	    udp_to_tcp(relay);
	    if (last_udp_input)
		last_udp_input = time(NULL);
	}
    }
}

int main(int argc, char *argv[])
{
    struct opts opts;
    struct relay relay;

    memset(&relay, 0, sizeof(relay));
    relay.tcp_sock = -1;

    memset(&opts, 0, sizeof(opts));
    parse_args(argc, argv, &opts);
    if (opts.handshake)
	memcpy(relay.handshake, opts.handshake, sizeof(relay.handshake));

    sd_notify(0, "READY=1");

    if (opts.is_server) {
	struct sigaction sa;

	sa.sa_handler = wait_for_child;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	    err_sys("sigaction");

	if (opts.timeout)
	    relay.tcp_timeout = opts.timeout;
	relay.expect_handshake = 1;

	if (opts.use_inetd) {
	    relay.tcp_sock = 0;
	    log_set_options(log_get_filter_level() | log_syslog);
	} else {
	    int socket_activation_fds = sd_listen_fds(0);
	    int *listening_sockets;

	    if (socket_activation_fds)
		listening_sockets = tcp_listener_sa(socket_activation_fds);
	    else
		listening_sockets = tcp_listener(opts.tcpaddr);
	    relay.tcp_sock = accept_connections(listening_sockets);
	}
	relay.udp_sock = udp_client(opts.udpaddr, &relay.remote_udpaddr);
    } else {
	if (opts.timeout)
	    relay.udp_timeout = opts.timeout;

	if (opts.use_inetd) {
	    relay.udp_sock = 0;
	    log_set_options(log_get_filter_level() | log_syslog);
	} else {
	    int socket_activation_fds = sd_listen_fds(0);

	    if (socket_activation_fds)
		relay.udp_sock = udp_listener_sa(socket_activation_fds);
	    else
		relay.udp_sock = udp_listener(opts.udpaddr);
	}
	relay.tcp_sock = tcp_client(opts.tcpaddr);

	send_handshake(&relay);
    }

    main_loop(&relay);
    exit(0);
}

