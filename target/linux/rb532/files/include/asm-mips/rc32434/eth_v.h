#ifndef	__IDT_ETH_V_H__
#define	__IDT_ETH_V_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * Ethernet register definition.
 *
 * File   : $Id: eth.h,v 1.3 2002/06/06 18:34:04 astichte Exp $
 *
 * Author : Allen.Stichter@idt.com
 * Date   : 20020605
 * Update :
 *	    $Log: eth.h,v $
 *	    Revision 1.3  2002/06/06 18:34:04  astichte
 *	    Added XXX_PhysicalAddress and XXX_VirtualAddress
 *	
 *	    Revision 1.2  2002/06/05 18:19:46  astichte
 *	    Added
 *	
 *	    Revision 1.1  2002/05/29 17:33:22  sysarch
 *	    jba File moved from vcode/include/idt/acacia
 *
 ******************************************************************************/

#include  <asm/rc32434/eth.h> 

#define IS_TX_TOK(X)         (((X) & (1<<ETHTX_tok_b)) >> ETHTX_tok_b )   /* Transmit Okay    */
#define IS_TX_MP(X)          (((X) & (1<<ETHTX_mp_b))  >> ETHTX_mp_b )    /* Multicast        */
#define IS_TX_BP(X)          (((X) & (1<<ETHTX_bp_b))  >> ETHTX_bp_b )    /* Broadcast        */
#define IS_TX_UND_ERR(X)     (((X) & (1<<ETHTX_und_b)) >> ETHTX_und_b )   /* Transmit FIFO Underflow */
#define IS_TX_OF_ERR(X)      (((X) & (1<<ETHTX_of_b))  >> ETHTX_of_b )    /* Oversized frame  */
#define IS_TX_ED_ERR(X)      (((X) & (1<<ETHTX_ed_b))  >> ETHTX_ed_b )    /* Excessive deferral  */
#define IS_TX_EC_ERR(X)      (((X) & (1<<ETHTX_ec_b))  >> ETHTX_ec_b)     /* Excessive collisions  */
#define IS_TX_LC_ERR(X)      (((X) & (1<<ETHTX_lc_b))  >> ETHTX_lc_b )    /* Late Collision   */
#define IS_TX_TD_ERR(X)      (((X) & (1<<ETHTX_td_b))  >> ETHTX_td_b )    /* Transmit deferred*/
#define IS_TX_CRC_ERR(X)     (((X) & (1<<ETHTX_crc_b)) >> ETHTX_crc_b )   /* CRC Error        */
#define IS_TX_LE_ERR(X)      (((X) & (1<<ETHTX_le_b))  >>  ETHTX_le_b )    /* Length Error     */

#define TX_COLLISION_COUNT(X) (((X) & ETHTX_cc_m)>>ETHTX_cc_b)  /* Collision Count  */

#define IS_RCV_ROK(X)        (((X) & (1<<ETHRX_rok_b)) >> ETHRX_rok_b)    /* Receive Okay     */
#define IS_RCV_FM(X)         (((X) & (1<<ETHRX_fm_b))  >> ETHRX_fm_b)     /* Is Filter Match  */
#define IS_RCV_MP(X)         (((X) & (1<<ETHRX_mp_b))  >> ETHRX_mp_b)     /* Is it MP         */
#define IS_RCV_BP(X)         (((X) & (1<<ETHRX_bp_b))  >> ETHRX_bp_b)     /* Is it BP         */
#define IS_RCV_VLT(X)        (((X) & (1<<ETHRX_vlt_b)) >> ETHRX_vlt_b)    /* VLAN Tag Detect  */
#define IS_RCV_CF(X)         (((X) & (1<<ETHRX_cf_b))  >> ETHRX_cf_b)     /* Control Frame    */
#define IS_RCV_OVR_ERR(X)    (((X) & (1<<ETHRX_ovr_b)) >> ETHRX_ovr_b)    /* Receive Overflow */
#define IS_RCV_CRC_ERR(X)    (((X) & (1<<ETHRX_crc_b)) >> ETHRX_crc_b)    /* CRC Error        */
#define IS_RCV_CV_ERR(X)     (((X) & (1<<ETHRX_cv_b))  >> ETHRX_cv_b)     /* Code Violation   */
#define IS_RCV_DB_ERR(X)     (((X) & (1<<ETHRX_db_b))  >> ETHRX_db_b)     /* Dribble Bits     */
#define IS_RCV_LE_ERR(X)     (((X) & (1<<ETHRX_le_b))  >> ETHRX_le_b)     /* Length error     */
#define IS_RCV_LOR_ERR(X)    (((X) & (1<<ETHRX_lor_b)) >> ETHRX_lor_b)    /* Length Out of Range */
#define IS_RCV_CES_ERR(X)    (((X) & (1<<ETHRX_ces_b)) >> ETHRX_ces_b)  /* Preamble error   */
#define RCVPKT_LENGTH(X)     (((X) & ETHRX_length_m) >> ETHRX_length_b)   /* Length of the received packet */
#endif	// __IDT_ETH_V_H__





