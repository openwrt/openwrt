/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Ethernet register definition
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef	__IDT_ETH_V_H__
#define	__IDT_ETH_V_H__

#include  <asm/idt-boards/rc32434/rc32434_eth.h> 

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





