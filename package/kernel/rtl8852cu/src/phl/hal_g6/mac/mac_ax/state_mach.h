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

#ifndef _MAC_AX_STATE_MACH_H_
#define _MAC_AX_STATE_MACH_H_

/**
 * @struct mac_ax_state_mach
 * @brief mac_ax_state_mach
 *
 * @var mac_ax_state_mach::pwr
 * Please Place Description here.
 * @var mac_ax_state_mach::fwdl
 * Please Place Description here.
 * @var mac_ax_state_mach::efuse
 * Please Place Description here.
 * @var mac_ax_state_mach::read_request
 * Please Place Description here.
 * @var mac_ax_state_mach::write_request
 * Please Place Description here.
 * @var mac_ax_state_mach::conf_request
 * Please Place Description here.
 * @var mac_ax_state_mach::write_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::conf_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::read_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::pkt_ofld
 * Please Place Description here.
 * @var mac_ax_state_mach::efuse_ofld
 * Please Place Description here.
 * @var mac_ax_state_mach::macid_pause
 * Please Place Description here.
 * @var mac_ax_state_mach::mcc_group
 * Please Place Description here.
 * @var mac_ax_state_mach::mcc_request
 * Please Place Description here.
 * @var mac_ax_state_mach::fw_rst
 * Please Place Description here.
 * @var mac_ax_state_mach::aoac_rpt
 * Please Place Description here.
 */

/**
 * @struct mac_ax_state_mach
 * @brief mac_ax_state_mach
 *
 * @var mac_ax_state_mach::pwr
 * Please Place Description here.
 * @var mac_ax_state_mach::fwdl
 * Please Place Description here.
 * @var mac_ax_state_mach::efuse
 * Please Place Description here.
 * @var mac_ax_state_mach::read_request
 * Please Place Description here.
 * @var mac_ax_state_mach::write_request
 * Please Place Description here.
 * @var mac_ax_state_mach::conf_request
 * Please Place Description here.
 * @var mac_ax_state_mach::write_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::conf_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::read_h2c
 * Please Place Description here.
 * @var mac_ax_state_mach::pkt_ofld
 * Please Place Description here.
 * @var mac_ax_state_mach::efuse_ofld
 * Please Place Description here.
 * @var mac_ax_state_mach::macid_pause
 * Please Place Description here.
 * @var mac_ax_state_mach::mcc_group
 * Please Place Description here.
 * @var mac_ax_state_mach::mcc_request
 * Please Place Description here.
 * @var mac_ax_state_mach::fw_rst
 * Please Place Description here.
 * @var mac_ax_state_mach::aoac_rpt
 * Please Place Description here.
 * @var mac_ax_state_mach::p2p_stat
 * Please Place Description here.
 * @var mac_ax_state_mach::nan_stat
 * Please Place Description here.
 */
struct mac_ax_state_mach {
#define MAC_AX_PWR_OFF 0
#define MAC_AX_PWR_ON 1
#define MAC_AX_PWR_PRE_OFF 2
#define MAC_AX_PWR_ERR 3
	u8 pwr;
#define MAC_AX_FWDL_IDLE 0
#define MAC_AX_FWDL_CPU_ON 1
#define MAC_AX_FWDL_H2C_PATH_RDY 2
#define MAC_AX_FWDL_PATH_RDY 3
#define MAC_AX_FWDL_INIT_RDY 4
	u8 fwdl;
#define MAC_AX_EFUSE_IDLE 0
#define MAC_AX_EFUSE_PHY 1
#define MAC_AX_EFUSE_LOG_MAP 2
#define MAC_AX_EFUSE_LOG_MASK 3
#define MAC_AX_EFUSE_MAX 4
	u8 efuse;
#define MAC_AX_OFLD_REQ_IDLE 0
#define MAC_AX_OFLD_REQ_H2C_SENT 1
#define MAC_AX_OFLD_REQ_CREATED 2
#define MAC_AX_OFLD_REQ_CLEANED 3
	u8 read_request;
	u8 write_request;
	u8 conf_request;
#define MAC_AX_CMD_OFLD_IDLE 0
#define MAC_AX_CMD_OFLD_PROC 1
#define MAC_AX_CMD_OFLD_SENDING 2
#define MAC_AX_CMD_OFLD_RCVD 3
	u8 cmd_state;
#define MAC_AX_OFLD_H2C_IDLE 0
#define MAC_AX_OFLD_H2C_SENDING 1
#define MAC_AX_OFLD_H2C_RCVD 2
#define MAC_AX_OFLD_H2C_ERROR 4
	u8 write_h2c;
	u8 conf_h2c;
#define MAC_AX_OFLD_H2C_DONE 3
	u8 read_h2c;
	u8 pkt_ofld;
	u8 efuse_ofld;
	u8 macid_pause;
	u8 disable_rf;
	u8 sch_tx_en_ofld;
	u8 macid_pause_sleep;
#define MAC_AX_MCC_EMPTY 0
#define MAC_AX_MCC_STATE_H2C_SENT 1
#define MAC_AX_MCC_STATE_H2C_RCVD 2
#define MAC_AX_MCC_ADD_DONE 3
#define MAC_AX_MCC_START_DONE 4
#define MAC_AX_MCC_STOP_DONE 5
#define MAC_AX_MCC_STATE_ERROR 6
	u8 mcc_group[4];
	u8 mcc_group_state[4];
#define MAC_AX_MCC_REQ_IDLE 0
#define MAC_AX_MCC_REQ_H2C_SENT 1
#define MAC_AX_MCC_REQ_H2C_RCVD 2
#define MAC_AX_MCC_REQ_DONE 3
#define MAC_AX_MCC_REQ_FAIL 4
	u8 mcc_request[4];
	u8 mcc_request_state[4];
#define MAC_AX_FW_RESET_IDLE 0
#define MAC_AX_FW_RESET_RECV 1
#define MAC_AX_FW_RESET_RECV_DONE 2
#define MAC_AX_FW_RESET_PROCESS 3
	u8 fw_rst;
#define MAC_AX_AOAC_RPT_IDLE 0
#define MAC_AX_AOAC_RPT_H2C_SENDING 1
#define MAC_AX_AOAC_RPT_H2C_RCVD 2
#define MAC_AX_AOAC_RPT_H2C_DONE 3
#define MAC_AX_AOAC_RPT_ERROR 4
	u8 aoac_rpt;
#define MAC_AX_P2P_ACT_IDLE 0
#define MAC_AX_P2P_ACT_BUSY 1
#define MAC_AX_P2P_ACT_FAIL 2
	u8 p2p_stat;
#define MAC_AX_FUNC_OFF 0
#define MAC_AX_FUNC_ON 1
	u8 dmac_func;
	u8 cmac0_func;
	u8 cmac1_func;
	u8 bb0_func;
	u8 bb1_func;
#define MAC_AX_WOW_STOPTRX_IDLE 0
#define MAC_AX_WOW_STOPTRX_BUSY 1
#define MAC_AX_WOW_STOPTRX_FAIL 2
	u8 wow_stoptrx_stat;
#define MAC_AX_MAC_NOT_RDY 0
#define MAC_AX_MAC_RDY 1
#define MAC_AX_MAC_INIT_ERR 2
#define MAC_AX_MAC_DEINIT_ERR 3
#define MAC_AX_MAC_FINIT_ERR 4
#define MAC_AX_MAC_FDEINIT_ERR 5
	u8 mac_rdy;
#define MAC_AX_ROLE_ALOC_SUCC 0
#define MAC_AX_ROLE_INIT_SUCC 1
#define MAC_AX_ROLE_HW_UPD_SUCC 2
#define MAC_AX_ROLE_ALOC_FAIL 3
#define MAC_AX_ROLE_INIT_FAIL 4
#define MAC_AX_ROLE_HW_UPD_FAIL 5
	u8 role_stat;
#define MAC_AX_PLAT_OFF 0
#define MAC_AX_PLAT_ON 1
	u8 plat;
#define MAC_AX_IO_ST_NORM 0
#define MAC_AX_IO_ST_HANG 1
	u8 io_st;
#define MAC_AX_L2_DIS 0
#define MAC_AX_L2_EN 1
#define MAC_AX_L2_TRIG 2
	u8 l2_st;
#define MAC_AX_SER_CTRL_SRT 0
#define MAC_AX_SER_CTRL_STOP 1
#define MAC_AX_SER_CTRL_ERR 2
	u8 ser_ctrl_st;
#define MAC_AX_CH_SWITCH_GET_RPT 4
	u8 ch_switch;
#define MAC_AX_PROXY_IDLE 0
#define MAC_AX_PROXY_SENDING 1
#define MAC_AX_PROXY_BUSY 2
	u8 proxy_st;
	u8 proxy_ret;
	u8 sensing_csi_st;
#define MAC_AX_SENSING_CSI_IDLE 0
#define MAC_AX_SENSING_CSI_SENDING 1
#define MAC_AX_NAN_IDLE 0
#define MAX_AX_NAN_ACT_H2C_FAIL 1
#define MAX_AX_NAN_ACT_H2C_DONE 2
	u8 nan_stat;
};

#define MAC_AX_DFLT_SM \
	{MAC_AX_PWR_OFF, MAC_AX_FWDL_IDLE, MAC_AX_EFUSE_IDLE, \
	MAC_AX_OFLD_REQ_IDLE, MAC_AX_OFLD_REQ_IDLE, MAC_AX_OFLD_REQ_IDLE, \
	MAC_AX_CMD_OFLD_IDLE, MAC_AX_OFLD_H2C_IDLE, MAC_AX_OFLD_H2C_IDLE, \
	MAC_AX_OFLD_H2C_IDLE, MAC_AX_OFLD_H2C_IDLE, MAC_AX_OFLD_H2C_IDLE, \
	MAC_AX_OFLD_H2C_IDLE, MAC_AX_OFLD_H2C_IDLE, MAC_AX_OFLD_H2C_IDLE, \
	MAC_AX_OFLD_H2C_IDLE, \
	{MAC_AX_MCC_EMPTY}, {MAC_AX_MCC_EMPTY}, {MAC_AX_MCC_REQ_IDLE}, \
	{MAC_AX_MCC_REQ_IDLE}, MAC_AX_FW_RESET_IDLE, MAC_AX_AOAC_RPT_IDLE, \
	MAC_AX_P2P_ACT_IDLE, MAC_AX_FUNC_OFF, MAC_AX_FUNC_OFF, \
	MAC_AX_FUNC_OFF, MAC_AX_FUNC_OFF, MAC_AX_FUNC_OFF, \
	MAC_AX_WOW_STOPTRX_IDLE, MAC_AX_MAC_NOT_RDY, MAC_AX_ROLE_ALOC_SUCC, \
	MAC_AX_PLAT_OFF, MAC_AX_IO_ST_NORM, MAC_AX_L2_EN, MAC_AX_SER_CTRL_SRT,\
	MAC_AX_OFLD_H2C_IDLE, MAC_AX_PROXY_IDLE, MAC_AX_PROXY_IDLE, MAC_AX_NAN_IDLE}
#endif
