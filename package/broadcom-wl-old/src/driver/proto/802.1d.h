/*
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Fundamental types and constants relating to 802.1D
 *
 */

#ifndef _802_1_D_
#define _802_1_D_

/* 802.1D priority defines */
#define	PRIO_8021D_NONE		2	/* None = - */
#define	PRIO_8021D_BK		1	/* BK - Background */
#define	PRIO_8021D_BE		0	/* BE - Best-effort */
#define	PRIO_8021D_EE		3	/* EE - Excellent-effort */
#define	PRIO_8021D_CL		4	/* CL - Controlled Load */
#define	PRIO_8021D_VI		5	/* Vi - Video */
#define	PRIO_8021D_VO		6	/* Vo - Voice */
#define	PRIO_8021D_NC		7	/* NC - Network Control */
#define	MAXPRIO			7	/* 0-7 */
#define NUMPRIO			(MAXPRIO + 1)

#define ALLPRIO		-1	/* All prioirty */

/* Converts prio to precedence since the numerical value of
 * PRIO_8021D_BE and PRIO_8021D_NONE are swapped.
 */
#define PRIO2PREC(prio) \
	(((prio) == PRIO_8021D_NONE || (prio) == PRIO_8021D_BE) ? ((prio^2)) : (prio))

#endif /* _802_1_D__ */
