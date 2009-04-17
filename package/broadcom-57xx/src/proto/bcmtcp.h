/*
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Fundamental constants relating to TCP Protocol
 *
 */

#ifndef _bcmtcp_h_
#define _bcmtcp_h_

/* enable structure packing */
#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#pragma pack(1)
#define	PACKED
#endif

#define TCP_SRC_PORT_OFFSET	0	/* TCP source port offset */
#define TCP_DEST_PORT_OFFSET	2	/* TCP dest port offset */
#define TCP_CHKSUM_OFFSET	16	/* TCP body checksum offset */

/* These fields are stored in network order */
struct bcmtcp_hdr
{
	uint16	src_port;	/* Source Port Address */
	uint16	dst_port;	/* Destination Port Address */
	uint32	seq_num;	/* TCP Sequence Number */
	uint32	ack_num;	/* TCP Sequence Number */
	uint16	hdrlen_rsvd_flags;	/* Header length, reserved bits and flags */
	uint16	tcpwin;		/* TCP window */
	uint16	chksum;		/* Segment checksum with pseudoheader */
	uint16	urg_ptr;	/* Points to seq-num of byte following urg data */
} PACKED;

#undef PACKED
#if !defined(__GNUC__)
#pragma pack()
#endif

/* Byte offset of flags in TCP header */
#define TCP_FLAGS_OFFSET	13

#define TCP_FLAGS_FIN		0x01
#define TCP_FLAGS_SYN		0x02
#define TCP_FLAGS_RST		0x03
#define TCP_FLAGS_PSH		0x04
#define TCP_FLAGS_ACK		0x10
#define TCP_FLAGS_URG		0x20
#define TCP_FLAGS_ECN		0x40
#define TCP_FLAGS_CWR		0x80

#define TCP_FLAGS(tcp_hdr)	(((uint8 *)(tcp_hdr))[TCP_FLAGS_OFFSET])
#define TCP_IS_ACK(tcp_hdr)	(TCP_FLAGS(tcp_hdr) & TCP_FLAGS_ACK)

#define TCP_SRC_PORT(tcp_hdr)	(ntoh16(((struct bcmtcp_hdr*)(tcp_hdr))->src_port))
#define TCP_DST_PORT(tcp_hdr)	(ntoh16(((struct bcmtcp_hdr*)(tcp_hdr))->dst_port))
#define TCP_SEQ_NUM(tcp_hdr)	(ntoh32(((struct bcmtcp_hdr*)(tcp_hdr))->seq_num))
#define TCP_ACK_NUM(tcp_hdr)	(ntoh32(((struct bcmtcp_hdr*)(tcp_hdr))->ack_num))

#endif	/* #ifndef _bcmtcp_h_ */
