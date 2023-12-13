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

#ifndef _MAC_AX_RRSR_H_
#define _MAC_AX_RRSR_H_
#include "../../type.h"
#if MAC_AX_8852C_SUPPORT

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RRSR
 * @{
 */
/**
 * @brief mac_get_rrsr_cfg_8852c
 *
 * @param *adapter
 * @param *mac_ax_rrsr_cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rrsr_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RRSR
 * @{
 */
/**
 * @brief mac_set_rrsr_cfg_8852c
 *
 * @param *adapter
 * @param *mac_ax_rrsr_cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rrsr_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RRSR
 * @{
 */
/**
 * @brief mac_get_cts_rrsr_cfg_8852c
 *
 * @param *adapter
 * @param mac_ax_cts_rrsr_cfg
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_get_cts_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			       struct mac_ax_cts_rrsr_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RRSR
 * @{
 */
/**
 * @brief mac_set_cts_rrsr_cfg_8852c
 *
 * @param *adapter
 * @param mac_ax_cts_rrsr_cfg
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_cts_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			       struct mac_ax_cts_rrsr_cfg *cfg);
/**
 * @}
 * @}
 */

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
