/*
 * Driver-specific socket ioctls
 * used by BSD, Linux, and PSOS
 * Broadcom BCM44XX 10/100Mbps Ethernet Device Driver
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _etsockio_h_
#define _etsockio_h_

/* THESE MUST BE CONTIGUOUS AND CONSISTENT WITH VALUES IN ETC.H */


#if defined(linux)
#define SIOCSETCUP		(SIOCDEVPRIVATE + 0)
#define SIOCSETCDOWN		(SIOCDEVPRIVATE + 1)
#define SIOCSETCLOOP		(SIOCDEVPRIVATE + 2)
#define SIOCGETCDUMP		(SIOCDEVPRIVATE + 3)
#define SIOCSETCSETMSGLEVEL	(SIOCDEVPRIVATE + 4)
#define SIOCSETCPROMISC		(SIOCDEVPRIVATE + 5)
#define SIOCSETCTXDOWN		(SIOCDEVPRIVATE + 6)	/* obsolete */
#define SIOCSETCSPEED		(SIOCDEVPRIVATE + 7)
#define SIOCTXGEN		(SIOCDEVPRIVATE + 8)
#define SIOCGETCPHYRD		(SIOCDEVPRIVATE + 9)
#define SIOCSETCPHYWR		(SIOCDEVPRIVATE + 10)

#else	/* !linux */

#define SIOCSETCUP		_IOWR('e', 130 + 0, struct ifreq)
#define SIOCSETCDOWN		_IOWR('e', 130 + 1, struct ifreq)
#define SIOCSETCLOOP		_IOWR('e', 130 + 2, struct ifreq)
#define SIOCGETCDUMP		_IOWR('e', 130 + 3, struct ifreq)
#define SIOCSETCSETMSGLEVEL	_IOWR('e', 130 + 4, struct ifreq)
#define SIOCSETCPROMISC		_IOWR('e', 130 + 5, struct ifreq)
#define SIOCSETCTXDOWN		_IOWR('e', 130 + 6, struct ifreq)	/* obsolete */
#define SIOCSETCSPEED		_IOWR('e', 130 + 7, struct ifreq)
#define SIOCTXGEN		_IOWR('e', 130 + 8, struct ifreq)

#endif

/* arg to SIOCTXGEN */
struct txg {
	uint32 num;		/* number of frames to send */
	uint32 delay;		/* delay in microseconds between sending each */
	uint32 size;		/* size of ether frame to send */
	uchar buf[1514];	/* starting ether frame data */
};

#endif
