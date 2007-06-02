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
 *
 * Modifications made to accommodate the new SunOS4.0 NIT facility by
 * Micky Liu, micky@cunixc.cc.columbia.edu, Columbia University in May, 1989.
 * This module now handles the STREAMS based NIT.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-snit.c,v 1.1 1999/08/26 10:05:25 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/dir.h>
#include <sys/fcntlcom.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stropts.h>

#include <net/if.h>
#include <net/nit.h>
#include <net/nit_if.h>
#include <net/nit_pf.h>
#include <net/nit_buf.h>

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
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
	register struct nit_bufhdr *hdrp;
	register struct nit_iftime *ntp;
	register struct nit_iflen *nlp;
	register struct nit_ifdrops *ndp;
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
	 * loop through each snapshot in the chunk
	 */
	n = 0;
	ep = bp + cc;
	while (bp < ep) {
		++p->md.stat.ps_recv;
		cp = bp;

		/* get past NIT buffer  */
		hdrp = (struct nit_bufhdr *)cp;
		cp += sizeof(*hdrp);

		/* get past NIT timer   */
		ntp = (struct nit_iftime *)cp;
		cp += sizeof(*ntp);

		ndp = (struct nit_ifdrops *)cp;
		p->md.stat.ps_drop = ndp->nh_drops;
		cp += sizeof *ndp;

		/* get past packet len  */
		nlp = (struct nit_iflen *)cp;
		cp += sizeof(*nlp);

		/* next snapshot        */
		bp += hdrp->nhb_totlen;

		caplen = nlp->nh_pktlen;
		if (caplen > p->snapshot)
			caplen = p->snapshot;

		if (bpf_filter(fcode, cp, nlp->nh_pktlen, caplen)) {
			struct pcap_pkthdr h;
			h.ts = ntp->nh_timestamp;
			h.len = nlp->nh_pktlen;
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
	bpf_u_int32 flags;
	struct strioctl si;
	struct timeval timeout;

	si.ic_timout = INFTIM;
	if (to_ms != 0) {
		timeout.tv_sec = to_ms / 1000;
		timeout.tv_usec = (to_ms * 1000) % 1000000;
		si.ic_cmd = NIOCSTIME;
		si.ic_len = sizeof(timeout);
		si.ic_dp = (char *)&timeout;
		if (ioctl(fd, I_STR, (char *)&si) < 0) {
			sprintf(ebuf, "NIOCSTIME: %s", pcap_strerror(errno));
			return (-1);
		}
	}
	flags = NI_TIMESTAMP | NI_LEN | NI_DROPS;
	if (promisc)
		flags |= NI_PROMISC;
	si.ic_cmd = NIOCSFLAGS;
	si.ic_len = sizeof(flags);
	si.ic_dp = (char *)&flags;
	if (ioctl(fd, I_STR, (char *)&si) < 0) {
		sprintf(ebuf, "NIOCSFLAGS: %s", pcap_strerror(errno));
		return (-1);
	}
	return (0);
}

pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	struct strioctl si;		/* struct for ioctl() */
	struct ifreq ifr;		/* interface request struct */
	int chunksize = CHUNKSIZE;
	int fd;
	static char dev[] = "/dev/nit";
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
	p->fd = fd = open(dev, O_RDONLY);
	if (fd < 0) {
		sprintf(ebuf, "%s: %s", dev, pcap_strerror(errno));
		goto bad;
	}

	/* arrange to get discrete messages from the STREAM and use NIT_BUF */
	if (ioctl(fd, I_SRDOPT, (char *)RMSGD) < 0) {
		sprintf(ebuf, "I_SRDOPT: %s", pcap_strerror(errno));
		goto bad;
	}
	if (ioctl(fd, I_PUSH, "nbuf") < 0) {
		sprintf(ebuf, "push nbuf: %s", pcap_strerror(errno));
		goto bad;
	}
	/* set the chunksize */
	si.ic_cmd = NIOCSCHUNK;
	si.ic_timout = INFTIM;
	si.ic_len = sizeof(chunksize);
	si.ic_dp = (char *)&chunksize;
	if (ioctl(fd, I_STR, (char *)&si) < 0) {
		sprintf(ebuf, "NIOCSCHUNK: %s", pcap_strerror(errno));
		goto bad;
	}

	/* request the interface */
	strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = ' ';
	si.ic_cmd = NIOCBIND;
	si.ic_len = sizeof(ifr);
	si.ic_dp = (char *)&ifr;
	if (ioctl(fd, I_STR, (char *)&si) < 0) {
		sprintf(ebuf, "NIOCBIND: %s: %s",
			ifr.ifr_name, pcap_strerror(errno));
		goto bad;
	}

	/* set the snapshot length */
	si.ic_cmd = NIOCSSNAP;
	si.ic_len = sizeof(snaplen);
	si.ic_dp = (char *)&snaplen;
	if (ioctl(fd, I_STR, (char *)&si) < 0) {
		sprintf(ebuf, "NIOCSSNAP: %s", pcap_strerror(errno));
		goto bad;
	}
	p->snapshot = snaplen;
	if (nit_setflags(p->fd, promisc, to_ms, ebuf) < 0)
		goto bad;

	(void)ioctl(fd, I_FLUSH, (char *)FLUSHR);
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
