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

#ifndef _MAC_AX_DBCC_H_
#define _MAC_AX_DBCC_H_

#include "../type.h"
#include "trxcfg.h"
#include "sta_sch.h"

/*--------------------Define -------------------------------------------*/
#define DBCC_INFO_SIZE sizeof(struct mac_ax_dbcc_info)
#define DBCC_WMM_LIST_SIZE 32

#define SCH_TX_EN_SIZE sizeof(struct mac_ax_sch_tx_en)
#define PPDU_RPT_CFG_SIZE sizeof(struct mac_ax_phy_rpt_cfg)
#define CHINFO_CFG_SIZE sizeof(struct mac_ax_phy_rpt_cfg)

#define DBCC_BAND_BIT BIT7
#define DBCC_PORT_MASK 0xF
#define DBCC_BP_INVALID 0xFF

#define DBCC_CHK_NTFY_CNT 20000
#define DBCC_CHK_NTFY_DLY 50

/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
/*--------------------Define Struct-------------------------------------*/
/*--------------------Function Prototype--------------------------------*/

/**
 * @brief dbcc_info_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbcc_info_init(struct mac_ax_adapter *adapter);

/**
 * @brief dbcc_info_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbcc_info_exit(struct mac_ax_adapter *adapter);

/**
 * @brief rst_dbcc_info
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 rst_dbcc_info(struct mac_ax_adapter *adapter);

/**
 * @brief dbcc_wmm_add_macid
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbcc_wmm_add_macid(struct mac_ax_adapter *adapter,
		       struct mac_ax_role_info *info);

/**
 * @brief dbcc_wmm_rm_macid
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 dbcc_wmm_rm_macid(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info);

/**
 * @brief mac_dbcc_trx_ctrl
 *
 * @param *adapter
 * @param band
 * @param pause
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbcc_trx_ctrl(struct mac_ax_adapter *adapter,
		      enum mac_ax_band band, u8 pause);

/**
 * @brief mac_dbcc_enable
 *
 * @param *adapter
 * @param *info
 * @param dbcc_en
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbcc_enable(struct mac_ax_adapter *adapter,
		    struct mac_ax_trx_info *info, u8 dbcc_en);

/**
 * @brief mac_dbcc_pre_cfg
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbcc_pre_cfg(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info);

/**
 * @brief mac_dbcc_cfg
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbcc_cfg(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info);

/**
 * @brief mac_dbcc_move_wmm
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dbcc_move_wmm(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info);

#endif
