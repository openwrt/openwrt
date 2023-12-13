/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_TCPIP_CHECKSUM_OFFLOAD_H_
#define _MAC_AX_TCPIP_CHECKSUM_OFFLOAD_H_

#include "../type.h"
#include "../mac_ax.h"

/**
 * @struct mac_ax_en_tcpipchksum
 * @brief mac_ax_en_tcpipchksum
 *
 * @var mac_ax_en_tcpipchksum::en_tx_chksum_ofd
 * Please Place Description here.
 * @var mac_ax_en_tcpipchksum::en_rx_chksum_ofd
 * Please Place Description here.
 * @var mac_ax_en_tcpipchksum::rsvd0
 * Please Place Description here.
 */
struct mac_ax_en_tcpipchksum {
	/* dword0 */
	u32 en_tx_chksum_ofd: 1;
	u32 en_rx_chksum_ofd: 1;
	u32 rsvd0: 30;
};

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_ShortCut
 * @{
 */
/**
 * @brief mac_tcpip_chksum_ofd
 *
 * @param *adapter
 * @param en_tx_chksum_ofd
 * @param en_rx_chksum_ofd
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_tcpip_chksum_ofd(struct mac_ax_adapter *adapter,
			 u8 en_tx_chksum_ofd,
			 u8 en_rx_chksum_ofd);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_ShortCut
 * @{
 */

/**
 * @brief mac_chk_rx_tcpip_chksum_ofd
 *
 * @param *adapter
 * @param chksum_status
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_chk_rx_tcpip_chksum_ofd(struct mac_ax_adapter *adapter,
				u8 chksum_status);
/**
 * @}
 * @}
 */

#define MAC_AX_CHKSUM_OFD_TX	0x1
#define MAC_AX_CHKSUM_OFD_RX	0x2

#define MAC_AX_CHKOFD_TCP_CHKSUM_ERR	BIT(4)
#define MAC_AX_CHKOFD_RX_IS_TCP_UDP		BIT(6)
#define MAC_AX_CHKOFD_RX_IPV			BIT(5)
#define MAC_AX_CHKOFD_TCP_CHKSUM_VLD	BIT(7)

#endif
