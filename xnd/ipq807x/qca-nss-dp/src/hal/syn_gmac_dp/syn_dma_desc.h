/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __SYN_DESC__
#define __SYN_DESC__

/**********************************************************
 * DMA Engine descriptors
 **********************************************************/
/*
******Enhanced Descritpor structure to support 8K buffer per buffer *******

dma_rx_base_addr = 0x000C,	CSR3 - Receive Descriptor list base address
dma_rx_base_addr is the pointer to the first Rx Descriptors.
The Descriptor format in Little endian with a 32 bit Data bus is as shown below.

Similarly
dma_tx_base_addr     = 0x0010,  CSR4 - Transmit Descriptor list base address
dma_tx_base_addr is the pointer to the first Tx Descriptors.
The Descriptor format in Little endian with a 32 bit Data bus is as shown below.
	-------------------------------------------------------------------------
 RDES0	|OWN (31)| Status						       	|
	-------------------------------------------------------------------------
 RDES1	| Ctrl | Res | Byte Count Buffer 2 | Ctrl | Res | Byte Count Buffer 1  	|
	-------------------------------------------------------------------------
 RDES2	|	Buffer 1 Address						|
	-------------------------------------------------------------------------
 RDES3	|  	Buffer 2 Address / Next Descriptor Address			|
	-------------------------------------------------------------------------
 RDES4	|	Extended Status							|
	-------------------------------------------------------------------------
 RDES5	|	Reserved							|
	-------------------------------------------------------------------------
 RDES6	|	Receive Timestamp Low 						|
	-------------------------------------------------------------------------
 RDES7	|	Receive Timestamp High 						|
	-------------------------------------------------------------------------

	------------------------------------------------------------------------
 TDES0	|OWN (31)| Ctrl | Res | Ctrl | Res | Status				|
	------------------------------------------------------------------------
 TDES1	| Res | Byte Count Buffer 2 | Res |         Byte Count Buffer 1		|
	------------------------------------------------------------------------
 TDES2	|	Buffer 1 Address						|
	------------------------------------------------------------------------
 TDES3	|	Buffer 2 Address / Next Descriptor Address			|
	------------------------------------------------------------------------
 TDES4	|	Reserved							|
	------------------------------------------------------------------------
 TDES5	|	Reserved							|
	------------------------------------------------------------------------
 TDES6	|	Transmit Timestamp Low						|
	------------------------------------------------------------------------
 TDES7	|	Transmit Timestamp Higher					|
	------------------------------------------------------------------------
*/

/*
 * dma_descriptor_status
 *	status word of DMA descriptor
 */
enum dma_descriptor_status {
	desc_own_by_dma = 0x80000000,		/* (OWN)Descriptor is
						   owned by DMA engine        	*/
	desc_rx_da_filter_fail = 0x40000000,	/* (AFM)Rx - DA Filter
						   Fail for the rx frame      	*/
	desc_rx_frame_length_mask = 0x3FFF0000,	/* (FL)Receive descriptor
						   frame length               	*/
	desc_rx_frame_length_shift = 16,
	desc_rx_error = 0x00008000,		/* (ES)Error summary bit
						   - OR of the  following bits:
						   DE || OE || IPC || GF || LC  || RWT
						   || RE || CE                	*/
	desc_rx_truncated = 0x00004000,		/* (DE)Rx - no more descriptors
						   for receive frame          	*/
	desc_sa_filter_fail = 0x00002000,	/* (SAF)Rx - SA Filter Fail for
						   the received frame         	*/
	desc_rx_length_error = 0x00001000,	/* (LE)Rx - frm size not
						   matching with len field    	*/
	desc_rx_overflow = 0x00000800,		/* (OE)Rx - frm was damaged due
						   to buffer overflow         	*/
	desc_rx_vlan_tag = 0x00000400,		/* (VLAN)Rx - received frame
						   is a VLAN frame            	*/
	desc_rx_first = 0x00000200,		/* (FS)Rx - first
						   descriptor of the frame    	*/
	desc_rx_last = 0x00000100,		/* (LS)Rx - last
						   descriptor of the frame    	*/
	desc_rx_long_frame = 0x00000080,	/* (Giant Frame)Rx - frame is
						   longer than 1518/1522      	*/
	desc_rx_collision = 0x00000040,		/* (LC)Rx - late collision
						   occurred during reception  	*/
	desc_rx_frame_ether = 0x00000020,	/* (FT)Rx - Frame type - Ether,
						   otherwise 802.3            	*/
	desc_rx_watchdog = 0x00000010,		/* (RWT)Rx - watchdog timer
						   expired during reception   	*/
	desc_rx_mii_error = 0x00000008,		/* (RE)Rx - error reported
						   by MII interface           	*/
	desc_rx_dribbling = 0x00000004,		/* (DE)Rx - frame contains non
						   int multiple of 8 bits     	*/
	desc_rx_crc = 0x00000002,		/* (CE)Rx - CRC error         	*/
	desc_rx_ext_sts = 0x00000001,		/* Extended Status Available
						   in RDES4			*/
	desc_tx_error = 0x00008000,		/* (ES)Error summary Bits	*/
	desc_tx_int_enable = 0x40000000,	/* (IC)Tx - interrupt on
						   completion			*/
	desc_tx_last = 0x20000000,		/* (LS)Tx - Last segment of the
						   frame			*/
	desc_tx_first = 0x10000000,		/* (FS)Tx - First segment of the
						   frame			*/
	desc_tx_disable_crc = 0x08000000,	/* (DC)Tx - Add CRC disabled
						   (first segment only)       	*/
	desc_tx_disable_padd = 0x04000000,	/* (DP)disable padding,
						   added by - reyaz		*/
	desc_tx_cis_mask = 0x00c00000,		/* Tx checksum offloading
						   control mask			*/
	desc_tx_cis_bypass = 0x00000000,	/* Checksum bypass            	*/
	desc_tx_cis_ipv4_hdr_cs = 0x00400000,	/* IPv4 header checksum       	*/
	desc_tx_cis_tcp_only_cs = 0x00800000,	/* TCP/UDP/ICMP checksum.
						   Pseudo header  checksum
						   is assumed to be present   	*/
	desc_tx_cis_tcp_pseudo_cs = 0x00c00000,	/* TCP/UDP/ICMP checksum fully
						   in hardware  including
						   pseudo header              	*/
	desc_tx_desc_end_of_ring = 0x00200000,	/* (TER)End of descriptor ring*/
	desc_tx_desc_chain = 0x00100000,		/* (TCH)Second buffer address
						   is chain address           	*/
	desc_rx_chk_bit0 = 0x00000001,		/* Rx Payload Checksum Error  	*/
	desc_rx_chk_bit7 = 0x00000080,		/* (IPC CS ERROR)Rx - Ipv4
						   header checksum error      	*/
	desc_rx_chk_bit5 = 0x00000020,		/* (FT)Rx - Frame type - Ether,
						   otherwise 802.3 		*/
	desc_rx_ts_avail = 0x00000080,		/* Time stamp available 	*/
	desc_rx_frame_type = 0x00000020,	/* (FT)Rx - Frame type - Ether,
						   otherwise 802.3		*/
	desc_tx_ipv4_chk_error = 0x00010000,	/* (IHE) Tx Ip header error	*/
	desc_tx_timeout = 0x00004000,		/* (JT)Tx - Transmit
						   jabber timeout		*/
	desc_tx_frame_flushed = 0x00002000,	/* (FF)Tx - DMA/MTL flushed
						   the frame  due to SW flush 	*/
	desc_tx_pay_chk_error = 0x00001000,	/* (PCE) Tx Payload checksum
						   Error		      	*/
	desc_tx_lost_carrier = 0x00000800,	/* (LC)Tx - carrier lost
						   during tramsmission        	*/
	desc_tx_no_carrier = 0x00000400,	/* (NC)Tx - no carrier signal
						   from the tranceiver        	*/
	desc_tx_late_collision = 0x00000200,	/* (LC)Tx - transmission aborted
						   due to collision           	*/
	desc_tx_exc_collisions = 0x00000100,	/* (EC)Tx - transmission aborted
						   after 16 collisions        	*/
	desc_tx_vlan_frame = 0x00000080,	/* (VF)Tx - VLAN-type frame   	*/
	desc_tx_coll_mask = 0x00000078,		/* (CC)Tx - Collision count   	*/
	desc_tx_coll_shift = 3,
	desc_tx_exc_deferral = 0x00000004,	/* (ED)Tx - excessive deferral 	*/
	desc_tx_underflow = 0x00000002,		/* (UF)Tx - late data arrival
						   from the memory            	*/
	desc_tx_deferred = 0x00000001,		/* (DB)Tx - frame
						   transmision deferred       	*/

	/*
	 * This explains the RDES1/TDES1 bits layout
	 *             ------------------------------------------------------
	 * RDES1/TDES1 | Control Bits | Byte Count Buf 2 | Byte Count Buf 1 |
	 *             ------------------------------------------------------
	 */

	/* dma_descriptor_length */	/* length word of DMA descriptor */
	desc_rx_dis_int_compl = 0x80000000,	/* (Disable Rx int on completion) */
	desc_rx_desc_end_of_ring = 0x00008000,	/* (RER)End of descriptor ring 	*/
	desc_rx_desc_chain = 0x00004000,	/* (RCH)Second buffer address
					   	    is chain address 		*/
	desc_size2_mask = 0x1FFF0000,		/* (RBS2/TBS2) Buffer 2 size 	*/
	desc_size2_shift = 16,
	desc_size1_mask = 0x00001FFF,		/* (RBS1/TBS1) Buffer 1 size 	*/
	desc_size1_shift = 0,

	/*
	 * This explains the RDES4 Extended Status bits layout
	 *              --------------------------------------------------------
	 *   RDES4      |                 Extended Status                      |
	 *              --------------------------------------------------------
	 */
	desc_rx_ts_dropped = 0x00004000,	/* PTP snapshot available     	*/
	desc_rx_ptp_ver = 0x00002000,		/* When set indicates IEEE1584
						   Version 2 (else Ver1)      	*/
	desc_rx_ptp_frame_type = 0x00001000,	/* PTP frame type Indicates PTP
						   sent over ethernet         	*/
	desc_rx_ptp_message_type = 0x00000F00,	/* Message Type               	*/
	desc_rx_ptp_no = 0x00000000,		/* 0000 => No PTP message rcvd 	*/
	desc_rx_ptp_sync = 0x00000100,		/* 0001 => Sync (all clock
						   types) received            	*/
	desc_rx_ptp_follow_up = 0x00000200,	/* 0010 => Follow_Up (all clock
						   types) received            	*/
	desc_rx_ptp_delay_req = 0x00000300,	/* 0011 => Delay_Req (all clock
						   types) received            	*/
	desc_rx_ptp_delay_resp = 0x00000400,	/* 0100 => Delay_Resp (all clock
						   types) received            	*/
	desc_rx_ptp_pdelay_req = 0x00000500,	/* 0101 => Pdelay_Req (in P
						   to P tras clk)  or Announce
						   in Ord and Bound clk       	*/
	desc_rx_ptp_pdelay_resp = 0x00000600,	/* 0110 => Pdealy_Resp(in P to
						   P trans clk) or Management in
						   Ord and Bound clk          	*/
	desc_rx_ptp_pdelay_resp_fp = 0x00000700,/* 0111 => Pdelay_Resp_Follow_Up
						   (in P to P trans clk) or
						   Signaling in Ord and Bound
						   clk			      	*/
	desc_rx_ptp_ipv6 = 0x00000080,		/* Received Packet is in IPV6 	*/
	desc_rx_ptp_ipv4 = 0x00000040,		/* Received Packet is in IPV4 	*/
	desc_rx_chk_sum_bypass = 0x00000020,	/* When set indicates checksum
						   offload engine is bypassed 	*/
	desc_rx_ip_payload_error = 0x00000010,	/* When set indicates 16bit IP
						   payload CS is in error     	*/
	desc_rx_ip_header_error = 0x00000008,	/* When set indicates 16bit IPV4
						   hdr CS is err or IP datagram
						   version is not consistent
						   with Ethernet type value   	*/
	desc_rx_ip_payload_type = 0x00000007,	/* Indicate the type of payload
						   encapsulated in IPdatagram
						   processed by COE (Rx)      	*/
	desc_rx_ip_payload_unknown = 0x00000000,/* Unknown or didnot process
						   IP payload                 	*/
	desc_rx_ip_payload_udp = 0x00000001,	/* UDP                        	*/
	desc_rx_ip_payload_tcp = 0x00000002,	/* TCP                        	*/
	desc_rx_ip_payload_icmp = 0x00000003,	/* ICMP                       	*/
};

/*
 * dma_desc
 *	DMA Descriptor Structure
 *
 * The structure is common for both receive and transmit descriptors.
 */
struct dma_desc {
	uint32_t status;	/* Status                             */
	uint32_t length;	/* Buffer 1  and Buffer 2 length      */
	uint32_t buffer1;	/* Network Buffer 1 pointer (DMA-able)*/
	uint32_t data1;		/* This holds virtual address of
				   buffer1, not used by DMA	      */

	/* This data below is used only by driver */
	uint32_t extstatus;	/* Extended status of a Rx Descriptor */
	uint32_t reserved1;	/* Reserved word                      */
	uint32_t timestamplow;	/* Lower 32 bits of the 64
				   bit timestamp value                */
	uint32_t timestamphigh;	/* Higher 32 bits of the 64
					   bit timestamp value        */
};

/*
 * syn_dp_gmac_tx_checksum_offload_tcp_pseudo
 *	The checksum offload engine is enabled to do complete checksum computation.
 */
static inline void syn_dp_gmac_tx_checksum_offload_tcp_pseudo(struct dma_desc *desc)
{
	desc->status = ((desc->status & (~desc_tx_cis_mask)) | desc_tx_cis_tcp_pseudo_cs);
}

/*
 * syn_dp_gmac_tx_desc_init_ring
 *	Initialize the tx descriptors for ring or chain mode operation.
 */
static inline void syn_dp_gmac_tx_desc_init_ring(struct dma_desc *desc, uint32_t no_of_desc)
{
	struct dma_desc *last_desc = desc + no_of_desc - 1;
	memset(desc, 0, no_of_desc * sizeof(struct dma_desc));
	last_desc->status = desc_tx_desc_end_of_ring;
}

/*
 * syn_dp_gmac_rx_desc_init_ring
 *	Initialize the rx descriptors for ring or chain mode operation.
 */
static inline void syn_dp_gmac_rx_desc_init_ring(struct dma_desc *desc, uint32_t no_of_desc)
{
	struct dma_desc *last_desc = desc + no_of_desc - 1;
	memset(desc, 0, no_of_desc * sizeof(struct dma_desc));
	last_desc->length = desc_rx_desc_end_of_ring;
}

/*
 * syn_dp_gmac_is_rx_desc_valid
 *	Checks whether the rx descriptor is valid.
 */
static inline bool syn_dp_gmac_is_rx_desc_valid(uint32_t status)
{
	return (status & (desc_rx_error | desc_rx_first | desc_rx_last)) ==
		(desc_rx_first | desc_rx_last);
}

/*
 * syn_dp_gmac_get_rx_desc_frame_length
 *	Returns the byte length of received frame including CRC.
 */
static inline uint32_t syn_dp_gmac_get_rx_desc_frame_length(uint32_t status)
{
	return (status & desc_rx_frame_length_mask) >> desc_rx_frame_length_shift;
}

/*
 * syn_dp_gmac_is_desc_owned_by_dma
 *	Checks whether the descriptor is owned by DMA.
 */
static inline bool syn_dp_gmac_is_desc_owned_by_dma(struct dma_desc *desc)
{
	return (desc->status & desc_own_by_dma) == desc_own_by_dma;
}

/*
 * syn_dp_gmac_is_desc_empty
 *	Checks whether the descriptor is empty.
 */
static inline bool syn_dp_gmac_is_desc_empty(struct dma_desc *desc)
{
	/*
	 * If length of both buffer1 & buffer2 are zero then desc is empty
	 */
	return (desc->length & desc_size1_mask) == 0;
}

/*
 * syn_dp_gmac_get_tx_collision_count
 *	Gives the transmission collision count.
 */
static inline uint32_t syn_dp_gmac_get_tx_collision_count(uint32_t status)
{
	return (status & desc_tx_coll_mask) >> desc_tx_coll_shift;
}

#endif /*  __SYN_DESC__ */