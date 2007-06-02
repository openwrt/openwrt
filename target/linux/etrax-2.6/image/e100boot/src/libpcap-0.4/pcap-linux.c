/*
 * Copyright (c) 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-linux.c,v 1.1 1999/08/26 10:05:24 johana Exp $ (LBL)";
#endif

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <net/if.h>
#ifdef HAVE_NET_IF_ARP_H
#include <net/if_arp.h>
#else
#include <linux/if_arp.h>
#endif
#include <linux/if_ether.h>

#include <netinet/in.h>

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct ifreq saved_ifr;
static int read_timout_ms = 0;

#include "pcap-int.h"

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

void linux_restore_ifr(void);

int
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{

	*ps = p->md.stat;
	return (0);
}

int
pcap_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	register int cc;
	register int bufsize;
	register int caplen;
	register u_char *bp;
	struct sockaddr from;
	int fromlen;

	fd_set mask;
	struct timeval tv;

	if (read_timout_ms == 0) {
	  tv.tv_sec  = 0;
	  tv.tv_usec = 0;
	}
	else {
	  tv.tv_sec  = read_timout_ms/1000;
	  tv.tv_usec = read_timout_ms%1000;
	}
	FD_ZERO(&mask);
	FD_SET(p->fd, &mask);

	bp = p->buffer + p->offset;
	bufsize = p->bufsize;
	if (p->md.pad > 0) {
		memset(bp, 0, p->md.pad);
		bp += p->md.pad;
		bufsize -= p->md.pad;
	}

again:
	do {
		fromlen = sizeof(from);
		select(FD_SETSIZE, &mask, NULL, NULL, &tv);
		if (FD_ISSET(p->fd, &mask) == 0) {
		  return (0);
		}

		cc = recvfrom(p->fd, bp, bufsize, 0, &from, &fromlen);
		if (cc < 0) {
			/* Don't choke when we get ptraced */
			switch (errno) {

			case EINTR:
					goto again;

			case EWOULDBLOCK:
				return (0);		/* XXX */
			}
			sprintf(p->errbuf, "read: %s", pcap_strerror(errno));
			return (-1);
		}
	} while (strcmp(p->md.device, from.sa_data));

	/* If we need have leading zero bytes, adjust count */
	cc += p->md.pad;
	bp = p->buffer + p->offset;

	/* If we need to step over leading junk, adjust count and pointer */
	cc -= p->md.skip;
	bp += p->md.skip;

	/* Captured length can't exceed our read buffer size */
	caplen = cc;
	if (caplen > bufsize)
		caplen = bufsize;

	/* Captured length can't exceed the snapshot length */
	if (caplen > p->snapshot)
		caplen = p->snapshot;

	if (p->fcode.bf_insns == NULL ||
	    bpf_filter(p->fcode.bf_insns, bp, cc, caplen)) {
		struct pcap_pkthdr h;

		++p->md.stat.ps_recv;
		/* Get timestamp */
		if (ioctl(p->fd, SIOCGSTAMP, &h.ts) < 0) {
			sprintf(p->errbuf, "SIOCGSTAMP: %s",
			    pcap_strerror(errno));
			return (-1);
		}
		h.len = cc;
		h.caplen = caplen;
		(*callback)(user, &h, bp);
		return (1);
	}
	return (0);
}

pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	register int fd, broadcast;
	register pcap_t *p;
	struct ifreq ifr;
	struct sockaddr sa;

	read_timout_ms = to_ms;
	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		return (NULL);
	}
	memset(p, 0, sizeof(*p));
	fd = -1;

	fd = socket(PF_INET, SOCK_PACKET, htons(ETH_P_ALL));
	if (fd < 0) {
		sprintf(ebuf, "socket: %s", pcap_strerror(errno));
		goto bad;
	}
	p->fd = fd;

	/* Bind to the interface name */
	memset(&sa, 0, sizeof(sa));
	sa.sa_family = AF_INET;
	(void)strncpy(sa.sa_data, device, sizeof(sa.sa_data));
	if (bind(p->fd, &sa, sizeof(sa))) {
		sprintf(ebuf, "bind: %s: %s", device, pcap_strerror(errno));
		goto bad;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(p->fd, SIOCGIFHWADDR, &ifr) < 0 ) {
		sprintf(ebuf, "SIOCGIFHWADDR: %s", pcap_strerror(errno));
		goto bad;
	}
	broadcast = 0;
	switch (ifr.ifr_hwaddr.sa_family) {

	case ARPHRD_ETHER:
	case ARPHRD_METRICOM:
		p->linktype = DLT_EN10MB;
		p->offset = 2;
		++broadcast;
		break;

	case ARPHRD_EETHER:
		p->linktype = DLT_EN3MB;
		++broadcast;
		break;

	case ARPHRD_AX25:
		p->linktype = DLT_AX25;
		++broadcast;
		break;

	case ARPHRD_PRONET:
		p->linktype = DLT_PRONET;
		break;

	case ARPHRD_CHAOS:
		p->linktype = DLT_CHAOS;
		break;

	case ARPHRD_IEEE802:
		p->linktype = DLT_IEEE802;
		++broadcast;
		break;

	case ARPHRD_ARCNET:
		p->linktype = DLT_ARCNET;
		++broadcast;
		break;

	case ARPHRD_SLIP:
	case ARPHRD_CSLIP:
	case ARPHRD_SLIP6:
	case ARPHRD_CSLIP6:
	case ARPHRD_PPP:
		p->linktype = DLT_RAW;
		break;

	case ARPHRD_LOOPBACK:
		p->linktype = DLT_NULL;
		p->md.pad = 2;
		p->md.skip = 12;
		break;

#ifdef ARPHRD_FDDI
	/* Not all versions of the kernel has this define */
	case ARPHRD_FDDI:
		p->linktype = DLT_FDDI;
		++broadcast;
		break;
#endif

#ifdef notdef
	case ARPHRD_LOCALTLK:
	case ARPHRD_NETROM:
	case ARPHRD_APPLETLK:
	case ARPHRD_DLCI:
	case ARPHRD_RSRVD:
	case ARPHRD_ADAPT:
	case ARPHRD_TUNNEL:
	case ARPHRD_TUNNEL6:
	case ARPHRD_FRAD:
	case ARPHRD_SKIP:
		/* XXX currently do not know what to do with these... */
		abort();
#endif

	default:
		sprintf(ebuf, "unknown physical layer type 0x%x",
		    ifr.ifr_hwaddr.sa_family);
		goto bad;
	}

	/* Base the buffer size on the interface MTU */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(p->fd, SIOCGIFMTU, &ifr) < 0 ) {
		sprintf(ebuf, "SIOCGIFMTU: %s", pcap_strerror(errno));
		goto bad;
	}

	/* Leave room for link header (which is never large under linux...) */
	p->bufsize = ifr.ifr_mtu + 64;

	p->buffer = (u_char *)malloc(p->bufsize + p->offset);
	if (p->buffer == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		goto bad;
	}

	/* XXX */
	if (promisc && broadcast) {
		memset(&ifr, 0, sizeof(ifr));
		strcpy(ifr.ifr_name, device);
		if (ioctl(p->fd, SIOCGIFFLAGS, &ifr) < 0 ) {
			sprintf(ebuf, "SIOCGIFFLAGS: %s", pcap_strerror(errno));
			goto bad;
		}
		saved_ifr = ifr;
		ifr.ifr_flags |= IFF_PROMISC;
		if (ioctl(p->fd, SIOCSIFFLAGS, &ifr) < 0 ) {
			sprintf(ebuf, "SIOCSIFFLAGS: %s", pcap_strerror(errno));
			goto bad;
		}
		ifr.ifr_flags &= ~IFF_PROMISC;
		atexit(linux_restore_ifr);
	}

	p->md.device = strdup(device);
	if (p->md.device == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		goto bad;
	}
	p->snapshot = snaplen;

	return (p);
bad:
	if (fd >= 0)
		(void)close(fd);
	if (p->buffer != NULL)
		free(p->buffer);
	if (p->md.device != NULL)
		free(p->md.device);
	free(p);
	return (NULL);
}

int
pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{

	p->fcode = *fp;
	return (0);
}

void
linux_restore_ifr(void)
{
	register int fd;

	fd = socket(PF_INET, SOCK_PACKET, htons(0x0003));
	if (fd < 0)
		fprintf(stderr, "linux socket: %s", pcap_strerror(errno));
	else if (ioctl(fd, SIOCSIFFLAGS, &saved_ifr) < 0)
		fprintf(stderr, "linux SIOCSIFFLAGS: %s", pcap_strerror(errno));
}
