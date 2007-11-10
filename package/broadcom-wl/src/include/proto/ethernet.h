/*
 * From FreeBSD 2.2.7: Fundamental constants relating to ethernet.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ethernet.h,v 1.1.1.14 2006/02/27 03:43:16 honor Exp $
 */

#ifndef __NET_ETHERNET_H
#define __NET_ETHERNET_H

#ifndef _TYPEDEFS_H_
#include "typedefs.h"
#endif

/* enable structure packing */
#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#pragma pack(1)
#define	PACKED
#endif

/*
 * The number of bytes in an ethernet (MAC) address.
 */
#define	ETHER_ADDR_LEN		6

/*
 * The number of bytes in the type field.
 */
#define	ETHER_TYPE_LEN		2

/*
 * The number of bytes in the trailing CRC field.
 */
#define	ETHER_CRC_LEN		4

/*
 * The length of the combined header.
 */
#define	ETHER_HDR_LEN		(ETHER_ADDR_LEN*2+ETHER_TYPE_LEN)

/*
 * The minimum packet length.
 */
#define	ETHER_MIN_LEN		64

/*
 * The minimum packet user data length.
 */
#define	ETHER_MIN_DATA		46

/*
 * The maximum packet length.
 */
#define	ETHER_MAX_LEN		1518

/*
 * The maximum packet user data length.
 */
#define	ETHER_MAX_DATA		1500

/* ether types */
#define	ETHER_TYPE_IP		0x0800		/* IP */
#define ETHER_TYPE_ARP		0x0806		/* ARP */
#define ETHER_TYPE_8021Q	0x8100		/* 802.1Q */
#define	ETHER_TYPE_BRCM		0x886c		/* Broadcom Corp. */
#define	ETHER_TYPE_802_1X	0x888e		/* 802.1x */
#ifdef BCMWPA2
#define	ETHER_TYPE_802_1X_PREAUTH 0x88c7	/* 802.1x preauthentication */
#endif

/* Broadcom subtype follows ethertype;  First 2 bytes are reserved; Next 2 are subtype; */
#define	ETHER_BRCM_SUBTYPE_LEN	4		/* Broadcom 4 byte subtype */
#define	ETHER_BRCM_CRAM		0x1		/* Broadcom subtype cram protocol */

/* ether header */
#define ETHER_DEST_OFFSET	0		/* dest address offset */
#define ETHER_SRC_OFFSET	6		/* src address offset */
#define ETHER_TYPE_OFFSET	12		/* ether type offset */

/*
 * A macro to validate a length with
 */
#define	ETHER_IS_VALID_LEN(foo)	\
	((foo) >= ETHER_MIN_LEN && (foo) <= ETHER_MAX_LEN)


#ifndef __INCif_etherh     /* Quick and ugly hack for VxWorks */
/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct	ether_header {
	uint8	ether_dhost[ETHER_ADDR_LEN];
	uint8	ether_shost[ETHER_ADDR_LEN];
	uint16	ether_type;
} PACKED;

/*
 * Structure of a 48-bit Ethernet address.
 */
struct	ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
} PACKED;
#endif	/* !__INCif_etherh Quick and ugly hack for VxWorks */

/*
 * Takes a pointer, sets locally admininistered
 * address bit in the 48-bit Ethernet address.
 */
#define ETHER_SET_LOCALADDR(ea)	(((uint8 *)(ea))[0] = (((uint8 *)(ea))[0] | 2))

/*
 * Takes a pointer, returns true if a 48-bit multicast address
 * (including broadcast, since it is all ones)
 */
#define ETHER_ISMULTI(ea) (((uint8 *)(ea))[0] & 1)


/* compare two ethernet addresses - assumes the pointers can be referenced as shorts */
#define	ether_cmp(a, b)	(!(((short*)a)[0] == ((short*)b)[0]) | \
			 !(((short*)a)[1] == ((short*)b)[1]) | \
			 !(((short*)a)[2] == ((short*)b)[2]))

/* copy an ethernet address - assumes the pointers can be referenced as shorts */
#define	ether_copy(s, d) { \
		((short*)d)[0] = ((short*)s)[0]; \
		((short*)d)[1] = ((short*)s)[1]; \
		((short*)d)[2] = ((short*)s)[2]; }

/*
 * Takes a pointer, returns true if a 48-bit broadcast (all ones)
 */
#define ETHER_ISBCAST(ea) ((((uint8 *)(ea))[0] &		\
			    ((uint8 *)(ea))[1] &		\
			    ((uint8 *)(ea))[2] &		\
			    ((uint8 *)(ea))[3] &		\
			    ((uint8 *)(ea))[4] &		\
			    ((uint8 *)(ea))[5]) == 0xff)

static const struct ether_addr ether_bcast = {{255, 255, 255, 255, 255, 255}};

/*
 * Takes a pointer, returns true if a 48-bit null address (all zeros)
 */
#define ETHER_ISNULLADDR(ea) ((((uint8 *)(ea))[0] |		\
			    ((uint8 *)(ea))[1] |		\
			    ((uint8 *)(ea))[2] |		\
			    ((uint8 *)(ea))[3] |		\
			    ((uint8 *)(ea))[4] |		\
			    ((uint8 *)(ea))[5]) == 0)

#undef PACKED
#if !defined(__GNUC__)
#pragma pack()
#endif

#endif /* __NET_ETHERNET_H */
