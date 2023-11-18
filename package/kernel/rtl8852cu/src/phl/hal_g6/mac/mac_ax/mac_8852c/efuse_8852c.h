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

#ifndef _MAC_AX_EFUSE_8852C_H_
#define _MAC_AX_EFUSE_8852C_H_

#include "../efuse.h"

#if MAC_AX_8852C_SUPPORT

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief switch_efuse_bank_8852c
 *
 * @param *adapter
 * @param bank
 * @return Please Place Description here.
 * @retval u32
 */
u32 switch_efuse_bank_8852c(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_bank bank);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief enable_efuse_sw_pwr_cut_8852c
 *
 * @param *adapter
 * @param is_write
 * @return Please Place Description here.
 * @retval void
 */
void enable_efuse_sw_pwr_cut_8852c(struct mac_ax_adapter *adapter,
				   bool is_write);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief disable_efuse_sw_pwr_cut_8852c
 *
 * @param *adapter
 * @param is_write
 * @return Please Place Description here.
 * @retval void
 */
void disable_efuse_sw_pwr_cut_8852c(struct mac_ax_adapter *adapter,
				    bool is_write);
/**
 * @}
 */

#endif /* MAC_AX_8852C_SUPPORT */
#endif
