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
 *
 * This code contributed by Atanu Ghosh (atanu@cs.ucl.ac.uk),
 * University College London.
 */

/*
 * Packet capture routine for dlpi under SunOS 5
 *
 * Notes:
 *
 *    - Apparently the DLIOCRAW ioctl() is specific to SunOS.
 *
 *    - There is a bug in bufmod(7) such that setting the snapshot
 *      length results in data being left of the front of the packet.
 *
 *    - It might be desirable to use pfmod(7) to filter packets in the
 *      kernel.
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/pcap-dlpi.c,v 1.1 1999/08/26 10:05:23 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/time.h>
#ifdef HAVE_SYS_BUFMOD_H
#include <sys/bufmod.h>
#endif
#include <sys/dlpi.h>
#ifdef HAVE_SYS_DLPI_EXT_H
#include <sys/dlpi_ext.h>
#endif
#ifdef HAVE_HPUX9
#include <sys/socket.h>
#endif
#ifdef DL_HP_PPA_ACK_OBS
#include <sys/stat.h>
#endif
#include <sys/stream.h>
#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
#include <sys/systeminfo.h>
#endif

#ifdef HAVE_HPUX9
#include <net/if.h>
#endif

#include <ctype.h>
#ifdef HAVE_HPUX9
#include <nlist.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>

#include "pcap-int.h"

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#ifndef PCAP_DEV_PREFIX
#define PCAP_DEV_PREFIX "/dev"
#endif

#define	MAXDLBUF	8192

/* Forwards */
static int dlattachreq(int, bpf_u_int32, char *);
static int dlbindack(int, char *, char *);
static int dlbindreq(int, bpf_u_int32, char *);
static int dlinfoack(int, char *, char *);
static int dlinforeq(int, char *);
static int dlokack(int, const char *, char *, char *);
static int recv_ack(int, int, const char *, char *, char *);
static int dlpromisconreq(int, bpf_u_int32, char *);
#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
static char *get_release(bpf_u_int32 *, bpf_u_int32 *, bpf_u_int32 *);
#endif
static int send_request(int, char *, int, char *, char *);
#ifdef HAVE_SYS_BUFMOD_H
static int strioctl(int, int, int, char *);
#endif
#ifdef HAVE_HPUX9
static int dlpi_kread(int, off_t, void *, u_int, char *);
#endif
#ifdef HAVE_DEV_DLPI
static int get_dlpi_ppa(int, const char *, int, char *);
#endif

int
pcap_stats(pcap_t *p, struct pcap_stat *ps)
{

	*ps = p->md.stat;
	return (0);
}

/* XXX Needed by HP-UX (at least) */
static bpf_u_int32 ctlbuf[MAXDLBUF];
static struct strbuf ctl = {
	MAXDLBUF,
	0,
	(char *)ctlbuf
};

int
pcap_read(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
	register int cc, n, caplen, origlen;
	register u_char *bp, *ep, *pk;
	register struct bpf_insn *fcode;
#ifdef HAVE_SYS_BUFMOD_H
	register struct sb_hdr *sbp;
#ifdef LBL_ALIGN
	struct sb_hdr sbhdr;
#endif
#endif
	int flags;
	struct strbuf data;
	struct pcap_pkthdr pkthdr;

	flags = 0;
	cc = p->cc;
	if (cc == 0) {
		data.buf = (char *)p->buffer + p->offset;
		data.maxlen = MAXDLBUF;
		data.len = 0;
		do {
			if (getmsg(p->fd, &ctl, &data, &flags) < 0) {
				/* Don't choke when we get ptraced */
				if (errno == EINTR) {
					cc = 0;
					continue;
				}
				strcpy(p->errbuf, pcap_strerror(errno));
				return (-1);
			}
			cc = data.len;
		} while (cc == 0);
		bp = p->buffer + p->offset;
	} else
		bp = p->bp;

	/* Loop through packets */
	fcode = p->fcode.bf_insns;
	ep = bp + cc;
	n = 0;
#ifdef HAVE_SYS_BUFMOD_H
	while (bp < ep) {
#ifdef LBL_ALIGN
		if ((long)bp & 3) {
			sbp = &sbhdr;
			memcpy(sbp, bp, sizeof(*sbp));
		} else
#endif
			sbp = (struct sb_hdr *)bp;
		p->md.stat.ps_drop += sbp->sbh_drops;
		pk = bp + sizeof(*sbp);
		bp += sbp->sbh_totlen;
		origlen = sbp->sbh_origlen;
		caplen = sbp->sbh_msglen;
#else
		origlen = cc;
		caplen = min(p->snapshot, cc);
		pk = bp;
		bp += caplen;
#endif
		++p->md.stat.ps_recv;
		if (bpf_filter(fcode, pk, origlen, caplen)) {
#ifdef HAVE_SYS_BUFMOD_H
			pkthdr.ts = sbp->sbh_timestamp;
#else
			(void)gettimeofday(&pkthdr.ts, NULL);
#endif
			pkthdr.len = origlen;
			pkthdr.caplen = caplen;
			/* Insure caplen does not exceed snapshot */
			if (pkthdr.caplen > p->snapshot)
				pkthdr.caplen = p->snapshot;
			(*callback)(user, &pkthdr, pk);
			if (++n >= cnt && cnt >= 0) {
				p->cc = ep - bp;
				p->bp = bp;
				return (n);
			}
		}
#ifdef HAVE_SYS_BUFMOD_H
	}
#endif
	p->cc = 0;
	return (n);
}

pcap_t *
pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
{
	register char *cp;
	char *eos;
	register pcap_t *p;
	register int ppa;
	register dl_info_ack_t *infop;
#ifdef HAVE_SYS_BUFMOD_H
	bpf_u_int32 ss, flag;
#ifdef HAVE_SOLARIS
	register char *release;
	bpf_u_int32 osmajor, osminor, osmicro;
#endif
#endif
	bpf_u_int32 buf[MAXDLBUF];
	char dname[100];
#ifndef HAVE_DEV_DLPI
	char dname2[100];
#endif

	p = (pcap_t *)malloc(sizeof(*p));
	if (p == NULL) {
		strcpy(ebuf, pcap_strerror(errno));
		return (NULL);
	}
	memset(p, 0, sizeof(*p));

	/*
	** Determine device and ppa
	*/
	cp = strpbrk(device, "0123456789");
	if (cp == NULL) {
		sprintf(ebuf, "%s missing unit number", device);
		goto bad;
	}
	ppa = strtol(cp, &eos, 10);
	if (*eos != '\0') {
		sprintf(ebuf, "%s bad unit number", device);
		goto bad;
	}

	if (*device == '/')
		strcpy(dname, device);
	else
		sprintf(dname, "%s/%s", PCAP_DEV_PREFIX, device);
#ifdef HAVE_DEV_DLPI
	/* Map network device to /dev/dlpi unit */
	cp = "/dev/dlpi";
	if ((p->fd = open(cp, O_RDWR)) < 0) {
		sprintf(ebuf, "%s: %s", cp, pcap_strerror(errno));
		goto bad;
	}
	/* Map network interface to /dev/dlpi unit */
	ppa = get_dlpi_ppa(p->fd, dname, ppa, ebuf);
	if (ppa < 0)
		goto bad;
#else
	/* Try device without unit number */
	strcpy(dname2, dname);
	cp = strchr(dname, *cp);
	*cp = '\0';
	if ((p->fd = open(dname, O_RDWR)) < 0) {
		if (errno != ENOENT) {
			sprintf(ebuf, "%s: %s", dname, pcap_strerror(errno));
			goto bad;
		}

		/* Try again with unit number */
		if ((p->fd = open(dname2, O_RDWR)) < 0) {
			sprintf(ebuf, "%s: %s", dname2, pcap_strerror(errno));
			goto bad;
		}
		/* XXX Assume unit zero */
		ppa = 0;
	}
#endif

	p->snapshot = snaplen;

	/*
	** Attach if "style 2" provider
	*/
	if (dlinforeq(p->fd, ebuf) < 0 ||
	    dlinfoack(p->fd, (char *)buf, ebuf) < 0)
		goto bad;
	infop = &((union DL_primitives *)buf)->info_ack;
	if (infop->dl_provider_style == DL_STYLE2 &&
	    (dlattachreq(p->fd, ppa, ebuf) < 0 ||
	    dlokack(p->fd, "attach", (char *)buf, ebuf) < 0))
		goto bad;
	/*
	** Bind (defer if using HP-UX 9 or HP-UX 10.20, totally skip if
	** using SINIX)
	*/
#if !defined(HAVE_HPUX9) && !defined(HAVE_HPUX10_20) && !defined(sinix)
	if (dlbindreq(p->fd, 0, ebuf) < 0 ||
	    dlbindack(p->fd, (char *)buf, ebuf) < 0)
		goto bad;
#endif

	if (promisc) {
		/*
		** Enable promiscuous
		*/
		if (dlpromisconreq(p->fd, DL_PROMISC_PHYS, ebuf) < 0 ||
		    dlokack(p->fd, "promisc_phys", (char *)buf, ebuf) < 0)
			goto bad;

		/*
		** Try to enable multicast (you would have thought
		** promiscuous would be sufficient). (Skip if using
		** HP-UX or SINIX)
		*/
#if !defined(__hpux) && !defined(sinix)
		if (dlpromisconreq(p->fd, DL_PROMISC_MULTI, ebuf) < 0 ||
		    dlokack(p->fd, "promisc_multi", (char *)buf, ebuf) < 0)
			fprintf(stderr,
			    "WARNING: DL_PROMISC_MULTI failed (%s)\n", ebuf);
#endif
	}
	/*
	** Try to enable sap (when not in promiscuous mode when using
	** using HP-UX and never under SINIX)
	*/
#ifndef sinix
	if (
#ifdef __hpux
	    !promisc &&
#endif
	    (dlpromisconreq(p->fd, DL_PROMISC_SAP, ebuf) < 0 ||
	    dlokack(p->fd, "promisc_sap", (char *)buf, ebuf) < 0)) {
		/* Not fatal if promisc since the DL_PROMISC_PHYS worked */
		if (promisc)
			fprintf(stderr,
			    "WARNING: DL_PROMISC_SAP failed (%s)\n", ebuf);
		else
			goto bad;
	}
#endif

	/*
	** HP-UX 9 and HP-UX 10.20 must bind after setting promiscuous
	** options)
	*/
#if defined(HAVE_HPUX9) || defined(HAVE_HPUX10_20)
	if (dlbindreq(p->fd, 0, ebuf) < 0 ||
	    dlbindack(p->fd, (char *)buf, ebuf) < 0)
		goto bad;
#endif

	/*
	** Determine link type
	*/
	if (dlinforeq(p->fd, ebuf) < 0 ||
	    dlinfoack(p->fd, (char *)buf, ebuf) < 0)
		goto bad;

	infop = &((union DL_primitives *)buf)->info_ack;
	switch (infop->dl_mac_type) {

	case DL_CSMACD:
	case DL_ETHER:
		p->linktype = DLT_EN10MB;
		p->offset = 2;
		break;

	case DL_FDDI:
		p->linktype = DLT_FDDI;
		p->offset = 3;
		break;

	default:
		sprintf(ebuf, "unknown mac type 0x%lu", infop->dl_mac_type);
		goto bad;
	}

#ifdef	DLIOCRAW
	/*
	** This is a non standard SunOS hack to get the ethernet header.
	*/
	if (strioctl(p->fd, DLIOCRAW, 0, NULL) < 0) {
		sprintf(ebuf, "DLIOCRAW: %s", pcap_strerror(errno));
		goto bad;
	}
#endif

#ifdef HAVE_SYS_BUFMOD_H
	/*
	** Another non standard call to get the data nicely buffered
	*/
	if (ioctl(p->fd, I_PUSH, "bufmod") != 0) {
		sprintf(ebuf, "I_PUSH bufmod: %s", pcap_strerror(errno));
		goto bad;
	}

	/*
	** Now that the bufmod is pushed lets configure it.
	**
	** There is a bug in bufmod(7). When dealing with messages of
	** less than snaplen size it strips data from the beginning not
	** the end.
	**
	** This bug is supposed to be fixed in 5.3.2. Also, there is a
	** patch available. Ask for bugid 1149065.
	*/
	ss = snaplen;
#ifdef HAVE_SOLARIS
	release = get_release(&osmajor, &osminor, &osmicro);
	if (osmajor == 5 && (osminor <= 2 || (osminor == 3 && osmicro < 2)) &&
	    getenv("BUFMOD_FIXED") == NULL) {
		fprintf(stderr,
		"WARNING: bufmod is broken in SunOS %s; ignoring snaplen.\n",
		    release);
		ss = 0;
	}
#endif
	if (ss > 0 &&
	    strioctl(p->fd, SBIOCSSNAP, sizeof(ss), (char *)&ss) != 0) {
		sprintf(ebuf, "SBIOCSSNAP: %s", pcap_strerror(errno));
		goto bad;
	}

	/*
	** Set up the bufmod flags
	*/
	if (strioctl(p->fd, SBIOCGFLAGS, sizeof(flag), (char *)&flag) < 0) {
		sprintf(ebuf, "SBIOCGFLAGS: %s", pcap_strerror(errno));
		goto bad;
	}
	flag |= SB_NO_DROPS;
	if (strioctl(p->fd, SBIOCSFLAGS, sizeof(flag), (char *)&flag) != 0) {
		sprintf(ebuf, "SBIOCSFLAGS: %s", pcap_strerror(errno));
		goto bad;
	}
	/*
	** Set up the bufmod timeout
	*/
	if (to_ms != 0) {
		struct timeval to;

		to.tv_sec = to_ms / 1000;
		to.tv_usec = (to_ms * 1000) % 1000000;
		if (strioctl(p->fd, SBIOCSTIME, sizeof(to), (char *)&to) != 0) {
			sprintf(ebuf, "SBIOCSTIME: %s", pcap_strerror(errno));
			goto bad;
		}
	}
#endif

	/*
	** As the last operation flush the read side.
	*/
	if (ioctl(p->fd, I_FLUSH, FLUSHR) != 0) {
		sprintf(ebuf, "FLUSHR: %s", pcap_strerror(errno));
		goto bad;
	}
	/* Allocate data buffer */
	p->bufsize = MAXDLBUF * sizeof(bpf_u_int32);
	p->buffer = (u_char *)malloc(p->bufsize + p->offset);

	return (p);
bad:
	free(p);
	return (NULL);
}

int
pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{

	p->fcode = *fp;
	return (0);
}

static int
send_request(int fd, char *ptr, int len, char *what, char *ebuf)
{
	struct	strbuf	ctl;
	int	flags;

	ctl.maxlen = 0;
	ctl.len = len;
	ctl.buf = ptr;

	flags = 0;
	if (putmsg(fd, &ctl, (struct strbuf *) NULL, flags) < 0) {
		sprintf(ebuf, "send_request: putmsg \"%s\": %s",
		    what, pcap_strerror(errno));
		return (-1);
	}
	return (0);
}

static int
recv_ack(int fd, int size, const char *what, char *bufp, char *ebuf)
{
	union	DL_primitives	*dlp;
	struct	strbuf	ctl;
	int	flags;

	ctl.maxlen = MAXDLBUF;
	ctl.len = 0;
	ctl.buf = bufp;

	flags = 0;
	if (getmsg(fd, &ctl, (struct strbuf*)NULL, &flags) < 0) {
		sprintf(ebuf, "recv_ack: %s getmsg: %s",
		    what, pcap_strerror(errno));
		return (-1);
	}

	dlp = (union DL_primitives *) ctl.buf;
	switch (dlp->dl_primitive) {

	case DL_INFO_ACK:
	case DL_BIND_ACK:
	case DL_OK_ACK:
#ifdef DL_HP_PPA_ACK
	case DL_HP_PPA_ACK:
#endif

		/* These are OK */
		break;

	case DL_ERROR_ACK:
		switch (dlp->error_ack.dl_errno) {

		case DL_BADPPA:
			sprintf(ebuf, "recv_ack: %s bad ppa (device unit)",
			    what);
			break;


		case DL_SYSERR:
			sprintf(ebuf, "recv_ack: %s: %s",
			    what, pcap_strerror(dlp->error_ack.dl_unix_errno));
			break;

		case DL_UNSUPPORTED:
			sprintf(ebuf,
			    "recv_ack: %s: Service not supplied by provider",
			    what);
			break;

		default:
			sprintf(ebuf, "recv_ack: %s error 0x%x",
			    what, (bpf_u_int32)dlp->error_ack.dl_errno);
			break;
		}
		return (-1);

	default:
		sprintf(ebuf, "recv_ack: %s unexpected primitive ack 0x%x ",
		    what, (bpf_u_int32)dlp->dl_primitive);
		return (-1);
	}

	if (ctl.len < size) {
		sprintf(ebuf, "recv_ack: %s ack too small (%d < %d)",
		    what, ctl.len, size);
		return (-1);
	}
	return (ctl.len);
}

static int
dlattachreq(int fd, bpf_u_int32 ppa, char *ebuf)
{
	dl_attach_req_t	req;

	req.dl_primitive = DL_ATTACH_REQ;
	req.dl_ppa = ppa;

	return (send_request(fd, (char *)&req, sizeof(req), "attach", ebuf));
}

static int
dlbindreq(int fd, bpf_u_int32 sap, char *ebuf)
{

	dl_bind_req_t	req;

	memset((char *)&req, 0, sizeof(req));
	req.dl_primitive = DL_BIND_REQ;
#ifdef DL_HP_RAWDLS
	req.dl_max_conind = 1;			/* XXX magic number */
	/* 22 is INSAP as per the HP-UX DLPI Programmer's Guide */
	req.dl_sap = 22;
	req.dl_service_mode = DL_HP_RAWDLS;
#else
	req.dl_sap = sap;
#ifdef DL_CLDLS
	req.dl_service_mode = DL_CLDLS;
#endif
#endif

	return (send_request(fd, (char *)&req, sizeof(req), "bind", ebuf));
}

static int
dlbindack(int fd, char *bufp, char *ebuf)
{

	return (recv_ack(fd, DL_BIND_ACK_SIZE, "bind", bufp, ebuf));
}

static int
dlpromisconreq(int fd, bpf_u_int32 level, char *ebuf)
{
	dl_promiscon_req_t req;

	req.dl_primitive = DL_PROMISCON_REQ;
	req.dl_level = level;

	return (send_request(fd, (char *)&req, sizeof(req), "promiscon", ebuf));
}

static int
dlokack(int fd, const char *what, char *bufp, char *ebuf)
{

	return (recv_ack(fd, DL_OK_ACK_SIZE, what, bufp, ebuf));
}


static int
dlinforeq(int fd, char *ebuf)
{
	dl_info_req_t req;

	req.dl_primitive = DL_INFO_REQ;

	return (send_request(fd, (char *)&req, sizeof(req), "info", ebuf));
}

static int
dlinfoack(int fd, char *bufp, char *ebuf)
{

	return (recv_ack(fd, DL_INFO_ACK_SIZE, "info", bufp, ebuf));
}

#ifdef HAVE_SYS_BUFMOD_H
static int
strioctl(int fd, int cmd, int len, char *dp)
{
	struct strioctl str;
	int rc;

	str.ic_cmd = cmd;
	str.ic_timout = -1;
	str.ic_len = len;
	str.ic_dp = dp;
	rc = ioctl(fd, I_STR, &str);

	if (rc < 0)
		return (rc);
	else
		return (str.ic_len);
}
#endif

#if defined(HAVE_SOLARIS) && defined(HAVE_SYS_BUFMOD_H)
static char *
get_release(bpf_u_int32 *majorp, bpf_u_int32 *minorp, bpf_u_int32 *microp)
{
	char *cp;
	static char buf[32];

	*majorp = 0;
	*minorp = 0;
	*microp = 0;
	if (sysinfo(SI_RELEASE, buf, sizeof(buf)) < 0)
		return ("?");
	cp = buf;
	if (!isdigit(*cp))
		return (buf);
	*majorp = strtol(cp, &cp, 10);
	if (*cp++ != '.')
		return (buf);
	*minorp =  strtol(cp, &cp, 10);
	if (*cp++ != '.')
		return (buf);
	*microp =  strtol(cp, &cp, 10);
	return (buf);
}
#endif

#ifdef DL_HP_PPA_ACK_OBS
/*
 * Under HP-UX 10, we can ask for the ppa
 */


/* Determine ppa number that specifies ifname */
static int
get_dlpi_ppa(register int fd, register const char *device, register int unit,
    register char *ebuf)
{
	register dl_hp_ppa_ack_t *ap;
	register dl_hp_ppa_info_t *ip;
	register int i;
	register u_long majdev;
	dl_hp_ppa_req_t	req;
	struct stat statbuf;
	bpf_u_int32 buf[MAXDLBUF];

	if (stat(device, &statbuf) < 0) {
		sprintf(ebuf, "stat: %s: %s", device, pcap_strerror(errno));
		return (-1);
	}
	majdev = major(statbuf.st_rdev);

	memset((char *)&req, 0, sizeof(req));
	req.dl_primitive = DL_HP_PPA_REQ;

	memset((char *)buf, 0, sizeof(buf));
	if (send_request(fd, (char *)&req, sizeof(req), "hpppa", ebuf) < 0 ||
	    recv_ack(fd, DL_HP_PPA_ACK_SIZE, "hpppa", (char *)buf, ebuf) < 0)
		return (-1);

	ap = (dl_hp_ppa_ack_t *)buf;
	ip = (dl_hp_ppa_info_t *)((u_char *)ap + ap->dl_offset);

        for(i = 0; i < ap->dl_count; i++) {
                if (ip->dl_mjr_num == majdev && ip->dl_instance_num == unit)
                        break;

                ip = (dl_hp_ppa_info_t *)((u_char *)ip + ip->dl_next_offset);
        }
        if (i == ap->dl_count) {
                sprintf(ebuf, "can't find PPA for %s", device);
		return (-1);
        }
        if (ip->dl_hdw_state == HDW_DEAD) {
                sprintf(ebuf, "%s: hardware state: DOWN\n", device);
		return (-1);
        }
        return ((int)ip->dl_ppa);
}
#endif

#ifdef HAVE_HPUX9
/*
 * Under HP-UX 9, there is no good way to determine the ppa.
 * So punt and read it from /dev/kmem.
 */
static struct nlist nl[] = {
#define NL_IFNET 0
	{ "ifnet" },
	{ "" }
};

static char path_vmunix[] = "/hp-ux";

/* Determine ppa number that specifies ifname */
static int
get_dlpi_ppa(register int fd, register const char *ifname, register int unit,
    register char *ebuf)
{
	register const char *cp;
	register int kd;
	void *addr;
	struct ifnet ifnet;
	char if_name[sizeof(ifnet.if_name)], tifname[32];

	cp = strrchr(ifname, '/');
	if (cp != NULL)
		ifname = cp + 1;
	if (nlist(path_vmunix, &nl) < 0) {
		sprintf(ebuf, "nlist %s failed", path_vmunix);
		return (-1);
	}
	if (nl[NL_IFNET].n_value == 0) {
		sprintf(ebuf, "could't find %s kernel symbol",
		    nl[NL_IFNET].n_name);
		return (-1);
	}
	kd = open("/dev/kmem", O_RDONLY);
	if (kd < 0) {
		sprintf(ebuf, "kmem open: %s", pcap_strerror(errno));
		return (-1);
	}
	if (dlpi_kread(kd, nl[NL_IFNET].n_value,
	    &addr, sizeof(addr), ebuf) < 0) {
		close(kd);
		return (-1);
	}
	for (; addr != NULL; addr = ifnet.if_next) {
		if (dlpi_kread(kd, (off_t)addr,
		    &ifnet, sizeof(ifnet), ebuf) < 0 ||
		    dlpi_kread(kd, (off_t)ifnet.if_name,
		    if_name, sizeof(if_name), ebuf) < 0) {
			(void)close(kd);
			return (-1);
		}
		sprintf(tifname, "%.*s%d",
		    (int)sizeof(if_name), if_name, ifnet.if_unit);
		if (strcmp(tifname, ifname) == 0)
			return (ifnet.if_index);
	}

	sprintf(ebuf, "Can't find %s", ifname);
	return (-1);
}

static int
dlpi_kread(register int fd, register off_t addr,
    register void *buf, register u_int len, register char *ebuf)
{
	register int cc;

	if (lseek(fd, addr, SEEK_SET) < 0) {
		sprintf(ebuf, "lseek: %s", pcap_strerror(errno));
		return (-1);
	}
	cc = read(fd, buf, len);
	if (cc < 0) {
		sprintf(ebuf, "read: %s", pcap_strerror(errno));
		return (-1);
	} else if (cc != len) {
		sprintf(ebuf, "short read (%d != %d)", cc, len);
		return (-1);
	}
	return (cc);
}
#endif
