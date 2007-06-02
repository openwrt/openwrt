/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1998
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
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-bpf.c,v 1.1 1999/08/26 10:05:23 johana Exp $ (LBL)";
#endif

#include <sys/param.h>			/* optionally get BSD define */
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <net/if.h>

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
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
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{
	struct bpf_stat s;

	if (ioctl(p->fd, BIOCGSTATS, (caddr_t)&s) < 0) {
		sprintf(p->errbuf, "BIOCGSTATS: %s", pcap_strerror(errno));
		return (-1);
	}

	ps->ps_recv = s.bs_recv;
	ps->ps_drop = s.bs_drop;
	return (0);
}

int
pcap_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	int cc;
	int n = 0;
	register u_char *bp, *ep;

 again:
	cc = p->cc;
	if (p->cc == 0) {
		cc = read(p->fd, (char *)p->buffer, p->bufsize);
		if (cc < 0) {
			/* Don't choke when we get ptraced */
			switch (errno) {

			case EINTR:
				goto again;

			case EWOULDBLOCK:
				return (0);
#if defined(sun) && !defined(BSD)
			/*
			 * Due to a SunOS bug, after 2^31 bytes, the kernel
			 * file offset overflows and read fails with EINVAL.
			 * The lseek() to 0 will fix things.
			 */
			case EINVAL:
				if (lseek(p->fd, 0L, SEEK_CUR) +
				    p->bufsize < 0) {
					(void)lseek(p->fd, 0L, SEEK_SET);
					goto again;
				}
				/* fall through */
#endif
			}
			sprintf(p->errbuf, "read: %s", pcap_strerror(errno));
			return (-1);
		}
		bp = p->buffer;
	} else
		bp = p->bp;

	/*
	 * Loop through each packet.
	 */
#define bhp ((struct bpf_hdr *)bp)
	ep = bp + cc;
	while (bp < ep) {
		register int caplen, hdrlen;
		caplen = bhp->bh_caplen;
		hdrlen = bhp->bh_hdrlen;
		/*
		 * XXX A bpf_hdr matches a pcap_pkthdr.
		 */
		(*callback)(user, (struct pcap_pkthdr*)bp, bp + hdrlen);
		bp += BPF_WORDALIGN(caplen + hdrlen);
		if (++n >= cnt && cnt > 0) {
			p->bp = bp;
			p->cc = ep - bp;
			return (n);
		}
	}
#undef bhp
	p->cc = 0;
	return (n);
}

static inline int
bpf_open(pcap_t *p, char *errbuf)
{
	int fd;
	int n = 0;
	char device[sizeof "/dev/bpf000"];

	/*
	 * Go through all the minors and find one that isn't in use.
	 */
	do {
		(void)sprintf(device, "/dev/bpf%d", n++);
		fd = open(device, O_RDONLY);
	} while (fd < 0 && errno == EBUSY);

	/*
	 * XXX better message for all minors used
	 */
	if (fd < 0)
		sprintf(errbuf, "%s: %s", device, pcap_strerror(errno));

	return (fd);
}

pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	int fd;
	struct ifreq ifr;
	struct bpf_version bv;
	u_int v;
	pcap_t *p;

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		sprintf(ebuf, "malloc: %s", pcap_strerror(errno));
		return (NULL);
	}
	bzero(p, sizeof(*p));
	fd = bpf_open(p, ebuf);
	if (fd < 0)
		goto bad;

	p->fd = fd;
	p->snapshot = snaplen;

	if (ioctl(fd, BIOCVERSION, (caddr_t)&bv) < 0) {
		sprintf(ebuf, "BIOCVERSION: %s", pcap_strerror(errno));
		goto bad;
	}
	if (bv.bv_major != BPF_MAJOR_VERSION ||
	    bv.bv_minor < BPF_MINOR_VERSION) {
		sprintf(ebuf, "kernel bpf filter out of date");
		goto bad;
	}
	v = 32768;	/* XXX this should be a user-accessible hook */
	/* Ignore the return value - this is because the call fails on
	 * BPF systems that don't have kernel malloc.  And if the call
	 * fails, it's no big deal, we just continue to use the standard
	 * buffer size.
	 */
	(void) ioctl(fd, BIOCSBLEN, (caddr_t)&v);

	(void)strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(fd, BIOCSETIF, (caddr_t)&ifr) < 0) {
		sprintf(ebuf, "%s: %s", device, pcap_strerror(errno));
		goto bad;
	}
	/* Get the data link layer type. */
	if (ioctl(fd, BIOCGDLT, (caddr_t)&v) < 0) {
		sprintf(ebuf, "BIOCGDLT: %s", pcap_strerror(errno));
		goto bad;
	}
#if _BSDI_VERSION - 0 >= 199510
	/* The SLIP and PPP link layer header changed in BSD/OS 2.1 */
	switch (v) {

	case DLT_SLIP:
		v = DLT_SLIP_BSDOS;
		break;

	case DLT_PPP:
		v = DLT_PPP_BSDOS;
		break;
	}
#endif
	p->linktype = v;

	/* set timeout */
	if (to_ms != 0) {
		struct timeval to;
		to.tv_sec = to_ms / 1000;
		to.tv_usec = (to_ms * 1000) % 1000000;
		if (ioctl(p->fd, BIOCSRTIMEOUT, (caddr_t)&to) < 0) {
			sprintf(ebuf, "BIOCSRTIMEOUT: %s",
				pcap_strerror(errno));
			goto bad;
		}
	}
	if (promisc)
		/* set promiscuous mode, okay if it fails */
		(void)ioctl(p->fd, BIOCPROMISC, NULL);

	if (ioctl(fd, BIOCGBLEN, (caddr_t)&v) < 0) {
		sprintf(ebuf, "BIOCGBLEN: %s", pcap_strerror(errno));
		goto bad;
	}
	p->bufsize = v;
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
	if (p->sf.rfile != NULL)
		p->fcode = *fp;
	else if (ioctl(p->fd, BIOCSETF, (caddr_t)fp) < 0) {
		sprintf(p->errbuf, "BIOCSETF: %s", pcap_strerror(errno));
		return (-1);
	}
	return (0);
}
