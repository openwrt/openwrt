/*
 * Hardware-specific Receive Data Header for the
 * Broadcom Home Networking Division
 * BCM44XX and BCM47XX 10/100 Mbps Ethernet cores.
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id$
 */

#ifndef _bcmenetrxh_h_
#define	_bcmenetrxh_h_

/*
 * The Ethernet MAC core returns an 8-byte Receive Frame Data Header
 * with every frame consisting of
 * 16bits of frame length, followed by
 * 16bits of EMAC rx descriptor info, followed by 32bits of undefined.
 */
typedef volatile struct {
	uint16	len;
	uint16	flags;
	uint16	pad[12];
} bcmenetrxh_t;

#define	RXHDR_LEN	28

#define	RXF_L		((uint16)1 << 11)	/* last buffer in a frame */
#define	RXF_MISS	((uint16)1 << 7)	/* received due to promisc mode */
#define	RXF_BRDCAST	((uint16)1 << 6)	/* dest is broadcast address */
#define	RXF_MULT	((uint16)1 << 5)	/* dest is multicast address */
#define	RXF_LG		((uint16)1 << 4)	/* frame length > rxmaxlength */
#define	RXF_NO		((uint16)1 << 3)	/* odd number of nibbles */
#define	RXF_RXER	((uint16)1 << 2)	/* receive symbol error */
#define	RXF_CRC		((uint16)1 << 1)	/* crc error */
#define	RXF_OV		((uint16)1 << 0)	/* fifo overflow */

#endif	/* _bcmenetrxh_h_ */
