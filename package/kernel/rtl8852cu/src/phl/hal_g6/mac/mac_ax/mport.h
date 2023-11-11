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

#ifndef _MAC_AX_MPORT_H_
#define _MAC_AX_MPORT_H_

#include "../type.h"

/*--------------------Define -------------------------------------------*/
#define BCN_ERLY_DEF 160
#define BCN_SETUP_DEF 4
#define BCN_HOLD_DEF 400
#define BCN_MASK_DEF 0
#define TBTT_ERLY_DEF 5
#define TBTT_AGG_DEF 1
#define BCN_SET_TO_US 32
#define TU_TO_BCN_SET (1024 / BCN_SET_TO_US)
#define BCN_ERLY_SET_DLY 10

#define PTCL_DBG_BCNQ_NUM0 8
#define PTCL_DBG_BCNQ_NUM1 9
#define PTCL_DBG_DLY_US 100

#define TU_UNIT 1024
#define MS_UNIT 1000
#define PORT_DLY_TU_CNT_LMT 200
#define PORT_DLY_US_CNT_LMT 200

#define MAX_BCN_ITVL 0x639C

#define CHK_BCNQ_CNT 200
#define BCNQ_EMP_CONT_CNT 2

#define LBK_PLCP_DLY_DEF 0x28
#define LBK_PLCP_DLY_FPGA 0x46

#define BCN_FAST_DRP_TBTT 1

/* 50% hold time , 30% beacon early */
#define BCN_ERLY_RATIO 3
#define BCN_HOLD_RATIO 5
#define BCN_ITVL_RATIO 10

#define PORT_H2C_DLY_US 10
#define PORT_H2C_DLY_CNT 20000

#define TBTT_SHIFT_OFST_MSB 0x800

/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
enum port_stat {
	PORT_ST_DIS = 0,
	PORT_ST_NOLINK,
	PORT_ST_ADHOC,
	PORT_ST_INFRA,
	PORT_ST_AP,
};

enum C2H_MPORT_RET_CODE {
	C2H_MPORT_RET_SUCCESS = 0,
	C2H_MPORT_RET_PENDING,
	C2H_MPORT_RET_BAND_ERR,
	C2H_MPORT_RET_PORT_ERR,
	C2H_MPORT_RET_MBID_ERR,
	C2H_MPORT_RET_NO_CONTENT,
	C2H_MPORT_RET_TSF32TOG_C2H_FAIL,
	C2H_MPORT_RET_CFG_TYPE_ERR,
	C2H_MPORT_RET_SET_CFG_ILEGAL_EN,
	C2H_MPORT_RET_SET_CFG_ARDY_DIS,
	C2H_MPORT_RET_INPUT_ILEGAL,
	C2H_MPORT_RET_CHK_BELW_LMT,
	C2H_MPORT_RET_CHK_OVER_LMT,
	C2H_MPORT_RET_SUB_SPC_ERR,
	C2H_MPORT_RET_POLL_TO,
	C2H_MPORT_RET_TSF_NOT_RUN,
	C2H_MPORT_RET_MALLOC_FAIL,
	C2H_MPORT_RET_DEINIT_FAIL,
	C2H_MPORT_RET_PKT_DROP,
	C2H_MPORT_RET_ADD_PSTIMER,
	C2H_MPORT_RET_CHK_BCNQ_TO,
	C2H_MPORT_RET_CHK_BCNQ_FAIL,
	C2H_MPORT_RET_SEND_C2H_INIT,
	C2H_MPORT_RET_SEND_C2H_CFG,
};

/*--------------------Define Struct-------------------------------------*/

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */
/**
 * @brief dly_port_tu
 *
 * @param *adapter
 * @param band
 * @param port
 * @param dly_tu
 * @return Please Place Description here.
 * @retval u32
 */
u32 dly_port_tu(struct mac_ax_adapter *adapter, u8 band, u8 port, u32 dly_tu);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief dly_port_us
 *
 * @param *adapter
 * @param band
 * @param port
 * @param dly_us
 * @return Please Place Description here.
 * @retval u32
 */
u32 dly_port_us(struct mac_ax_adapter *adapter, u8 band, u8 port, u32 dly_us);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief rst_port_info
 *
 * @param *adapter
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */
u32 rst_port_info(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief mac_port_cfg
 *
 * @param *adapter
 * @param type
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_port_cfg(struct mac_ax_adapter *adapter,
		 enum mac_ax_port_cfg_type type,
		 struct mac_ax_port_cfg_para *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief mac_port_init
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_port_init(struct mac_ax_adapter *adapter,
		  struct mac_ax_port_init_para *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief mac_tsf_sync
 *
 * @param *adapter
 * @param from_port
 * @param to_port
 * @param sync_offset
 * @param action
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_tsf_sync(struct mac_ax_adapter *adapter, u8 from_port, u8 to_port,
		 s32 sync_offset, enum mac_ax_tsf_sync_act action);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief mac_parse_bcn_stats_c2h
 *
 * @param *adapter
 * @param *content
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_parse_bcn_stats_c2h(struct mac_ax_adapter *adapter,
			    u8 *content, struct mac_ax_bcn_cnt *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup MultiPort
 * @{
 */

/**
 * @brief get_bcn_stats_event
 *
 * @param *adapter
 * @param *c2h
 * @param *id
 * @param *c2h_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_bcn_stats_event(struct mac_ax_adapter *adapter,
			struct rtw_c2h_info *c2h,
			enum phl_msg_evt_id *id, u8 *c2h_info);
/**
 * @}
 * @}
 */

u32 mac_tsf32_togl_h2c(struct mac_ax_adapter *adapter,
		       struct mac_ax_t32_togl_info *info);

u32 mac_get_t32_togl_rpt(struct mac_ax_adapter *adapter,
			 struct mac_ax_t32_togl_rpt *ret_rpt);

u32 get_bp_idx(u8 band, u8 port);

u32 mport_info_init(struct mac_ax_adapter *adapter);

u32 mport_info_exit(struct mac_ax_adapter *adapter);

#endif
