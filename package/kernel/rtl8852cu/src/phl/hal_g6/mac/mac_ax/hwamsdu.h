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

#ifndef _MAC_AX_HWAMSDU_H_
#define _MAC_AX_HWAMSDU_H_

#include "../type.h"
#include "../mac_ax.h"

#define MAX_LENGTH_ENUM 7
#define HW_AMSDU_MACID_ENABLE 0x1

/**
 * @struct mac_ax_en_amsdu_cut
 * @brief mac_ax_en_amsdu_cut
 *
 * @var mac_ax_en_amsdu_cut::enable
 * Please Place Description here.
 * @var mac_ax_en_amsdu_cut::low_th
 * Please Place Description here.
 * @var mac_ax_en_amsdu_cut::high_th
 * Please Place Description here.
 * @var mac_ax_en_amsdu_cut::aligned
 * Please Place Description here.
 * @var mac_ax_en_amsdu_cut::rsvd0
 * Please Place Description here.
 */
struct mac_ax_en_amsdu_cut {
	/* dword0 */
	u32 enable: 1;
	u32 low_th: 8;
	u32 high_th: 16;
	u32 aligned: 2;
	u32 rsvd0: 5;
};

/**
 * @struct mac_ax_en_hwamsdu
 * @brief mac_ax_en_hwamsdu
 *
 * @var mac_ax_en_hwamsdu::enable
 * Please Place Description here.
 * @var mac_ax_en_hwamsdu::max_num
 * Please Place Description here.
 * @var mac_ax_en_hwamsdu::en_single_amsdu
 * Please Place Description here.
 * @var mac_ax_en_hwamsdu::en_last_amsdu_padding
 * Please Place Description here.
 * @var mac_ax_en_hwamsdu::rsvd0
 * Please Place Description here.
 */
struct mac_ax_en_hwamsdu {
	/* dword0 */
	u32 enable: 1;
	u32 max_num: 2;
	u32 en_single_amsdu:1;
	u32 en_last_amsdu_padding:1;
	u32 rsvd0: 27;
};

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_ShortCut
 * @{
 */

/**
 * @brief mac_enable_hwmasdu
 *
 * @param *adapter
 * @param enable
 * @param max_num
 * @param en_single_amsdu
 * @param en_last_amsdu_padding
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_enable_hwamsdu(struct mac_ax_adapter *adapter,
		       u8 enable,
		       enum mac_ax_amsdu_pkt_num max_num,
		       u8 en_single_amsdu,
		       u8 en_last_amsdu_padding);
/**
 * @}
 * @}
 */
/**
 * @brief mac_hwamsdu_fwd_search_en
 *
 * @param *adapter
 * @param enable
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hwamsdu_fwd_search_en(struct mac_ax_adapter *adapter, u8 enable);
/**
 * @}
 * @}
 */
/**
 * @brief mac_enable_hwmasdu
 *
 * @param *adapter
 * @param macid
 * @param enable
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hwamsdu_macid_en(struct mac_ax_adapter *adapter, u8 macid, u8 enable);
/**
 * @}
 * @}
 */
/**
 * @brief mac_enable_hwmasdu
 *
 * @param *adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hwamsdu_get_macid_en(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_enable_hwmasdu
 *
 * @param *adapter
 * @param macid
 * @param amsdu_max_len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hwamsdu_max_len(struct mac_ax_adapter *adapter, u8 macid, u8 amsdu_max_len);
/**
 * @}
 * @}
 */

/**
 * @brief mac_enable_hwmasdu
 *
 * @param *adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hwamsdu_get_max_len(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

#endif
