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

#ifndef _MAC_AX_COEX_8852C_H_
#define _MAC_AX_COEX_8852C_H_

#include "../../mac_def.h"
#if MAC_AX_8852C_SUPPORT

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_coex_init
 *
 * @param *adapter
 * @param *coex
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_coex_init_8852c(struct mac_ax_adapter *adapter,
			struct mac_ax_coex *coex);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_cfg_gnt
 *
 * @param *adapter
 * @param *gnt_cfg
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_cfg_gnt_8852c(struct mac_ax_adapter *adapter,
		      struct mac_ax_coex_gnt *gnt_cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_get_gnt
 *
 * @param *adapter
 * @param *gnt_cfg
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_get_gnt_8852c(struct mac_ax_adapter *adapter,
		      struct mac_ax_coex_gnt *gnt_cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_cfg_plt
 *
 * @param *adapter
 * @param *plt
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_cfg_plt_8852c(struct mac_ax_adapter *adapter, struct mac_ax_plt *plt);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_write_coex_reg
 *
 * @param *adapter
 * @param offset
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_write_coex_reg_8852c(struct mac_ax_adapter *adapter,
			     const u32 offset, const u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_read_coex_reg
 *
 * @param *adapter
 * @param offset
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_read_coex_reg_8852c(struct mac_ax_adapter *adapter,
			    const u32 offset, u32 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_cfg_ctrl_path
 *
 * @param *adapter
 * @param wl
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_cfg_ctrl_path_8852c(struct mac_ax_adapter *adapter, u32 wl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BTCoex
 * @{
 */

/**
 * @brief mac_get_ctrl_path
 *
 * @param *adapter
 * @param *wl
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_get_ctrl_path_8852c(struct mac_ax_adapter *adapter, u32 *wl);
/**
 * @}
 * @}
 */
/**
 * @addtogroup Common
 * @{
 * @addtogroup Init
 * @{
 */

/**
 * @brief coex_mac_init_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 coex_mac_init_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup LTE_COEX
 * @{
 */

/**
 * @brief mac_write_lte_8852c
 *
 * @param *adapter
 * @param offset
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_lte_8852c(struct mac_ax_adapter *adapter,
			const u32 offset, u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup LTE_COEX
 * @{
 */

/**
 * @brief mac_read_lte_8852c
 *
 * @param *adapter
 * @param offset
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_lte_8852c(struct mac_ax_adapter *adapter,
		       const u32 offset, u32 *val);
/**
 * @}
 * @}
 */
#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
