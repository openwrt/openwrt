/*******************************************************************************
 * $Id$
 * Copyright 2004, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 * From FreeBSD 2.2.7: Fundamental constants relating to ethernet.
 ******************************************************************************/

#ifndef _NET_ETHERNET_H_	    /* use native BSD ethernet.h when available */
#define _NET_ETHERNET_H_

#ifndef _TYPEDEFS_H_
#include "typedefs.h"
#endif

#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#define	PACKED
#endif

/*
 * The number of bytes in an ethernet (MAC) address.
 */
#ifndef ETHER_ADDR_LEN
#define	ETHER_ADDR_LEN		6
#endif

/*
 * The number of bytes in the type field.
 */
#ifndef	ETHER_TYPE_LEN
#define	ETHER_TYPE_LEN		2
#endif

/*
 * The number of bytes in the trailing CRC field.
 */
#ifndef	ETHER_CRC_LEN
#define	ETHER_CRC_LEN		4
#endif

/*
 * The length of the combined header.
 */
#ifndef	ETHER_HDR_LEN
#define	ETHER_HDR_LEN		(ETHER_ADDR_LEN*2+ETHER_TYPE_LEN)
#endif

/*
 * The minimum packet length.
 */
#ifndef ETHER_MIN_LEN
#define	ETHER_MIN_LEN		64
#endif

/*
 * The minimum packet user data length.
 */
#ifndef ETHER_MIN_DATA
#define	ETHER_MIN_DATA		46
#endif

/*
 * The maximum packet length.
 */
#ifndef ETHER_MAX_LEN
#define	ETHER_MAX_LEN		1518
#endif

/*
 * The maximum packet user data length.
 */
#define	ETHER_MAX_DATA		1500

/*
 * Used to uniquely identify a 802.1q VLAN-tagged header.
 */
#define	VLAN_TAG			0x8100

/*
 * Located after dest & src address in ether header.
 */
#define VLAN_FIELDS_OFFSET		(ETHER_ADDR_LEN * 2)

/*
 * 4 bytes of vlan field info.
 */
#define VLAN_FIELDS_SIZE		4

/* location of bits in 16-bit vlan fields */
#define VLAN_PRI_SHIFT		13	/* user priority */
#define VLAN_CFI_SHIFT		12	/* canonical format indicator bit */

/* 3 bits of priority */
#define VLAN_PRI_MASK			7
/* 12 bits of vlan identfier (VID) */
#define VLAN_VID_MASK		0xFFF	/* VLAN identifier (VID) field */

struct  vlan_tags {
	uint16  tag_type;	/* 0x8100 for VLAN */
	uint16  tag_control;	/* prio | cfi | vid */
} PACKED ;

/* 802.1X ethertype */

#define	ETHER_TYPE_IP		0x0800		/* IP */
#define	ETHER_TYPE_BRCM		0x886c		/* Broadcom Corp. */
#define	ETHER_TYPE_802_1X	0x888e		/* 802.1x */

#define	ETHER_BRCM_SUBTYPE_LEN	4		/* Broadcom 4byte subtype follows ethertype */
#define	ETHER_BRCM_CRAM		0x1		/* Broadcom subtype cram protocol */

/*
 * A macro to validate a length with
 */
#define	ETHER_IS_VALID_LEN(foo)	\
	((foo) >= ETHER_MIN_LEN && (foo) <= ETHER_MAX_LEN)

#ifndef __NET_ETHERNET_H
#ifndef __INCif_etherh     /* Quick and ugly hack for VxWorks */
/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct	ether_header {
	uint8	ether_dhost[ETHER_ADDR_LEN];
	uint8	ether_shost[ETHER_ADDR_LEN];
	uint16	ether_type;
} PACKED ;

/*
 * Structure of a 48-bit Ethernet address.
 */
struct	ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
} PACKED ;
#endif
#endif

/*
 * Takes a pointer, returns true if a 48-bit multicast address
 * (including broadcast, since it is all ones)
 */
#define ETHER_ISMULTI(ea) (((uint8 *)(ea))[0] & 1)

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

/* Differentiated Services Codepoint - lower 6 bits of tos in iphdr */
#define	DSCP_PRI_MASK		0x3F		/* bits 0-6 */
#define	DSCP_WME_PRI_MASK	0x38		/* bits 3-6 */
#define	DSCP_WME_PRI_SHIFT	3

#undef PACKED

#endif /* _NET_ETHERNET_H_ */
