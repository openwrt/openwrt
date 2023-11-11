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

#ifndef _MAC_AX_HWAMSDU_8852C_H_
#define _MAC_AX_HWAMSDU_8852C_H_

#include "../hwamsdu.h"
#include "../../type.h"
#include "../../mac_ax.h"

/**
 * @brief mac_enable_cut_hwamsdu_8852c
 *
 * @param *adapter
 * @param enable
 * @param aligned
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_enable_cut_hwamsdu_8852c(struct mac_ax_adapter *adapter,
				 u8 enable,
				 enum mac_ax_ex_shift aligned);

/**
 * @brief mac_cut_hwamsdu_chk_mpdu_len_8852c
 *
 * @param *adapter
 * @param enable
 * @param low_th
 * @param high_th
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cut_hwamsdu_chk_mpdu_len_en_8852c(struct mac_ax_adapter *adapter,
					  u8 enable,
					  u8 low_th,
					  u16 high_th);

#endif
