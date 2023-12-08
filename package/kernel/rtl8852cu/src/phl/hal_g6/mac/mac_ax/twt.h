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

#ifndef _MAC_AX_TWT_H_
#define _MAC_AX_TWT_H_

#include "../type.h"
#include "fwcmd.h"

#define TWT_INFO_SIZE (sizeof(struct mac_ax_twt_info))
#define TWT_DBG_INFO_SIZE 16 /* 1st dword of each TWT common info */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief twt_info_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 twt_info_init(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief twt_info_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 twt_info_exit(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief mac_twt_info_upd_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_twt_info_upd_h2c(struct mac_ax_adapter *adapter,
			 struct mac_ax_twt_para *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief mac_twt_act_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_twt_act_h2c(struct mac_ax_adapter *adapter,
		    struct mac_ax_twtact_para *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief mac_twt_staanno_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_twt_staanno_h2c(struct mac_ax_adapter *adapter,
			struct mac_ax_twtanno_para *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief mac_twt_wait_anno
 *
 * @param *adapter
 * @param *c2h_content
 * @param *upd_addr
 * @return Please Place Description here.
 * @retval void
 */
void mac_twt_wait_anno(struct mac_ax_adapter *adapter,
		       u8 *c2h_content, u8 *upd_addr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup PowerSaving
 * @{
 * @addtogroup TWT
 * @{
 */

/**
 * @brief mac_get_tsf
 *
 * @param *adapter
 * @param *tsf
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_tsf(struct mac_ax_adapter *adapter, struct mac_ax_port_tsf *tsf);
/**
 * @}
 * @}
 */

#endif
