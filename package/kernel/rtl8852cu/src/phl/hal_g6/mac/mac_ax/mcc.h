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

#ifndef _MAC_AX_MCC_H_
#define _MAC_AX_MCC_H_

#include "../type.h"
#include "fwcmd.h"

#define MCC_GROUP_ID_MAX 3

/**
 * @addtogroup MCC
 * @{
 */
/**
 * @brief mac_reset_mcc_group
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_reset_mcc_group(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_reset_mcc_request
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_reset_mcc_request(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_add_mcc
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_add_mcc(struct mac_ax_adapter *adapter, struct mac_ax_mcc_role *info);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_start_mcc
 *
 * @param *adapter
 * @param group
 * @param macid
 * @param tsf_high
 * @param tsf_low
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_start_mcc(struct mac_ax_adapter *adapter,
		  struct mac_ax_mcc_start *info);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_stop_mcc
 *
 * @param *adapter
 * @param group
 * @param macid
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_stop_mcc(struct mac_ax_adapter *adapter, u8 group, u8 macid,
		 u8 prev_groups);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_del_mcc_group
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_del_mcc_group(struct mac_ax_adapter *adapter, u8 group,
		      u8 prev_groups);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_mcc_request_tsf
 *
 * @param *adapter
 * @param group
 * @param macid_x
 * @param macid_y
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mcc_request_tsf(struct mac_ax_adapter *adapter, u8 group,
			u8 macid_x, u8 macid_y);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_mcc_macid_bitmap
 *
 * @param *adapter
 * @param group
 * @param macid
 * @param *bitmap
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mcc_macid_bitmap(struct mac_ax_adapter *adapter, u8 group,
			 u8 macid, u8 *bitmap, u8 len);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_mcc_sync_enable
 *
 * @param *adapter
 * @param group
 * @param source
 * @param target
 * @param offset
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mcc_sync_enable(struct mac_ax_adapter *adapter, u8 group,
			u8 source, u8 target, u8 offset);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_mcc_set_duration
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mcc_set_duration(struct mac_ax_adapter *adapter,
			 struct mac_ax_mcc_duration_info *info);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_get_mcc_status_rpt
 *
 * @param *adapter
 * @param group
 * @param *status
 * @param *tsf_high
 * @param *tsf_low
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_mcc_status_rpt(struct mac_ax_adapter *adapter,
			   u8 group, u8 *status, u32 *tsf_high, u32 *tsf_low);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_get_mcc_tsf_rpt
 *
 * @param *adapter
 * @param group
 * @param *tsf_x_high
 * @param *tsf_x_low
 * @param *tsf_y_high
 * @param *tsf_y_low
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_mcc_tsf_rpt(struct mac_ax_adapter *adapter, u8 group,
			u32 *tsf_x_high, u32 *tsf_x_low,
			u32 *tsf_y_high, u32 *tsf_y_low);
/**
 * @}
 */
u32 mac_get_mcc_group(struct mac_ax_adapter *adapter, u8 *pget_group);
/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_add_mcc_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_add_mcc_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_start_mcc_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_start_mcc_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_stop_mcc_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_stop_mcc_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_del_mcc_group_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_del_mcc_group_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_mcc_request_tsf_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_mcc_request_tsf_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_mcc_macid_bitmap_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_mcc_macid_bitmap_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_mcc_sync_enable_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_mcc_sync_enable_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

/**
 * @addtogroup MCC
 * @{
 */

/**
 * @brief mac_check_mcc_set_duration_done
 *
 * @param *adapter
 * @param group
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_mcc_set_duration_done(struct mac_ax_adapter *adapter, u8 group);
/**
 * @}
 */

#endif
