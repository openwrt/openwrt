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

#ifndef _MAC_AX_STATUS_H_
#define _MAC_AX_STATUS_H_

#include "../type.h"
#include "trxcfg.h"

/**
 * @brief mac_get_ft_status
 *
 * @param *adapter
 * @param mac_ft
 * @param *stat
 * @param *buf
 * @param size
 * @param *ret_size
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_ft_status(struct mac_ax_adapter *adapter,
		      enum mac_ax_feature mac_ft, enum mac_ax_status *stat,
		      u8 *buf, const u32 size, u32 *ret_size);

/**
 * @brief mac_get_ft
 *
 * @param *adapter
 * @param mac_ft
 * @return Please Place Description here.
 * @retval  mac_ax_ft_status
 */

struct mac_ax_ft_status *mac_get_ft(struct mac_ax_adapter *adapter,
				    enum mac_ax_feature mac_ft);

/**
 * @brief set_hw_ch_busy_cnt
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_hw_ch_busy_cnt(struct mac_ax_adapter *adapter,
		       struct mac_ax_ch_busy_cnt_cfg *cfg);

/**
 * @brief get_hw_ch_stat_cnt
 *
 * @param *adapter
 * @param *cnt
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_hw_ch_stat_cnt(struct mac_ax_adapter *adapter,
		       struct mac_ax_ch_stat_cnt *cnt);

#endif
