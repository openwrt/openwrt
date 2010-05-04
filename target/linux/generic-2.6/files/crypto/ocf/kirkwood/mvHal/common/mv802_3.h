/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __INCmv802_3h
#define __INCmv802_3h


/* includes */
#include "mvTypes.h"

/* Defines */
#define MV_MAX_ETH_DATA     1500

/* 802.3 types */
#define MV_IP_TYPE                  0x0800
#define MV_IP_ARP_TYPE              0x0806
#define MV_APPLE_TALK_ARP_TYPE      0x80F3
#define MV_NOVELL_IPX_TYPE          0x8137
#define MV_EAPOL_TYPE				0x888e



/* Encapsulation header for RFC1042 and Ethernet_tunnel */

#define MV_RFC1042_SNAP_HEADER     {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00}

#define MV_ETH_SNAP_LSB             0xF8


#define	MV_MAC_ADDR_SIZE	(6)
#define MV_MAC_STR_SIZE		(20)
#define MV_VLAN_HLEN		(4)

/* This macro checks for a multicast mac address    */
#define MV_IS_MULTICAST_MAC(mac)  (((mac)[0] & 0x1) == 1)


/* This macro checks for an broadcast mac address     */
#define MV_IS_BROADCAST_MAC(mac)            \
       (((mac)[0] == 0xFF) &&       \
        ((mac)[1] == 0xFF) &&       \
        ((mac)[2] == 0xFF) &&       \
        ((mac)[3] == 0xFF) &&       \
        ((mac)[4] == 0xFF) &&       \
        ((mac)[5] == 0xFF))


/* Typedefs */
typedef struct
{
    MV_U8     pDA[MV_MAC_ADDR_SIZE];
    MV_U8     pSA[MV_MAC_ADDR_SIZE];
    MV_U16    typeOrLen;

} MV_802_3_HEADER;

enum {
  MV_IP_PROTO_NULL	= 0,    /* Dummy protocol for TCP               */
  MV_IP_PROTO_ICMP	= 1,    /* Internet Control Message Protocol    */
  MV_IP_PROTO_IGMP	= 2,    /* Internet Group Management Protocol   */
  MV_IP_PROTO_IPIP	= 4,    /* IPIP tunnels (older KA9Q tunnels use 94) */
  MV_IP_PROTO_TCP	= 6,    /* Transmission Control Protocol        */
  MV_IP_PROTO_EGP	= 8,    /* Exterior Gateway Protocol            */
  MV_IP_PROTO_PUP	= 12,   /* PUP protocol                         */
  MV_IP_PROTO_UDP	= 17,   /* User Datagram Protocol               */
  MV_IP_PROTO_IDP	= 22,   /* XNS IDP protocol                     */
  MV_IP_PROTO_DCCP	= 33,   /* Datagram Congestion Control Protocol */
  MV_IP_PROTO_IPV6	= 41,   /* IPv6-in-IPv4 tunnelling              */
  MV_IP_PROTO_RSVP	= 46,   /* RSVP protocol                        */
  MV_IP_PROTO_GRE	= 47,   /* Cisco GRE tunnels (rfc 1701,1702)    */
  MV_IP_PROTO_ESP	= 50,   /* Encapsulation Security Payload protocol */
  MV_IP_PROTO_AH	= 51,   /* Authentication Header protocol       */
  MV_IP_PROTO_BEETPH	= 94,   /* IP option pseudo header for BEET     */
  MV_IP_PROTO_PIM	= 103, 
  MV_IP_PROTO_COMP	= 108,  /* Compression Header protocol          */
  MV_IP_PROTO_ZERO_HOP	= 114,  /* Any 0 hop protocol (IANA)            */
  MV_IP_PROTO_SCTP	= 132,  /* Stream Control Transport Protocol    */
  MV_IP_PROTO_UDPLITE	= 136,  /* UDP-Lite (RFC 3828)                  */

  MV_IP_PROTO_RAW	= 255,  /* Raw IP packets                       */
  MV_IP_PROTO_MAX
};

typedef struct
{
    MV_U8   version;
    MV_U8   tos;
    MV_U16  totalLength;
    MV_U16  identifier;
    MV_U16  fragmentCtrl;
    MV_U8   ttl;
    MV_U8   protocol;
    MV_U16  checksum;
    MV_U32  srcIP;
    MV_U32  dstIP;

} MV_IP_HEADER; 

typedef struct
{       
    MV_U32 spi;
    MV_U32 seqNum;
} MV_ESP_HEADER; 

#define MV_ICMP_ECHOREPLY          0       /* Echo Reply                   */
#define MV_ICMP_DEST_UNREACH       3       /* Destination Unreachable      */
#define MV_ICMP_SOURCE_QUENCH      4       /* Source Quench                */
#define MV_ICMP_REDIRECT           5       /* Redirect (change route)      */
#define MV_ICMP_ECHO               8       /* Echo Request                 */
#define MV_ICMP_TIME_EXCEEDED      11      /* Time Exceeded                */
#define MV_ICMP_PARAMETERPROB      12      /* Parameter Problem            */
#define MV_ICMP_TIMESTAMP          13      /* Timestamp Request            */
#define MV_ICMP_TIMESTAMPREPLY     14      /* Timestamp Reply              */
#define MV_ICMP_INFO_REQUEST       15      /* Information Request          */
#define MV_ICMP_INFO_REPLY         16      /* Information Reply            */
#define MV_ICMP_ADDRESS            17      /* Address Mask Request         */
#define MV_ICMP_ADDRESSREPLY       18      /* Address Mask Reply           */

typedef struct
{
    MV_U8   type;
    MV_U8   code;
    MV_U16  checksum;
    MV_U16  id;
    MV_U16  sequence;

} MV_ICMP_ECHO_HEADER;

typedef struct
{
    MV_U16  source;
    MV_U16  dest;
    MV_U32  seq;
    MV_U32  ack_seq;
    MV_U16  flags;
    MV_U16  window;
    MV_U16  chksum;
    MV_U16  urg_offset;

} MV_TCP_HEADER;

typedef struct
{
    MV_U16  source;
    MV_U16  dest;
    MV_U16  len;
    MV_U16  check;

} MV_UDP_HEADER;

#endif /* __INCmv802_3h */
