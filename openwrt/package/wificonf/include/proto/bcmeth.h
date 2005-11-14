/*
 * Broadcom Ethernettype  protocol definitions
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

/*
 * Broadcom Ethernet protocol defines 
 *
 */

#ifndef _BCMETH_H_
#define _BCMETH_H_

/* enable structure packing */
#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#pragma pack(1)
#define	PACKED
#endif

/* ETHER_TYPE_BRCM is defined in ethernet.h */

/*
 * Following the 2byte BRCM ether_type is a 16bit BRCM subtype field
 * in one of two formats: (only subtypes 32768-65535 are in use now)
 *
 * subtypes 0-32767:
 *     8 bit subtype (0-127)
 *     8 bit length in bytes (0-255)
 *
 * subtypes 32768-65535:
 *     16 bit big-endian subtype
 *     16 bit big-endian length in bytes (0-65535)
 *
 * length is the number of additional bytes beyond the 4 or 6 byte header
 *
 * Reserved values:
 * 0 reserved
 * 5-15 reserved for iLine protocol assignments
 * 17-126 reserved, assignable
 * 127 reserved
 * 32768 reserved
 * 32769-65534 reserved, assignable
 * 65535 reserved
 */

/* 
 * While adding the subtypes and their specific processing code make sure 
 * bcmeth_bcm_hdr_t is the first data structure in the user specific data structure definition 
 */

#define	BCMILCP_SUBTYPE_RATE		1
#define	BCMILCP_SUBTYPE_LINK		2
#define	BCMILCP_SUBTYPE_CSA		3
#define	BCMILCP_SUBTYPE_LARQ		4
#define BCMILCP_SUBTYPE_VENDOR		5
#define	BCMILCP_SUBTYPE_FLH		17

#define BCMILCP_SUBTYPE_VENDOR_LONG	32769
#define BCMILCP_SUBTYPE_CERT		32770
#define BCMILCP_SUBTYPE_SES		32771


#define BCMILCP_BCM_SUBTYPE_RESERVED	0
#define BCMILCP_BCM_SUBTYPE_EVENT		1
#define BCMILCP_BCM_SUBTYPE_SES			2
/*
The EAPOL type is not used anymore. Instead EAPOL messages are now embedded
within BCMILCP_BCM_SUBTYPE_EVENT type messages
*/
/*#define BCMILCP_BCM_SUBTYPE_EAPOL		3*/

#define BCMILCP_BCM_SUBTYPEHDR_MINLENGTH	8
#define BCMILCP_BCM_SUBTYPEHDR_VERSION		0

/* These fields are stored in network order */
typedef  struct bcmeth_hdr
{
	uint16	subtype; /* Vendor specific..32769*/
	uint16	length; 
	uint8	version; /* Version is 0*/
	uint8	oui[3]; /* Broadcom OUI*/
	/* user specific Data */
	uint16	usr_subtype;
} PACKED bcmeth_hdr_t;



#undef PACKED
#if !defined(__GNUC__)
#pragma pack()
#endif

#endif
