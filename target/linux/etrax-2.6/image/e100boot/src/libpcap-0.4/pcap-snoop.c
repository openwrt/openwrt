/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
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
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-snoop.c,v 1.1 1999/08/26 10:05:26 johana Exp $ (LBL)";
#endif

#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <net/raw.h>
#include <net/if.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pcap-int.h"

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

int
pcap_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	int cc;
	register struct snoopheader *sh;
	register int datalen;
	register int caplen;
	register u_char *cp;

again:
	cc = read(p->fd, (char *)p->buffer, p->bufsize);
	if (cc < 0) {
		/* Don't choke when we get ptraced */
		switch (errno) {

		case EINTR:
				goto again;

		case EWOULDBLOCK:
			return (0);			/* XXX */
		}
		sprintf(p->errbuf, "read: %s", pcap_strerror(errno));
		return (-1);
	}
	sh = (struct snoopheader *)p->buffer;
	datalen = sh->snoop_packetlen;
	caplen = (datalen < p->snapshot) ? datalen : p->snapshot;
	cp = (u_char *)(sh + 1) + p->offset;		/* XXX */

	if (p->fcode.bf_insns == NULL ||
	    bpf_filter(p->fcode.bf_insns, cp, datalen, caplen)) {
		struct pcap_pkthdr h;
		++p->md.stat.ps_recv;
		h.ts = sh->snoop_timestamp;
		h.len = datalen;
		h.caplen = caplen;
		(*callback)(user, &h, cp);
		return (1);
	}
	return (0);
}

int
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{
	register struct rawstats *rs;
	struct rawstats rawstats;

	rs = &rawstats;
	bzero((char *)rs, sizeof(*rs));
	if (ioctl(p->fd, SIOCRAWSTATS, (char *)rs) < 0) {
		sprintf(p->errbuf, "SIOCRAWSTATS: %s", pcap_strerror(errno));
		return (-1);
	}

	p->md.stat.ps_drop =
	    rs->rs_snoop.ss_ifdrops + rs->rs_snoop.ss_sbdrops +
	    rs->rs_drain.ds_ifdrops + rs->rs_drain.ds_sbdrops;

	*ps = p->md.stat;
	return (0);
}

/* XXX can't disable promiscuous */
pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	int fd;
	struct sockaddr_raw sr;
	struct snoopfilter sf;
	u_int v;
	pcap_t *p;

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		return (NULL);
	}
	bzero((char *)p, sizeof(*p));
	fd = socket(PF_RAW, SOCK_RAW, RAWPROTO_SNOOP);
	if (fd < 0) {
		sprintf(ebuf, "snoop socket: %s", pcap_strerror(errno));
		goto bad;
	}
	p->fd = fd;
	bzero((char *)&sr, sizeof(sr));
	sr.sr_family = AF_RAW;
	(void)strncpy(sr.sr_ifname, device, sizeof(sr.sr_ifname));
	if (bind(fd, (struct sockaddr *)&sr, sizeof(sr))) {
		sprintf(ebuf, "snoop bind: %s", pcap_strerror(errno));
		goto bad;
	}
	bzero((char *)&sf, sizeof(sf));
	if (ioctl(fd, SIOCADDSNOOP, &sf) < 0) {
		sprintf(ebuf, "SIOCADDSNOOP: %s", pcap_strerror(errno));
		goto bad;
	}
	v = 64 * 1024;
	(void)setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&v, sizeof(v));
	if (ioctl(fd, SIOCSNOOPLEN, &snaplen) < 0) {
		sprintf(ebuf, "SIOCSNOOPLEN: %s", pcap_strerror(errno));
		goto bad;
	}
	p->snapshot = snaplen;
	v = 1;
	if (ioctl(fd, SIOCSNOOPING, &v) < 0) {
		sprintf(ebuf, "SIOCSNOOPING: %s", pcap_strerror(errno));
		goto bad;
	}
	/*
	 * XXX hack - map device name to link layer type
	 */
	if (strncmp("et", device, 2) == 0 ||	/* Challenge 10 Mbit */
	    strncmp("ec", device, 2) == 0 ||	/* Indigo/Indy 10 Mbit,
						   O2 10/100 */
	    strncmp("ef", device, 2) == 0 ||	/* O200/2000 10/100 Mbit */
	    strncmp("gfe", device, 3) == 0 ||	/* GIO 100 Mbit */
	    strncmp("fxp", device, 3) == 0 ||	/* Challenge VME Enet */
	    strncmp("ep", device, 2) == 0 ||	/* Challenge 8x10 Mbit EPLEX */
	    strncmp("vfe", device, 3) == 0 ||	/* Challenge VME 100Mbit */
	    strncmp("fa", device, 2) == 0 ||
	    strncmp("qaa", device, 3) == 0) {
		p->linktype = DLT_EN10MB;
		p->offset = RAW_HDRPAD(sizeof(struct ether_header));
	} else if (strncmp("ipg", device, 3) == 0 ||
		   strncmp("rns", device, 3) == 0 ||	/* O2/200/2000 FDDI */
		   strncmp("xpi", device, 3) == 0) {
		p->linktype = DLT_FDDI;
		p->offset = 3;				/* XXX yeah? */
	} else if (strncmp("ppp", device, 3) == 0) {
		p->linktype = DLT_RAW;
	} else if (strncmp("lo", device, 2) == 0) {
		p->linktype = DLT_NULL;
	} else {
		sprintf(ebuf, "snoop: unknown physical layer type");
		goto bad;
	}

	p->bufsize = 4096;				/* XXX */
	p->buffer = (u_char *)malloc(p->bufsize);
	if (p->buffer == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		goto bad;
	}

	return (p);
 bad:
	(void)close(fd);
	free(p);
	return (NULL);
}

int
pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{

	p->fcode = *fp;
	return (0);
}
