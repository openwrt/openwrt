/*
 * Copyright (c) 1990, 1991, 1992, 1993, 1994, 1995, 1996
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
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-nit.c,v 1.1 1999/08/26 10:05:24 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/nit.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>

#include "pcap-int.h"

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

/*
 * The chunk size for NIT.  This is the amount of buffering
 * done for read calls.
 */
#define CHUNKSIZE (2*1024)

/*
 * The total buffer space used by NIT.
 */
#define BUFSPACE (4*CHUNKSIZE)

/* Forwards */
static int nit_setflags(int, int, int, char *);

int
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{

	*ps = p->md.stat;
	return (0);
}

int
pcap_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	register int cc, n;
	register struct bpf_insn *fcode = p->fcode.bf_insns;
	register u_char *bp, *cp, *ep;
	register struct nit_hdr *nh;
	register int caplen;

	cc = p->cc;
	if (cc == 0) {
		cc = read(p->fd, (char *)p->buffer, p->bufsize);
		if (cc < 0) {
			if (errno == EWOULDBLOCK)
				return (0);
			sprintf(p->errbuf, "pcap_read: %s",
				pcap_strerror(errno));
			return (-1);
		}
		bp = p->buffer;
	} else
		bp = p->bp;

	/*
	 * Loop through each packet.  The increment expression
	 * rounds up to the next int boundary past the end of
	 * the previous packet.
	 */
	n = 0;
	ep = bp + cc;
	while (bp < ep) {
		nh = (struct nit_hdr *)bp;
		cp = bp + sizeof(*nh);

		switch (nh->nh_state) {

		case NIT_CATCH:
			break;

		case NIT_NOMBUF:
		case NIT_NOCLUSTER:
		case NIT_NOSPACE:
			p->md.stat.ps_drop = nh->nh_dropped;
			continue;

		case NIT_SEQNO:
			continue;

		default:
			sprintf(p->errbuf, "bad nit state %d", nh->nh_state);
			return (-1);
		}
		++p->md.stat.ps_recv;
		bp += ((sizeof(struct nit_hdr) + nh->nh_datalen +
		    sizeof(int) - 1) & ~(sizeof(int) - 1));

		caplen = nh->nh_wirelen;
		if (caplen > p->snapshot)
			caplen = p->snapshot;
		if (bpf_filter(fcode, cp, nh->nh_wirelen, caplen)) {
			struct pcap_pkthdr h;
			h.ts = nh->nh_timestamp;
			h.len = nh->nh_wirelen;
			h.caplen = caplen;
			(*callback)(user, &h, cp);
			if (++n >= cnt && cnt >= 0) {
				p->cc = ep - bp;
				p->bp = bp;
				return (n);
			}
		}
	}
	p->cc = 0;
	return (n);
}

static int
nit_setflags(int fd, int promisc, int to_ms, char *ebuf)
{
	struct nit_ioc nioc;

	bzero((char *)&nioc, sizeof(nioc));
	nioc.nioc_bufspace = BUFSPACE;
	nioc.nioc_chunksize = CHUNKSIZE;
	nioc.nioc_typetomatch = NT_ALLTYPES;
	nioc.nioc_snaplen = p->snapshot;
	nioc.nioc_bufalign = sizeof(int);
	nioc.nioc_bufoffset = 0;

	if (to_ms != 0) {
		nioc.nioc_flags |= NF_TIMEOUT;
		nioc.nioc_timeout.tv_sec = to_ms / 1000;
		nioc.nioc_timeout.tv_usec = (to_ms * 1000) % 1000000;
	}
	if (promisc)
		nioc.nioc_flags |= NF_PROMISC;

	if (ioctl(fd, SIOCSNIT, &nioc) < 0) {
		sprintf(ebuf, "SIOCSNIT: %s", pcap_strerror(errno));
		return (-1);
	}
	return (0);
}

pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	int fd;
	struct sockaddr_nit snit;
	register pcap_t *p;

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		strcpy(ebuf, pcap_strerror(errno));
		return (NULL);
	}

	if (snaplen < 96)
		/*
		 * NIT requires a snapshot length of at least 96.
		 */
		snaplen = 96;

	bzero(p, sizeof(*p));
	p->fd = fd = socket(AF_NIT, SOCK_RAW, NITPROTO_RAW);
	if (fd < 0) {
		sprintf(ebuf, "socket: %s", pcap_strerror(errno));
		goto bad;
	}
	snit.snit_family = AF_NIT;
	(void)strncpy(snit.snit_ifname, device, NITIFSIZ);

	if (bind(fd, (struct sockaddr *)&snit, sizeof(snit))) {
		sprintf(ebuf, "bind: %s: %s", snit.snit_ifname,
			pcap_strerror(errno));
		goto bad;
	}
	p->snapshot = snaplen;
	nit_setflags(p->fd, promisc, to_ms, ebuf);

	/*
	 * NIT supports only ethernets.
	 */
	p->linktype = DLT_EN10MB;

	p->bufsize = BUFSPACE;
	p->buffer = (u_char *)malloc(p->bufsize);
	if (p->buffer == NULL) {
		strcpy(ebuf, pcap_strerror(errno));
		goto bad;
	}
	return (p);
 bad:
	if (fd >= 0)
		close(fd);
	free(p);
	return (NULL);
}

int
pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{

	p->fcode = *fp;
	return (0);
}
