/*
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.                
 *                                     
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;   
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior      
 * written permission of Broadcom Corporation.                            
 *
 * Fundamental constants relating to DHCP Protocol
 *
 */

#ifndef _bcmdhcp_h_
#define _bcmdhcp_h_

/* DHCP params */
#define DHCP_TYPE_OFFSET	0	/* DHCP type (request|reply) offset */
#define DHCP_FLAGS_OFFSET	10	/* DHCP flags offset */
#define DHCP_CIADDR_OFFSET	12	/* DHCP client IP address offset */
#define DHCP_YIADDR_OFFSET	16	/* DHCP your IP address offset */
#define DHCP_GIADDR_OFFSET	24	/* DHCP relay agent IP address offset */
#define DHCP_CHADDR_OFFSET	28	/* DHCP client h/w address offset */

#define DHCP_TYPE_REQUEST	1	/* DHCP request (discover|request) */
#define DHCP_TYPE_REPLY		2	/* DHCP reply (offset|ack) */

#define DHCP_PORT_SERVER	67	/* DHCP server UDP port */
#define DHCP_PORT_CLIENT	68	/* DHCP client UDP port */

#define DHCP_FLAG_BCAST	0x8000	/* DHCP broadcast flag */

#define DHCP_FLAGS_LEN	2	/* DHCP flags field length */

#endif	/* #ifndef _bcmdhcp_h_ */
