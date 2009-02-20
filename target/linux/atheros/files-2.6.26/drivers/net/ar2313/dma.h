#ifndef __ARUBA_DMA_H__
#define __ARUBA_DMA_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * DMA register definition.
 *
 * File   : $Id: dma.h,v 1.3 2002/06/06 18:34:03 astichte Exp $
 *
 * Author : ryan.holmQVist@idt.com
 * Date   : 20011005
 * Update :
 *	    $Log: dma.h,v $
 *	    Revision 1.3  2002/06/06 18:34:03  astichte
 *	    Added XXX_PhysicalAddress and XXX_VirtualAddress
 *
 *	    Revision 1.2  2002/06/05 18:30:46  astichte
 *	    Removed IDTField
 *
 *	    Revision 1.1  2002/05/29 17:33:21  sysarch
 *	    jba File moved from vcode/include/idt/acacia
 *
 *
 ******************************************************************************/

#define AR_BIT(x)            (1 << (x))
#define DMA_RX_ERR_CRC       AR_BIT(1)
#define DMA_RX_ERR_DRIB      AR_BIT(2)
#define DMA_RX_ERR_MII       AR_BIT(3)
#define DMA_RX_EV2           AR_BIT(5)
#define DMA_RX_ERR_COL       AR_BIT(6)
#define DMA_RX_LONG          AR_BIT(7)
#define DMA_RX_LS            AR_BIT(8)	/* last descriptor */
#define DMA_RX_FS            AR_BIT(9)	/* first descriptor */
#define DMA_RX_MF            AR_BIT(10)	/* multicast frame */
#define DMA_RX_ERR_RUNT      AR_BIT(11)	/* runt frame */
#define DMA_RX_ERR_LENGTH    AR_BIT(12)	/* length error */
#define DMA_RX_ERR_DESC      AR_BIT(14)	/* descriptor error */
#define DMA_RX_ERROR         AR_BIT(15)	/* error summary */
#define DMA_RX_LEN_MASK      0x3fff0000
#define DMA_RX_LEN_SHIFT     16
#define DMA_RX_FILT          AR_BIT(30)
#define DMA_RX_OWN           AR_BIT(31)	/* desc owned by DMA controller */

#define DMA_RX1_BSIZE_MASK   0x000007ff
#define DMA_RX1_BSIZE_SHIFT  0
#define DMA_RX1_CHAINED      AR_BIT(24)
#define DMA_RX1_RER          AR_BIT(25)

#define DMA_TX_ERR_UNDER     AR_BIT(1)	/* underflow error */
#define DMA_TX_ERR_DEFER     AR_BIT(2)	/* excessive deferral */
#define DMA_TX_COL_MASK      0x78
#define DMA_TX_COL_SHIFT     3
#define DMA_TX_ERR_HB        AR_BIT(7)	/* hearbeat failure */
#define DMA_TX_ERR_COL       AR_BIT(8)	/* excessive collisions */
#define DMA_TX_ERR_LATE      AR_BIT(9)	/* late collision */
#define DMA_TX_ERR_LINK      AR_BIT(10)	/* no carrier */
#define DMA_TX_ERR_LOSS      AR_BIT(11)	/* loss of carrier */
#define DMA_TX_ERR_JABBER    AR_BIT(14)	/* transmit jabber timeout */
#define DMA_TX_ERROR         AR_BIT(15)	/* frame aborted */
#define DMA_TX_OWN           AR_BIT(31)	/* descr owned by DMA controller */

#define DMA_TX1_BSIZE_MASK   0x000007ff
#define DMA_TX1_BSIZE_SHIFT  0
#define DMA_TX1_CHAINED      AR_BIT(24)	/* chained descriptors */
#define DMA_TX1_TER          AR_BIT(25)	/* transmit end of ring */
#define DMA_TX1_FS           AR_BIT(29)	/* first segment */
#define DMA_TX1_LS           AR_BIT(30)	/* last segment */
#define DMA_TX1_IC           AR_BIT(31)	/* interrupt on completion */

#define RCVPKT_LENGTH(X)     (X  >> 16)	/* Received pkt Length */

#define MAC_CONTROL_RE       AR_BIT(2)	/* receive enable */
#define MAC_CONTROL_TE       AR_BIT(3)	/* transmit enable */
#define MAC_CONTROL_DC       AR_BIT(5)	/* Deferral check */
#define MAC_CONTROL_ASTP     AR_BIT(8)	/* Auto pad strip */
#define MAC_CONTROL_DRTY     AR_BIT(10)	/* Disable retry */
#define MAC_CONTROL_DBF      AR_BIT(11)	/* Disable bcast frames */
#define MAC_CONTROL_LCC      AR_BIT(12)	/* late collision ctrl */
#define MAC_CONTROL_HP       AR_BIT(13)	/* Hash Perfect filtering */
#define MAC_CONTROL_HASH     AR_BIT(14)	/* Unicast hash filtering */
#define MAC_CONTROL_HO       AR_BIT(15)	/* Hash only filtering */
#define MAC_CONTROL_PB       AR_BIT(16)	/* Pass Bad frames */
#define MAC_CONTROL_IF       AR_BIT(17)	/* Inverse filtering */
#define MAC_CONTROL_PR       AR_BIT(18)	/* promiscuous mode (valid frames
										   only) */
#define MAC_CONTROL_PM       AR_BIT(19)	/* pass multicast */
#define MAC_CONTROL_F        AR_BIT(20)	/* full-duplex */
#define MAC_CONTROL_DRO      AR_BIT(23)	/* Disable Receive Own */
#define MAC_CONTROL_HBD      AR_BIT(28)	/* heart-beat disabled (MUST BE
										   SET) */
#define MAC_CONTROL_BLE      AR_BIT(30)	/* big endian mode */
#define MAC_CONTROL_RA       AR_BIT(31)	/* receive all (valid and invalid
										   frames) */

#define MII_ADDR_BUSY        AR_BIT(0)
#define MII_ADDR_WRITE       AR_BIT(1)
#define MII_ADDR_REG_SHIFT   6
#define MII_ADDR_PHY_SHIFT   11
#define MII_DATA_SHIFT       0

#define FLOW_CONTROL_FCE     AR_BIT(1)

#define DMA_BUS_MODE_SWR       AR_BIT(0)	/* software reset */
#define DMA_BUS_MODE_BLE       AR_BIT(7)	/* big endian mode */
#define DMA_BUS_MODE_PBL_SHIFT 8	/* programmable burst length 32 */
#define DMA_BUS_MODE_DBO       AR_BIT(20)	/* big-endian descriptors */

#define DMA_STATUS_TI        AR_BIT(0)	/* transmit interrupt */
#define DMA_STATUS_TPS       AR_BIT(1)	/* transmit process stopped */
#define DMA_STATUS_TU        AR_BIT(2)	/* transmit buffer unavailable */
#define DMA_STATUS_TJT       AR_BIT(3)	/* transmit buffer timeout */
#define DMA_STATUS_UNF       AR_BIT(5)	/* transmit underflow */
#define DMA_STATUS_RI        AR_BIT(6)	/* receive interrupt */
#define DMA_STATUS_RU        AR_BIT(7)	/* receive buffer unavailable */
#define DMA_STATUS_RPS       AR_BIT(8)	/* receive process stopped */
#define DMA_STATUS_ETI       AR_BIT(10)	/* early transmit interrupt */
#define DMA_STATUS_FBE       AR_BIT(13)	/* fatal bus interrupt */
#define DMA_STATUS_ERI       AR_BIT(14)	/* early receive interrupt */
#define DMA_STATUS_AIS       AR_BIT(15)	/* abnormal interrupt summary */
#define DMA_STATUS_NIS       AR_BIT(16)	/* normal interrupt summary */
#define DMA_STATUS_RS_SHIFT  17	/* receive process state */
#define DMA_STATUS_TS_SHIFT  20	/* transmit process state */
#define DMA_STATUS_EB_SHIFT  23	/* error bits */

#define DMA_CONTROL_SR       AR_BIT(1)	/* start receive */
#define DMA_CONTROL_ST       AR_BIT(13)	/* start transmit */
#define DMA_CONTROL_SF       AR_BIT(21)	/* store and forward */


typedef struct {
	volatile unsigned int status;	// OWN, Device control and status.
	volatile unsigned int devcs;	// pkt Control bits + Length
	volatile unsigned int addr;	// Current Address.
	volatile unsigned int descr;	// Next descriptor in chain.
} ar2313_descr_t;


#endif							// __ARUBA_DMA_H__
