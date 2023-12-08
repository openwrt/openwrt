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

#ifndef _MAC_OUTSRC_DEF_H_
#define _MAC_OUTSRC_DEF_H_

// for other submodule ref definition defined here
/*--------------------Define -------------------------------------------*/
/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/

/**
 * @enum rtw_mac_src_cmd_ofld
 *
 * @brief rtw_mac_src_cmd_ofld
 *
 * @var rtw_mac_src_cmd_ofld::MAC_AX_BB_CMD_OFLD
 * Please Place Description here.
 * @var rtw_mac_src_cmd_ofld::MAC_AX_RF_CMD_OFLD
 * Please Place Description here.
 * @var rtw_mac_src_cmd_ofld::MAC_AX_MAC_CMD_OFLD
 * Please Place Description here.
 * @var rtw_mac_src_cmd_ofld::MAC_RF_DDIE_CMD_OFLD
 * Please Place Description here.
 * @var rtw_mac_src_cmd_ofld::MAC_AX_OTHER_CMD_OFLD
 * Please Place Description here.
 */
enum rtw_mac_src_cmd_ofld {
	RTW_MAC_BB_CMD_OFLD = 0,
	RTW_MAC_RF_CMD_OFLD,
	RTW_MAC_MAC_CMD_OFLD,
	RTW_MAC_RF_DDIE_CMD_OFLD,
	RTW_MAC_OTHER_CMD_OFLD
};

/**
 * @enum rtw_mac_cmd_type_ofld
 *
 * @brief rtw_mac_cmd_type_ofld
 *
 * @var rtw_mac_cmd_type_ofld::MAC_AX_WRITE_OFLD
 * Please Place Description here.
 * @var rtw_mac_cmd_type_ofld::MAC_AX_POLLING_OFLD
 * Please Place Description here.
 * @var rtw_mac_cmd_type_ofld::MAC_AX_DELAY_OFLD
 * Please Place Description here.
 */
enum rtw_mac_cmd_type_ofld {
	RTW_MAC_WRITE_OFLD = 0,
	RTW_MAC_COMPARE_OFLD,
	RTW_MAC_DELAY_OFLD,
	RTW_MAC_MOVE_OFLD
};

/**
 * @enum mac_ax_cmd_id
 *
 * @brief mac_ax_cmd_id
 *
 * @var mac_ax_host_rpr_mode::MAC_AX_ID_0
 * Please Place Description here.
 * @var mac_ax_host_rpr_mode::MAC_AX_ID_1
 * Please Place Description here.
 */
enum mac_ax_cmd_id {
	MAC_AX_ID_0 = 0,
	MAC_AX_ID_1
};

/**
 * @enum rtw_mac_rf_path
 *
 * @brief rtw_mac_rf_path
 *
 * @var rtw_mac_rf_path::RF_PATH_A
 * Please Place Description here.
 * @var rtw_mac_rf_path::RF_PATH_B
 * Please Place Description here.
 * @var rtw_mac_rf_path::RF_PATH_C
 * Please Place Description here.
 * @var rtw_mac_rf_path::RF_PATH_D
 * Please Place Description here.
 */
enum rtw_mac_rf_path {
	RTW_MAC_RF_PATH_A = 0,   //Radio Path A
	RTW_MAC_RF_PATH_B,	//Radio Path B
	RTW_MAC_RF_PATH_C,	//Radio Path C
	RTW_MAC_RF_PATH_D,	//Radio Path D
};

enum rtw_fw_cap {
	FW_CAP_IO_OFLD = BIT(0),
};

/*--------------------Define Struct-------------------------------------*/

/**
 * @struct rtw_mac_cmd
 * @brief rtw_mac_cmd
 *
 * @var rtw_mac_cmd::src
 * Please Place Description here.
 * @var rtw_mac_cmd::type
 * Please Place Description here.
 * @var rtw_mac_cmd::lc
 * Please Place Description here.
 * @var rtw_mac_cmd::rf_path
 * Please Place Description here.
 * @var rtw_mac_cmd::offset
 * Please Place Description here.
 * @var rtw_mac_cmd::id
 * Please Place Description here.
 * @var rtw_mac_cmd::value
 * Please Place Description here.
 * @var rtw_mac_cmd::mask
 * Please Place Description here.
 */
struct rtw_mac_cmd {
	enum rtw_mac_src_cmd_ofld src;
	enum rtw_mac_cmd_type_ofld type;
	u8 lc;
	enum rtw_mac_rf_path rf_path;
	u16 offset;
	u16 id;
	u32 value;
	u32 mask;
};

/**
 * @struct rtw_mac_cmd_v1
 * @brief rtw_mac_cmd_v1
 *
 * @var rtw_mac_cmd::src0
 * Please Place Description here.
 * @var rtw_mac_cmd::src1
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::type
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::lc
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::rf_path
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::offset0
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::value0
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::mask0
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::offset1
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::rsvd0
 * Please Place Description here.
 * @var rtw_mac_cmd_v1::mask1
 * Please Place Description here.
 */
struct rtw_mac_cmd_v1 {
	enum rtw_mac_src_cmd_ofld src0;
	enum rtw_mac_rf_path rf_path0;
	enum rtw_mac_src_cmd_ofld src1;
	enum rtw_mac_rf_path rf_path1;
	enum rtw_mac_cmd_type_ofld type;
	u8 lc;
	u16 offset0;
	u16 offset1;
	u32 mask0;
	u32 value;
};
#endif //_MAC_OUTSRC_DEF_H_
