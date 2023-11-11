/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation. All rights reserved.
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

#ifndef _MAC_AX_NAN_H_
#define _MAC_AX_NAN_H_

#include "../type.h"
#include "fwcmd.h"

/*--------------------Define ----------------------------------------*/

/*--------------------Define Struct-------------------------------------*/

/*--------------------Funciton declaration----------------------------*/

/**
 * @brief mac_get_act_schedule_id
 *
 * @param *adapter
 * @param *act_ack_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_act_schedule_id(struct mac_ax_adapter *adapter,
			    struct mac_ax_act_ack_info *act_ack_info);

/**
 * @brief mac_check_cluster_info
 *
 * @param *adapter
 * @param *mac_ax_nan_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_cluster_info(struct mac_ax_adapter *adapter, struct mac_ax_nan_info *cluster_info);

/**
 * @brief mac_nan_act_schedule_req
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_act_schedule_req(struct mac_ax_adapter *adapter, struct mac_ax_nan_sched_info *info);

/**
 * @brief mac_nan_bcn_req
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_bcn_req(struct mac_ax_adapter *adapter, struct mac_ax_nan_bcn *info);

/**
 * @brief mac_nan_func_ctrl
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_func_ctrl(struct mac_ax_adapter *adapter, struct mac_ax_nan_func_info *info);

/**
 * @brief mac_nan_de_info
 *
 * @param *adapter
 * @param status
 * @param loc_bcast_sdf
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_de_info(struct mac_ax_adapter *adapter, u8 status, u8 loc_bcast_sdf);

/**
 * @brief mac_nan_join_cluster
 *
 * @param *adapter
 * @param is_allow
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_join_cluster(struct mac_ax_adapter *adapter, u8 is_allow);

/**
 * @brief mac_nan_pause_faw_tx
 *
 * @param *adapter
 * @param id_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_pause_faw_tx(struct mac_ax_adapter *adapter, u32 id_map);

/**
 * @brief mac_nan_get_cluster_info
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_nan_get_cluster_info(struct mac_ax_adapter *adapter, struct mac_ax_nan_info *cluster_info);

#endif