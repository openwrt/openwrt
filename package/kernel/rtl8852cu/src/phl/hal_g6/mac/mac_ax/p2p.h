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

#ifndef _MAC_AX_P2P_H_
#define _MAC_AX_P2P_H_

#include "../type.h"
#include "fwcmd.h"

/*--------------------Define -------------------------------------------*/
#define P2P_MAX_NUM 2
#define NOA_MAX_NUM 2
#define P2PID_INVALID 0xFF

#define MACID_BMAP_BYTE_SH 2 /* DWORD to Byte */
#define MACID_BMAP_BYTE_MSK 0x3
#define MACID_BMAP_BIT_SH 5 /* DWORD to bit */

/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
enum P2P_ACT {
	P2P_ACT_INIT = 0,
	P2P_ACT_UPD,
	P2P_ACT_RM,
	P2P_ACT_TERM
};

enum P2P_MACID_CTRL_TYPE {
	P2P_MACID_CTRL_JOIN = 0,
	P2P_MACID_CTRL_CLR,
	P2P_MACID_CTRL_CLR_ALL,
	P2P_MACID_CTRL_MAX
};

/*--------------------Define Struct-------------------------------------*/

/**
 * @brief get_wait_dack_p2pid
 *
 * @param *adapter
 * @param *p2pid
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_wait_dack_p2pid(struct mac_ax_adapter *adapter, u8 *p2pid);

/**
 * @brief p2p_info_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 p2p_info_init(struct mac_ax_adapter *adapter);

/**
 * @brief p2p_info_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 p2p_info_exit(struct mac_ax_adapter *adapter);

/**
 * @brief rst_p2p_info
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 rst_p2p_info(struct mac_ax_adapter *adapter);

/**
 * @brief mac_p2p_act_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_p2p_act_h2c(struct mac_ax_adapter *adapter,
		    struct mac_ax_p2p_act_info *info);

/**
 * @brief mac_p2p_macid_ctrl_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_p2p_macid_ctrl_h2c(struct mac_ax_adapter *adapter,
			   struct mac_ax_p2p_macid_info *info);

/**
 * @brief mac_get_p2p_stat
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_p2p_stat(struct mac_ax_adapter *adapter);

#endif
