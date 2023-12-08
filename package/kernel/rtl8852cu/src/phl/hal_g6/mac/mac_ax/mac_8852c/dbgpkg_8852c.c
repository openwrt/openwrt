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
#include "../dbgpkg.h"
#include "dbgpkg_8852c.h"

#if MAC_AX_8852C_SUPPORT
/* For dbg port */
static struct mac_ax_dbg_port_info dbg_port_ptcl_c0 = {
	R_AX_PTCL_DBG,
	MAC_AX_BYTE_SEL_1,
	B_AX_PTCL_DBG_SEL_SH,
	B_AX_PTCL_DBG_SEL_MSK,
	0x00,
	0x3F,
	1,
	R_AX_PTCL_DBG_INFO,
	MAC_AX_BYTE_SEL_4,
	B_AX_PTCL_DBG_INFO_SH,
	B_AX_PTCL_DBG_INFO_MSK
};

static struct mac_ax_dbg_port_info dbg_port_ptcl_c1 = {
	R_AX_PTCL_DBG_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_PTCL_DBG_SEL_SH,
	B_AX_PTCL_DBG_SEL_MSK,
	0x00,
	0x3F,
	1,
	R_AX_PTCL_DBG_INFO_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_PTCL_DBG_INFO_SH,
	B_AX_PTCL_DBG_INFO_MSK
};

static struct mac_ax_dbg_port_info dbg_port_sch_c0 = {
	R_AX_SCH_DBG_SEL,
	MAC_AX_BYTE_SEL_1,
	B_AX_SCH_DBG_SEL_SH,
	B_AX_SCH_DBG_SEL_MSK,
	0x00,
	0x2F,
	1,
	R_AX_SCH_DBG,
	MAC_AX_BYTE_SEL_4,
	B_AX_SCHEDULER_DBG_SH,
	B_AX_SCHEDULER_DBG_MSK
};

static struct mac_ax_dbg_port_info dbg_port_sch_c1 = {
	R_AX_SCH_DBG_SEL_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_SCH_DBG_SEL_SH,
	B_AX_SCH_DBG_SEL_MSK,
	0x00,
	0x2F,
	1,
	R_AX_SCH_DBG_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_SCHEDULER_DBG_SH,
	B_AX_SCHEDULER_DBG_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tmac_v1_c0 = {
	R_AX_MACTX_DBG_SEL_CNT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DBGSEL_MACTX_V1_SH,
	B_AX_DBGSEL_MACTX_V1_MSK,
	0x00,
	0x41,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tmac_v1_c1 = {
	R_AX_MACTX_DBG_SEL_CNT_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_DBGSEL_MACTX_V1_SH,
	B_AX_DBGSEL_MACTX_V1_MSK,
	0x00,
	0x41,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_rmac_c0 = {
	R_AX_RX_DEBUG_SELECT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DEBUG_SEL_SH,
	B_AX_DEBUG_SEL_MSK,
	0x00,
	0x58,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_rmac_c1 = {
	R_AX_RX_DEBUG_SELECT_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_DEBUG_SEL_SH,
	B_AX_DEBUG_SEL_MSK,
	0x00,
	0x58,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_rmacst_c0 = {
	R_AX_RX_STATE_MONITOR,
	MAC_AX_BYTE_SEL_1,
	B_AX_STATE_SEL_SH,
	B_AX_STATE_SEL_MSK,
	0x00,
	0x17,
	1,
	R_AX_RX_STATE_MONITOR,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffffL
};

static struct mac_ax_dbg_port_info dbg_port_rmacst_c1 = {
	R_AX_RX_STATE_MONITOR_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_STATE_SEL_SH,
	B_AX_STATE_SEL_MSK,
	0x00,
	0x17,
	1,
	R_AX_RX_STATE_MONITOR_C1,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffffL
};

static struct mac_ax_dbg_port_info dbg_port_rmac_plcp_c0 = {
	R_AX_RX_PLCP_MON,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_PLCP_MON_SEL_SH,
	B_AX_RX_PLCP_MON_SEL_MSK,
	0x0,
	0xF,
	1,
	R_AX_RX_PLCP_MON,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffffL
};

static struct mac_ax_dbg_port_info dbg_port_rmac_plcp_c1 = {
	R_AX_RX_PLCP_MON_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_PLCP_MON_SEL_SH,
	B_AX_RX_PLCP_MON_SEL_MSK,
	0x0,
	0xF,
	1,
	R_AX_RX_PLCP_MON_C1,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffffL
};

static struct mac_ax_dbg_port_info dbg_port_trxptcl_v1_c0 = {
	R_AX_DBGSEL_TRXPTCL,
	MAC_AX_BYTE_SEL_1,
	B_AX_DBGSEL_TRXPTCL_SH,
	B_AX_DBGSEL_TRXPTCL_MSK,
	0x08,
	0x12,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_trxptcl_v1_c1 = {
	R_AX_DBGSEL_TRXPTCL_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_DBGSEL_TRXPTCL_SH,
	B_AX_DBGSEL_TRXPTCL_MSK,
	0x08,
	0x12,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tx_infol_c0 = {
	R_AX_WMAC_TX_INFO_DEBUG_SEL,
	MAC_AX_BYTE_SEL_1,
	B_AX_TX_INFO_DEBUG_SEL_SH,
	B_AX_TX_INFO_DEBUG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_WMAC_TX_INFO0_DEBUG,
	MAC_AX_BYTE_SEL_4,
	B_AX_TX_INFO_L4B_SH,
	B_AX_TX_INFO_L4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tx_infoh_c0 = {
	R_AX_WMAC_TX_INFO_DEBUG_SEL,
	MAC_AX_BYTE_SEL_1,
	B_AX_TX_INFO_DEBUG_SEL_SH,
	B_AX_TX_INFO_DEBUG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_WMAC_TX_INFO1_DEBUG,
	MAC_AX_BYTE_SEL_4,
	B_AX_TX_INFO_H4B_SH,
	B_AX_TX_INFO_H4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tx_infol_c1 = {
	R_AX_WMAC_TX_INFO_DEBUG_SEL_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_TX_INFO_DEBUG_SEL_SH,
	B_AX_TX_INFO_DEBUG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_WMAC_TX_INFO0_DEBUG_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_TX_INFO_L4B_SH,
	B_AX_TX_INFO_L4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_tx_infoh_c1 = {
	R_AX_WMAC_TX_INFO_DEBUG_SEL_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_TX_INFO_DEBUG_SEL_SH,
	B_AX_TX_INFO_DEBUG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_WMAC_TX_INFO1_DEBUG_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_TX_INFO_H4B_SH,
	B_AX_TX_INFO_H4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_txtf_infol_c0 = {
	R_AX_RX_TB_CTRL_INFO_CFG,
	MAC_AX_BYTE_SEL_1,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_SH,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_MSK,
	0x00,
	0x04,
	1,
	R_AX_RX_TB_CTRL_INFO_0,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_TB_CTRL_L4B_SH,
	B_AX_RX_TB_CTRL_L4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_txtf_infoh_c0 = {
	R_AX_RX_TB_CTRL_INFO_CFG,
	MAC_AX_BYTE_SEL_1,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_SH,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_MSK,
	0x00,
	0x04,
	1,
	R_AX_RX_TB_CTRL_INFO_1,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_TB_CTRL_H4B_SH,
	B_AX_RX_TB_CTRL_H4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_txtf_infol_c1 = {
	R_AX_RX_TB_CTRL_INFO_CFG_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_SH,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_MSK,
	0x00,
	0x04,
	1,
	R_AX_RX_TB_CTRL_INFO_0_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_TB_CTRL_L4B_SH,
	B_AX_RX_TB_CTRL_L4B_MSK
};

static struct mac_ax_dbg_port_info dbg_port_txtf_infoh_c1 = {
	R_AX_RX_TB_CTRL_INFO_CFG_C1,
	MAC_AX_BYTE_SEL_1,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_SH,
	B_AX_WMAC_RX_TB_CTRL_DBG_SEL_MSK,
	0x00,
	0x04,
	1,
	R_AX_RX_TB_CTRL_INFO_1_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_RX_TB_CTRL_H4B_SH,
	B_AX_RX_TB_CTRL_H4B_MSK
};

#if 0
static struct mac_ax_dbg_port_info dbg_port_cmac_dma0_c0 = {
	R_AX_RXDMA_CTRL_0,
	MAC_AX_BYTE_SEL_4,
	B_AX_RXDMA_FIFO_DBG_SEL_SH,
	0x3F,
	0x00,
	0x3F,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_cmac_dma1_c0 = {
	R_AX_TXDMA_DBG,
	MAC_AX_BYTE_SEL_4,
	B_AX_TXDMA_DBG_SEL_SH,
	B_AX_TXDMA_DBG_SEL_MSK,
	0x00,
	0x03,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_cmac_dma0_c1 = {
	R_AX_RXDMA_CTRL_0_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_RXDMA_FIFO_DBG_SEL_SH,
	0x3F,
	0x00,
	0x3F,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_cmac_dma1_c1 = {
	R_AX_TXDMA_DBG_C1,
	MAC_AX_BYTE_SEL_4,
	B_AX_TXDMA_DBG_SEL_SH,
	B_AX_TXDMA_DBG_SEL_MSK,
	0x00,
	0x03,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};
#endif

static struct mac_ax_dbg_port_info dbg_port_wde_bufmgn_ctl = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0,
	0x101,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_bufmgn_arb = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xE0E,
	0xE0E,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_quemgn_ctl = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1010,
	0x1111,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_quemgn_info = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1414,
	0x1414,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_quemgn_arb = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1E1E,
	0x1E1E,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif0 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x8080,
	0x8282,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif1 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x9090,
	0x9292,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif3 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xB0B0,
	0xB2B2,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif4 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xC0C0,
	0xC2C2,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif6 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xE0E0,
	0xE2E2,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wde_portif7 = {
	R_AX_WDE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xF0F0,
	0xF2F2,
	0x101,
	R_AX_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_bufmgn_ctl = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0,
	0x101,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_bufmgn_arb = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xE0E,
	0xE0E,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_quemgn_ctl = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1010,
	0x1111,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_quemgn_info = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1414,
	0x1414,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_quemgn_arb = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x1E1E,
	0x1E1E,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif0 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x8080,
	0x8282,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif1 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0x9090,
	0x9292,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif2_0 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xA0A0,
	0xA2A2,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif2_1 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xA8A8,
	0xAAAA,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif3 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xB0B0,
	0xB2B2,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif4 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xC0C0,
	0xC2C2,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif5 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xD0D0,
	0xD2D2,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_ple_portif6 = {
	R_AX_PLE_DBG_CTL,
	MAC_AX_BYTE_SEL_2,
	0,
	0xFFFF,
	0xE0E0,
	0xE2E2,
	0x101,
	R_AX_PLE_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xFFFFFFFF
};

static struct mac_ax_dbg_port_info dbg_port_wdrls_ctl = {
	R_AX_DBG_CTL_WDRLS,
	MAC_AX_BYTE_SEL_2,
	0,
	0xffff,
	0,
	0,
	0x101,
	R_AX_DBG_OUT_WDRLS,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_wdrls_rptgen0 = {
	R_AX_DBG_CTL_WDRLS,
	MAC_AX_BYTE_SEL_2,
	0,
	0xffff,
	0x808,
	0x909,
	0x101,
	R_AX_DBG_OUT_WDRLS,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_wdrls_rptgen1 = {
	R_AX_DBG_CTL_WDRLS,
	MAC_AX_BYTE_SEL_2,
	0,
	0xffff,
	0xC0C,
	0xD0D,
	0x101,
	R_AX_DBG_OUT_WDRLS,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_wdrls_pledchn0 = {
	R_AX_DBG_CTL_WDRLS,
	MAC_AX_BYTE_SEL_2,
	0,
	0xffff,
	0x1010,
	0x1818,
	0x404,
	R_AX_DBG_OUT_WDRLS,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_wdrls_pledchn1 = {
	R_AX_DBG_CTL_WDRLS,
	MAC_AX_BYTE_SEL_2,
	0,
	0xffff,
	0x1919,
	0x1B1B,
	0x101,
	R_AX_DBG_OUT_WDRLS,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_fetpkt = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0,
	0x10001,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_cmdpsr = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x800080,
	0x810081,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_cmacdmaif = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x880088,
	0x880088,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_preld0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x900090,
	0x940094,
	0x20002,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_preld1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x950095,
	0x950095,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit0_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1000100,
	0x1030103,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit0_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1080108,
	0x10E010E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit1_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1100110,
	0x1130113,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit1_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1180118,
	0x11E011E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit2_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1200120,
	0x1230123,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit2_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1280128,
	0x12E012E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit3_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1300130,
	0x1330133,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit3_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1380138,
	0x13E013E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit4_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1400140,
	0x1430143,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit4_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1480148,
	0x14E014E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit5_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1500150,
	0x1530153,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit5_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1580158,
	0x15E015E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit6_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1600160,
	0x1630163,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit6_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1680168,
	0x16E016E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit7_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1700170,
	0x1730173,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b0_unit7_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x1780178,
	0x17E017E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b1_cmdpsr = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x2800280,
	0x2810281,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b1_cmacdmaif = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x2880288,
	0x2880288,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b1_unit0_0 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x3000300,
	0x3030303,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_txpktctrl_b1_unit0_1 = {
	R_AX_DBG_CTL_TXPKT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
	0x3080308,
	0x30E030E,
	0x10001,
	R_AX_TPC_DBG_OUT,
	MAC_AX_BYTE_SEL_4,
	0,
	0xffffffff,
};

static struct mac_ax_dbg_port_info dbg_port_pktinfo = {
	R_AX_DBG_FUN_INTF_CTL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK,
	0x80000000,
	0x8000017f,
	1,
	R_AX_DBG_FUN_INTF_DATA,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK
};

static struct mac_ax_dbg_port_info dbg_port_mpduinfo_b0 = {
	R_AX_DBG_FUN_INTF_CTL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK,
	0x80010000,
	0x80010003,
	1,
	R_AX_DBG_FUN_INTF_DATA,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK
};

static struct mac_ax_dbg_port_info dbg_port_mpduinfo_b1 = {
	R_AX_DBG_FUN_INTF_CTL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK,
	0x80010100,
	0x80010103,
	1,
	R_AX_DBG_FUN_INTF_DATA,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK
};

static struct mac_ax_dbg_port_info dbg_port_preld_b0 = {
	R_AX_DBG_FUN_INTF_CTL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK,
	0x80020000,
	0x80020009,
	1,
	R_AX_DBG_FUN_INTF_DATA,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK
};

static struct mac_ax_dbg_port_info dbg_port_preld_b1 = {
	R_AX_DBG_FUN_INTF_CTL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK,
	0x80020100,
	0x80020103,
	1,
	R_AX_DBG_FUN_INTF_DATA,
	MAC_AX_BYTE_SEL_4,
	B_AX_DFI_DATA_SH,
	B_AX_DFI_DATA_MSK
};

#if 0
static struct mac_ax_dbg_port_info dbg_port_pcie_txdma = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x03,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_rxdma = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x04,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_cvt = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x01,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac04 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x05,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac5 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac6 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac7 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_pnp_io = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x05,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac814 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x06,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac15 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac16 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac17 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_pcie_emac18 = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_2,
	B_AX_DBG_SEL_SH,
	B_AX_DBG_SEL_MSK,
	0x00,
	0x00,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};
#endif

static struct mac_ax_dbg_port_info dbg_port_usb2_phy = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x00,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_sie = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x04,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_utmi = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x01,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_sie_mmu = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x03,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_sie_pce = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x03,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_utmi_if = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x00,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_wltx = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x05,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_wlrx = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x0D,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb3 = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x0E,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_setup = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x01,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_wltx_dma = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x09,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_wlrx_dma = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x00,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_ainst = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x03,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_misc = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x00,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb_bttx = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x02,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_usb2_bt = {
	R_AX_USB_DBGSEL_0_V1,
	MAC_AX_BYTE_SEL_1,
	B_AX_USB_DBGSEL_0_V1_SH,
	B_AX_USB_DBGSEL_0_V1_MSK,
	0x00,
	0x00,
	1,
	R_AX_USB_DBGSEL_1_V1,
	MAC_AX_BYTE_SEL_4,
	B_AX_USB_DBGSEL_1_V1_SH,
	B_AX_USB_DBGSEL_1_V1_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_tx0_5 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0xF,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_tx6 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x6,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_tx7 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x9,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_tx8 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x3,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_tx9_C = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x1,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_txD_F = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx0 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0xB,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx1 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x4,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx3 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x8,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx4 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x7,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx5_8 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x1,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_tx9 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x3,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_txA_C = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_rx0 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x8,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_rx1_2 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_rx3 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x6,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_rx4 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_hdt_rx5 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_2,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_rx_p0_0 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x3,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_rx_p0_1 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x6,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_rx_p0_2 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_DBG_SEL_SH,
	B_AX_DISPATCHER_DBG_SEL_MSK,
	0x0,
	0x0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_cdt_rx_p1 = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x8,
	0xE,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_stf_ctrl = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x0,
	0x5,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_addr_ctrl = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x0,
	0x7,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_wde_intf = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x0,
	0xF,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_ple_intf = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x0,
	0x9,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_dspt_flow_ctrl = {
	R_AX_DISPATCHER_DBG_PORT,
	MAC_AX_BYTE_SEL_1,
	B_AX_DISPATCHER_CH_SEL_SH,
	B_AX_DISPATCHER_CH_SEL_MSK,
	0x0,
	0x3,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_paxi_txdma = {
	R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_PL_AXIDMA_DBG_SEL_SH,
	B_PL_AXIDMA_DBG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_paxi_rxdma = {
	R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_PL_AXIDMA_DBG_SEL_SH,
	B_PL_AXIDMA_DBG_SEL_MSK,
	0x40,
	0x64,
	9,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_paxi_mst = {
	R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_PL_AXIDMA_DBG_SEL_SH,
	B_PL_AXIDMA_DBG_SEL_MSK,
	0x80,
	0x80,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_paxi_int = {
	R_AX_INDIR_ACCESS_ENTRY + R_PL_AXIDMA_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_PL_AXIDMA_DBG_SEL_SH,
	B_PL_AXIDMA_DBG_SEL_MSK,
	0xC0,
	0xC0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_haxi_txdma = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_AX_HAXI_DBG_SEL_SH,
	B_AX_HAXI_DBG_SEL_MSK,
	0x00,
	0x07,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_haxi_rxdma = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_AX_HAXI_DBG_SEL_SH,
	B_AX_HAXI_DBG_SEL_MSK,
	0x40,
	0x64,
	9,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_haxi_mst = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_AX_HAXI_DBG_SEL_SH,
	B_AX_HAXI_DBG_SEL_MSK,
	0x80,
	0x80,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_haxi_int = {
	R_AX_HAXI_DBG_CTRL,
	MAC_AX_BYTE_SEL_4,
	B_AX_HAXI_DBG_SEL_SH,
	B_AX_HAXI_DBG_SEL_MSK,
	0xC0,
	0xC0,
	1,
	R_AX_DBG_PORT_SEL,
	MAC_AX_BYTE_SEL_4,
	B_AX_DEBUG_ST_SH,
	B_AX_DEBUG_ST_MSK
};

static struct mac_ax_dbg_port_info dbg_port_sta_sch = {
	R_AX_SS_DBG_3,
	MAC_AX_BYTE_SEL_2,
	B_AX_SS_TOP_DBG_SEL_V1_SH,
	B_AX_SS_TOP_DBG_SEL_V1_MSK,
	0x00,
	0x11,
	1,
	R_AX_SS_DBG_INFO,
	MAC_AX_BYTE_SEL_4,
	B_AX_SS_DBG_INFO_SH,
	B_AX_SS_DBG_INFO_MSK
};

u32 tx_dbg_dump_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;
	u32 i;

	PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_0=0x%x\n",
			 MAC_REG_R32(R_AX_CMAC_MACID_DROP_0));

	PLTFM_MSG_ALWAYS("R_AX_MBSSID_DROP_0=0x%x\n",
			 MAC_REG_R32(R_AX_MBSSID_DROP_0));

	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_0=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_0));

	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_1=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_1));

	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_2=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_2));

	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_3=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_3));

	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_0=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_0));

	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_1=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_1));

	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_2=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_2));

	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_3=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_3));

	PLTFM_MSG_ALWAYS("R_AX_CTN_TXEN=0x%x\n",
			 MAC_REG_R32(R_AX_CTN_TXEN));

	PLTFM_MSG_ALWAYS("R_AX_PTCL_COMMON_SETTING_0=0x%x\n",
			 MAC_REG_R32(R_AX_PTCL_COMMON_SETTING_0));

	PLTFM_MSG_ALWAYS("R_AX_MAC_LOOPBACK=0x%x\n",
			 MAC_REG_R32(R_AX_MAC_LOOPBACK));

	ret = dbg_port_dump(adapter, MAC_AX_DBG_PORT_SEL_PTCL_C0);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("PTCL_C0 dbg port dump %d\n", ret);

	ret = dbg_port_dump(adapter, MAC_AX_DBG_PORT_SEL_SCH_C0);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("SCH_C0 dbg port dump %d\n", ret);

	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY0=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY0));

	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY1=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY1));

	for (i = 0; i < TRX_CNT_REPT_CNT; i++) {
#if MAC_AX_PCIE_SUPPORT
		if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
			PLTFM_MSG_ALWAYS("R_AX_PCIE_MIO_INTF=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_MIO_INTF));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_MIO_INTD=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_MIO_INTD));

			PLTFM_MSG_ALWAYS("R_AX_HAXI_INIT_CFG1=0x%x\n",
					 MAC_REG_R32(R_AX_HAXI_INIT_CFG1));

			PLTFM_MSG_ALWAYS("R_AX_HAXI_DMA_STOP1=0x%x\n",
					 MAC_REG_R32(R_AX_HAXI_DMA_STOP1));

			PLTFM_MSG_ALWAYS("R_AX_HAXI_DMA_BUSY1=0x%x\n",
					 MAC_REG_R32(R_AX_HAXI_DMA_BUSY1));

			PLTFM_MSG_ALWAYS("R_AX_HAXI_DBG_CTRL=0x%x\n",
					 MAC_REG_R32(R_AX_HAXI_DBG_CTRL));

			PLTFM_MSG_ALWAYS("R_AX_DBG_ERR_FLAG_V1=0x%x\n",
					 MAC_REG_R32(R_AX_DBG_ERR_FLAG_V1));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_M1=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_IO_RCY_M1));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_M1=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_PADDR_M1));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_M2=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_IO_RCY_M2));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_M2=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_PADDR_M2));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_E0=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_IO_RCY_E0));

			PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_E0=0x%x\n",
					 MAC_REG_R32(R_AX_PCIE_PADDR_E0));

			PLTFM_MSG_ALWAYS("R_AX_ACH0_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH0_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH1_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH1_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH2_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH2_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH3_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH3_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH4_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH4_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH5_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH5_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH6_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH6_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_ACH7_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_ACH7_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_CH8_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_CH8_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_CH9_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_CH9_TXBD_IDX));

			PLTFM_MSG_ALWAYS("R_AX_CH10_TXBD_IDX_V1=0x%x\n",
					 MAC_REG_R32(R_AX_CH10_TXBD_IDX_V1));

			PLTFM_MSG_ALWAYS("R_AX_CH11_TXBD_IDX_V1=0x%x\n",
					 MAC_REG_R32(R_AX_CH11_TXBD_IDX_V1));

			PLTFM_MSG_ALWAYS("R_AX_CH12_TXBD_IDX=0x%x\n",
					 MAC_REG_R32(R_AX_CH12_TXBD_IDX));
		}
#endif
		/* TMAC TX COUNTER */
		ret = tx_cnt_dump(adapter, MAC_AX_BAND_0, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ALWAYS("[ERR]tx cnt dump err %d\n", ret);

		pltfm_dbg_dump(adapter);

		PLTFM_DELAY_US(TRX_CNT_REPT_DLY_US);
	}

	return MACSUCCESS;
}

u32 crit_dbg_dump_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ALWAYS("R_AX_SYS_ISO_CTRL=0x%x\n",
			 MAC_REG_R16(R_AX_SYS_ISO_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_SYS_FUNC_EN=0x%x\n",
			 MAC_REG_R16(R_AX_SYS_FUNC_EN));

	PLTFM_MSG_ALWAYS("R_AX_SYS_PW_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_SYS_PW_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_SYS_CLK_CTRL=0x%x\n",
			 MAC_REG_R16(R_AX_SYS_CLK_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_SYS_EEPROM_CTRL=0x%x\n",
			 MAC_REG_R16(R_AX_SYS_WL_EFUSE_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_DBG_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_DBG_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_PLATFORM_ENABLE=0x%x\n",
			 MAC_REG_R32(R_AX_PLATFORM_ENABLE));

	PLTFM_MSG_ALWAYS("R_AX_WLLPS_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_WLLPS_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_WLRESUME_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_WLRESUME_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_DBG_PORT_SEL=0x%x\n",
			 MAC_REG_R32(R_AX_DBG_PORT_SEL));

	PLTFM_MSG_ALWAYS("R_AX_HALT_H2C_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_HALT_H2C_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_HALT_H2C=0x%x\n",
			 MAC_REG_R32(R_AX_HALT_H2C));

	PLTFM_MSG_ALWAYS("R_AX_HALT_C2H_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_HALT_C2H_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_HALT_C2H=0x%x\n",
			 MAC_REG_R32(R_AX_HALT_C2H));

	PLTFM_MSG_ALWAYS("R_AX_SYS_CFG5=0x%x\n",
			 MAC_REG_R32(R_AX_SYS_CFG5));

	PLTFM_MSG_ALWAYS("R_AX_WCPU_FW_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_WCPU_FW_CTRL));

	PLTFM_MSG_ALWAYS("R_AX_RPWM=0x%x\n",
			 MAC_REG_R16(R_AX_RPWM));

	PLTFM_MSG_ALWAYS("R_AX_BOOT_REASON=0x%x\n",
			 MAC_REG_R16(R_AX_BOOT_REASON));

	PLTFM_MSG_ALWAYS("R_AX_LDM=0x%x\n",
			 MAC_REG_R32(R_AX_LDM));

	PLTFM_MSG_ALWAYS("R_AX_UDM0=0x%x\n",
			 MAC_REG_R32(R_AX_UDM0));

	PLTFM_MSG_ALWAYS("R_AX_UDM1=0x%x\n",
			 MAC_REG_R32(R_AX_UDM1));

	PLTFM_MSG_ALWAYS("R_AX_UDM2=0x%x\n",
			 MAC_REG_R32(R_AX_UDM2));

	PLTFM_MSG_ALWAYS("R_AX_UDM3=0x%x\n",
			 MAC_REG_R32(R_AX_UDM3));

	PLTFM_MSG_ALWAYS("R_AX_IC_PWR_STATE=0x%x\n",
			 MAC_REG_R32(R_AX_IC_PWR_STATE));

	PLTFM_MSG_ALWAYS("R_AX_FW_DEBUG_USE_ONLY_0=0x%x\n",
			 MAC_REG_R32(R_AX_FW_DEBUG_USE_ONLY_0));

	PLTFM_MSG_ALWAYS("R_AX_FW_DEBUG_USE_ONLY_1=0x%x\n",
			 MAC_REG_R32(R_AX_FW_DEBUG_USE_ONLY_1));

	PLTFM_MSG_ALWAYS("R_AX_FW_DEBUG_USE_ONLY_2=0x%x\n",
			 MAC_REG_R32(R_AX_FW_DEBUG_USE_ONLY_2));

	PLTFM_MSG_ALWAYS("R_AX_FW_DEBUG_USE_ONLY_3=0x%x\n",
			 MAC_REG_R32(R_AX_FW_DEBUG_USE_ONLY_3));

	if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
		PLTFM_MSG_ALWAYS("R_AX_PCIE_MIO_INTF=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_MIO_INTF));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_MIO_INTD=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_MIO_INTD));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_M1=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_IO_RCY_M1));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_M1=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_PADDR_M1));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_M2=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_IO_RCY_M2));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_M2=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_PADDR_M2));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_IO_RCY_E0=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_IO_RCY_E0));

		PLTFM_MSG_ALWAYS("R_AX_PCIE_PADDR_E0=0x%x\n",
				 MAC_REG_R32(R_AX_PCIE_PADDR_E0));
	}

	return MACSUCCESS;
}

u32 cmac_dbg_dump_8852c(struct mac_ax_adapter *adapter, enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg;

	if (band == MAC_AX_BAND_1 && is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
		return MACSUCCESS;

	adapter->sm.l2_st = MAC_AX_L2_DIS;
	/* Func/Clk band 0 */
	if (band == MAC_AX_BAND_0) {
		reg = R_AX_CMAC_FUNC_EN;
		PLTFM_MSG_ALWAYS("B%d R_AX_CMAC_FUNC_EN=0x%x\n", band,
				 MAC_REG_R32(reg));

		reg = R_AX_CK_EN;
		PLTFM_MSG_ALWAYS("B%d R_AX_CK_EN=0x%x\n", band,
				 MAC_REG_R32(reg));
	}

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] check cmac en %d\n", ret);
		return ret;
	}

	/* Func/Clk band 1 */
	if (band == MAC_AX_BAND_1) {
		reg = R_AX_CMAC_FUNC_EN_C1;
		PLTFM_MSG_ALWAYS("B%d R_AX_CMAC_FUNC_EN=0x%x\n", band,
				 MAC_REG_R32(reg));

		reg = R_AX_CK_EN_C1;
		PLTFM_MSG_ALWAYS("B%d R_AX_CK_EN=0x%x\n", band,
				 MAC_REG_R32(reg));
	}

	/* Error IMR/ISR & Flag */
	reg = band == MAC_AX_BAND_1 ? R_AX_CMAC_ERR_IMR_C1 : R_AX_CMAC_ERR_IMR;
	PLTFM_MSG_ALWAYS("B%d R_AX_CMAC_ERR_IMR=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_CMAC_ERR_ISR_C1 : R_AX_CMAC_ERR_ISR;
	PLTFM_MSG_ALWAYS("B%d R_AX_CMAC_ERR_ISR=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_SCHEDULE_ERR_IMR_C1 : R_AX_SCHEDULE_ERR_IMR;
	PLTFM_MSG_ALWAYS("[0]B%d R_AX_SCHEDULE_ERR_IMR=0x%x\n", band,
			 MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ?
			 R_AX_SCHEDULE_ERR_ISR_C1 : R_AX_SCHEDULE_ERR_ISR;
	PLTFM_MSG_ALWAYS("[0]B%d R_AX_SCHEDULE_ERR_ISR=0x%x\n", band,
			 MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_PTCL_IMR0_C1 : R_AX_PTCL_IMR0;
	PLTFM_MSG_ALWAYS("[1]B%d R_AX_PTCL_IMR0=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_PTCL_ISR0_C1 : R_AX_PTCL_ISR0;
	PLTFM_MSG_ALWAYS("[1]B%d R_AX_PTCL_ISR0=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_ERR_FLAG_IMR_C1 : R_AX_RX_ERR_FLAG_IMR;
	PLTFM_MSG_ALWAYS("[3]B%d R_AX_RX_ERR_FLAG=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_RX_ERR_FLAG_C1 : R_AX_RX_ERR_FLAG;
	PLTFM_MSG_ALWAYS("[3]B%d R_AX_RX_ERR_FLAG=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_ERR_FLAG_IMR_C1 : R_AX_TX_ERR_FLAG_IMR;
	PLTFM_MSG_ALWAYS("[3]B%d R_AX_TX_ERR_FLAG=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_TX_ERR_FLAG_C1 : R_AX_TX_ERR_FLAG;
	PLTFM_MSG_ALWAYS("[3]B%d R_AX_TX_ERR_FLAG=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_PHYINFO_ERR_IMR_V1_C1 : R_AX_PHYINFO_ERR_IMR_V1;
	PLTFM_MSG_ALWAYS("[4]B%d R_AX_PHYINFO_ERR_IMR_V1=0x%x\n", band,
			 MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ?
		R_AX_PHYINFO_ERR_ISR_C1 : R_AX_PHYINFO_ERR_ISR;
	PLTFM_MSG_ALWAYS("[4]B%d R_AX_PHYINFO_ERR_ISR=0x%x\n", band,
			 MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TXPWR_IMR_C1 : R_AX_TXPWR_IMR;
	PLTFM_MSG_ALWAYS("[5]B%d R_AX_TXPWR_IMR=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_TXPWR_ISR_C1 : R_AX_TXPWR_ISR;
	PLTFM_MSG_ALWAYS("[5]B%d R_AX_TXPWR_ISR=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_ERR_IMR_C1 : R_AX_RX_ERR_IMR;
	PLTFM_MSG_ALWAYS("[6]B%d R_AX_RX_ERR_IMR=0x%x\n", band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ? R_AX_RX_ERR_ISR_C1 : R_AX_RX_ERR_ISR;
	PLTFM_MSG_ALWAYS("[6]B%d R_AX_RX_ERR_ISR=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_TRXPTCL_ERROR_INDICA_MASK_C1 : R_AX_TRXPTCL_ERROR_INDICA_MASK;
	PLTFM_MSG_ALWAYS("[7]B%d R_AX_TRXPTCL_ERROR_INDICA_MASK=0x%x\n",
			 band, MAC_REG_R32(reg));
	reg = band == MAC_AX_BAND_1 ?
		R_AX_TRXPTCL_ERROR_INDICA_C1 : R_AX_TRXPTCL_ERROR_INDICA;
	PLTFM_MSG_ALWAYS("[7]B%d R_AX_TRXPTCL_ERROR_INDICA=0x%x\n",
			 band, MAC_REG_R32(reg));

	/* CMAC DMA */
	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_RU0RU1_C1 : R_AX_RX_INFO_RU0RU1;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_RU0RU1=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_RU2RU3_C1 : R_AX_RX_INFO_RU2RU3;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_RU2RU3=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_RU4RU5_C1 : R_AX_RX_INFO_RU4RU5;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_RU4RU5=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_RU6RU7_C1 : R_AX_RX_INFO_RU6RU7;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_RU6RU7=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_F2P_TXRPT_C1 : R_AX_RX_INFO_F2P_TXRPT;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_F2P_TXRPT=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_RXSTS_C1 : R_AX_RX_INFO_RXSTS;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_RXSTS=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_CSI_C1 : R_AX_RX_INFO_CSI;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_CSI=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_INFO_CSI_1_C1 : R_AX_RX_INFO_CSI_1;
	PLTFM_MSG_ALWAYS("B%d R_AX_RX_INFO_CSI_1=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_TX_FIFO_INFO_RU0RU1_C1 : R_AX_TX_FIFO_INFO_RU0RU1;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_FIFO_INFO_RU0RU1=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_TX_FIFO_INFO_RU2RU3_C1 : R_AX_TX_FIFO_INFO_RU2RU3;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_FIFO_INFO_RU2RU3=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_TX_FIFO_INFO_RU4RU5_C1 : R_AX_TX_FIFO_INFO_RU4RU5;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_FIFO_INFO_RU4RU5=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
		R_AX_TX_FIFO_INFO_RU6RU7_C1 : R_AX_TX_FIFO_INFO_RU6RU7;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_FIFO_INFO_RU6RU7=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TXDMA_CTRL_C1 : R_AX_TXDMA_CTRL;
	PLTFM_MSG_ALWAYS("B%d R_AX_TXDMA_CTRL=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU0_C1 : R_AX_TX_INFO_RU0;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU0=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU1_C1 : R_AX_TX_INFO_RU1;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU1=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU2_C1 : R_AX_TX_INFO_RU2;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU2=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU3_C1 : R_AX_TX_INFO_RU3;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU3=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU4_C1 : R_AX_TX_INFO_RU4;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU4=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU5_C1 : R_AX_TX_INFO_RU5;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU5=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU6_C1 : R_AX_TX_INFO_RU6;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU6=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_RU7_C1 : R_AX_TX_INFO_RU7;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_RU7=0x%x\n", band, MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_INFO_CSI_C1 : R_AX_TX_INFO_CSI;
	PLTFM_MSG_ALWAYS("B%d R_AX_TX_INFO_CSI=0x%x\n", band, MAC_REG_R32(reg));

	/* TMAC */
	reg = band == MAC_AX_BAND_1 ?
			 R_AX_MACTX_DBG_SEL_CNT_C1 : R_AX_MACTX_DBG_SEL_CNT;
	PLTFM_MSG_ALWAYS("B%d R_AX_MACTX_DBG_SEL_CNT=0x%x\n", band,
			 MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_WMAC_TX_INFO_DEBUG_SEL_C1 : R_AX_WMAC_TX_INFO_DEBUG_SEL;
	PLTFM_MSG_ALWAYS("B%d R_AX_WMAC_TX_INFO_DEBUG_SEL=0x%x\n", band,
			 MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_WMAC_TX_INFO0_DEBUG_C1 : R_AX_WMAC_TX_INFO0_DEBUG;
	PLTFM_MSG_ALWAYS("B%d R_AX_WMAC_TX_INFO0_DEBUG=0x%x\n", band,
			 MAC_REG_R32(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_WMAC_TX_INFO1_DEBUG_C1 : R_AX_WMAC_TX_INFO1_DEBUG;
	PLTFM_MSG_ALWAYS("B%d R_AX_WMAC_TX_INFO1_DEBUG=0x%x\n", band,
			 MAC_REG_R32(reg));

	/* TMAC TX COUNTER */
	ret = tx_cnt_dump(adapter, band, TRX_CNT_REPT_CNT);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ALWAYS("[ERR]tx cnt dump err %d\n", ret);

	/* TRX PTCL */
	reg = band == MAC_AX_BAND_1 ?
			 R_AX_MAC_LOOPBACK_COUNT_C1 : R_AX_MAC_LOOPBACK_COUNT;
	PLTFM_MSG_ALWAYS("B%d R_AX_MAC_LOOPBACK_COUNT=0x%x\n", band,
			 MAC_REG_R32(reg));

	/* RMAC */
	reg = band == MAC_AX_BAND_1 ? R_AX_RCR_C1 : R_AX_RCR;
	PLTFM_MSG_ALWAYS("B%d R_AX_RCR=0x%x\n", band, MAC_REG_R16(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_DLK_PROTECT_CTL_C1 : R_AX_DLK_PROTECT_CTL;
	PLTFM_MSG_ALWAYS("B%d R_AX_DLK_PROTECT_CTL=0x%x\n", band, MAC_REG_R16(reg));

	reg = band == MAC_AX_BAND_1 ? R_AX_PPDU_STAT_C1 : R_AX_PPDU_STAT;
	PLTFM_MSG_ALWAYS("B%d R_AX_PPDU_STAT=0x%x\n", band, MAC_REG_R16(reg));

	reg = band == MAC_AX_BAND_1 ?
			 R_AX_PPDU_STAT_ERR_C1 : R_AX_PPDU_STAT_ERR;
	PLTFM_MSG_ALWAYS("B%d R_AX_PPDU_STAT_ERR=0x%x\n", band, MAC_REG_R16(reg));

	/* RMAC RX COUNTER */
	ret = rx_cnt_dump(adapter, band, TRX_CNT_REPT_CNT);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ALWAYS("[ERR]rx cnt dump err %d\n", ret);

	adapter->sm.l2_st = MAC_AX_L2_EN;

	return MACSUCCESS;
}

u32 dmac_dbg_dump_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

	/* Func/Clk */
	PLTFM_MSG_ALWAYS("R_AX_DMAC_FUNC_EN=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_FUNC_EN));
	PLTFM_MSG_ALWAYS("R_AX_DMAC_CLK_EN=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_CLK_EN));

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ALWAYS("[ERR] check dmac en %d\n", ret);
		return ret;
	}
	/* Common */
	PLTFM_MSG_ALWAYS("R_AX_SER_DBG_INFO=0x%x\n",
			 MAC_REG_R32(R_AX_SER_DBG_INFO));
	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY0=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY0));
	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY1=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY1));
	/* Error IMR/ISR & Flag */
	PLTFM_MSG_ALWAYS("R_AX_DMAC_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_ERR_IMR));
	PLTFM_MSG_ALWAYS("R_AX_DMAC_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_ERR_ISR));
	PLTFM_MSG_ALWAYS("[0]R_AX_WDRLS_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_WDRLS_ERR_IMR));
	PLTFM_MSG_ALWAYS("[0]R_AX_WDRLS_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_WDRLS_ERR_ISR));
	PLTFM_MSG_ALWAYS("[1]R_AX_SEC_ERR_IMR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_SEC_DEBUG1));
	PLTFM_MSG_ALWAYS("[2.1]R_AX_MPDU_TX_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_TX_ERR_IMR));
	PLTFM_MSG_ALWAYS("[2.1]R_AX_MPDU_TX_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_TX_ERR_ISR));
	PLTFM_MSG_ALWAYS("[2.2]R_AX_MPDU_RX_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_RX_ERR_IMR));
	PLTFM_MSG_ALWAYS("[2.2]R_AX_MPDU_RX_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_RX_ERR_ISR));
	PLTFM_MSG_ALWAYS("[3]R_AX_STA_SCHEDULER_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_STA_SCHEDULER_ERR_IMR));
	PLTFM_MSG_ALWAYS("[3]R_AX_STA_SCHEDULER_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_STA_SCHEDULER_ERR_ISR));
	PLTFM_MSG_ALWAYS("[4]R_AX_WDE_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_WDE_ERR_IMR));
	PLTFM_MSG_ALWAYS("[4]R_AX_WDE_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_WDE_ERR_ISR));
	PLTFM_MSG_ALWAYS("[5.1]R_AX_TXPKTCTL_B0_ERRFLAG_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_TXPKTCTL_B0_ERRFLAG_IMR));
	PLTFM_MSG_ALWAYS("[5.1]R_AX_TXPKTCTL_B0_ERRFLAG_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_TXPKTCTL_B0_ERRFLAG_ISR));
	PLTFM_MSG_ALWAYS("[5.2]R_AX_TXPKTCTL_B1_ERRFLAG_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_TXPKTCTL_B1_ERRFLAG_IMR));
	PLTFM_MSG_ALWAYS("[5.2]R_AX_TXPKTCTL_B1_ERRFLAG_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_TXPKTCTL_B1_ERRFLAG_ISR));
	PLTFM_MSG_ALWAYS("[6]R_AX_PLE_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_PLE_ERR_IMR));
	PLTFM_MSG_ALWAYS("[6]R_AX_PLE_ERR_FLAG_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_PLE_ERR_FLAG_ISR));
	PLTFM_MSG_ALWAYS("[7]R_AX_PKTIN_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_PKTIN_ERR_IMR));
	PLTFM_MSG_ALWAYS("[7]R_AX_PKTIN_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_PKTIN_ERR_ISR));
	PLTFM_MSG_ALWAYS("[8.1]R_AX_OTHER_DISPATCHER_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_OTHER_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ALWAYS("[8.1]R_AX_OTHER_DISPATCHER_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_OTHER_DISPATCHER_ERR_ISR));
	PLTFM_MSG_ALWAYS("[8.2]R_AX_HOST_DISPATCHER_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_HOST_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ALWAYS("[8.2]R_AX_HOST_DISPATCHER_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_HOST_DISPATCHER_ERR_ISR));
	PLTFM_MSG_ALWAYS("[8.3]R_AX_CPU_DISPATCHER_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_CPU_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ALWAYS("[8.3]R_AX_CPU_DISPATCHER_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_CPU_DISPATCHER_ERR_ISR));
	PLTFM_MSG_ALWAYS("[10]R_AX_CPUIO_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_CPUIO_ERR_IMR));
	PLTFM_MSG_ALWAYS("[10]R_AX_CPUIO_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_CPUIO_ERR_ISR));
	PLTFM_MSG_ALWAYS("[11.1]R_AX_BBRPT_COM_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_BBRPT_COM_ERR_ISR));
	PLTFM_MSG_ALWAYS("[11.2]R_AX_BBRPT_CHINFO_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_ISR));
	PLTFM_MSG_ALWAYS("[11.3]R_AX_BBRPT_DFS_ERR_ISR=0x%x\n",
			 MAC_REG_R32(R_AX_BBRPT_DFS_ERR_ISR));
	PLTFM_MSG_ALWAYS("[11.4]R_AX_LA_ERRFLAG=0x%x\n",
			 MAC_REG_R32(R_AX_LA_ERRFLAG));
	PLTFM_MSG_ALWAYS("R_AX_HCI_FC_ERR_FLAG=0x%x\n",
			 MAC_REG_R32(R_AX_HCI_FC_ERR_FLAG_V1));
	/* Dispatcher */
	PLTFM_MSG_ALWAYS("R_AX_DISPATCHER_GLOBAL_SETTING_0=0x%x\n",
			 MAC_REG_R32(R_AX_DISPATCHER_GLOBAL_SETTING_0));
	PLTFM_MSG_ALWAYS("R_AX_TX_ADDRESS_INFO_MODE_SETTING=0x%x\n",
			 MAC_REG_R32(R_AX_TX_ADDRESS_INFO_MODE_SETTING));
	PLTFM_MSG_ALWAYS("R_AX_CPU_PORT_DEBUG_SETTING=0x%x\n",
			 MAC_REG_R32(R_AX_CPU_PORT_DEBUG_SETTING));
	PLTFM_MSG_ALWAYS("R_AX_HDP_DBG_INFO_2_V1=0x%x\n",
			 MAC_REG_R32(R_AX_HDP_DBG_INFO_2_V1));
	/* PKTIN */
	PLTFM_MSG_ALWAYS("R_AX_PKTIN_SETTING=0x%x\n",
			 MAC_REG_R32(R_AX_PKTIN_SETTING));
	/* MPDU Proc */
	PLTFM_MSG_ALWAYS("R_AX_TX_PTK_CNT=0x%x\n",
			 MAC_REG_R32(R_AX_TX_PTK_CNT));
	PLTFM_MSG_ALWAYS("R_AX_MPDU_TX_ERRFLAG=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_TX_ERR_ISR));
	PLTFM_MSG_ALWAYS("R_AX_MPDU_TX_ERRFLAG_MSK=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_TX_ERR_IMR));
	PLTFM_MSG_ALWAYS("R_AX_MPDU_RX_PKTCNT=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_RX_PKTCNT));
	PLTFM_MSG_ALWAYS("R_AX_MPDU_DROP_PKTCNT=0x%x\n",
			 MAC_REG_R32(R_AX_MPDU_DROP_PKTCNT));
	/* STA SCH */
	PLTFM_MSG_ALWAYS("R_AX_SS_CTRL=0x%x\n", MAC_REG_R32(R_AX_SS_CTRL));
	PLTFM_MSG_ALWAYS("R_AX_SS_DBG_0=0x%x\n", MAC_REG_R32(R_AX_SS_DBG_0));
	PLTFM_MSG_ALWAYS("R_AX_SS_DBG_1=0x%x\n", MAC_REG_R32(R_AX_SS_DBG_1));
	PLTFM_MSG_ALWAYS("R_AX_SS_DBG_2=0x%x\n", MAC_REG_R32(R_AX_SS_DBG_2));
	PLTFM_MSG_ALWAYS("R_AX_SS_DBG_3=0x%x\n", MAC_REG_R32(R_AX_SS_DBG_3));
	/* DLE */
	PLTFM_MSG_ALWAYS("R_AX_WDE_ERRFLAG_MSG = 0x%x\n",
			 MAC_REG_R32(R_AX_WDE_ERRFLAG_MSG));
	PLTFM_MSG_ALWAYS("R_AX_WDE_DBGERR_LOCKEN = 0x%x\n",
			 MAC_REG_R32(R_AX_WDE_DBGERR_LOCKEN));
	PLTFM_MSG_ALWAYS("R_AX_WDE_DBGERR_STS = 0x%x\n",
			 MAC_REG_R32(R_AX_WDE_DBGERR_STS));
	PLTFM_MSG_ALWAYS("R_AX_PLE_ERRFLAG_MSG = 0x%x\n",
			 MAC_REG_R32(R_AX_PLE_ERRFLAG_MSG));
	PLTFM_MSG_ALWAYS("R_AX_PLE_DBGERR_LOCKEN = 0x%x\n",
			 MAC_REG_R32(R_AX_PLE_DBGERR_LOCKEN));
	PLTFM_MSG_ALWAYS("R_AX_PLE_DBGERR_STS = 0x%x\n",
			 MAC_REG_R32(R_AX_PLE_DBGERR_STS));
	/* HAXIDMA */
	PLTFM_MSG_ALWAYS("R_AX_HAXI_INIT_CFG1=0x%x\n",
			 MAC_REG_R32(R_AX_HAXI_INIT_CFG1));
	if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
		PLTFM_MSG_ALWAYS("R_AX_HAXI_DMA_STOP1=0x%x\n",
				 MAC_REG_R32(R_AX_HAXI_DMA_STOP1));
		PLTFM_MSG_ALWAYS("R_AX_HAXI_DMA_BUSY1=0x%x\n",
				 MAC_REG_R32(R_AX_HAXI_DMA_BUSY1));
		PLTFM_MSG_ALWAYS("R_AX_HAXI_DBG_CTRL=0x%x\n",
				 MAC_REG_R32(R_AX_HAXI_DBG_CTRL));
		PLTFM_MSG_ALWAYS("R_AX_DBG_ERR_FLAG_V1=0x%x\n",
				 MAC_REG_R32(R_AX_DBG_ERR_FLAG_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH0_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH0_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH1_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH1_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH2_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH2_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH3_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH3_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH4_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH4_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH5_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH5_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH6_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH6_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_ACH7_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_ACH7_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_CH8_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_CH8_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_CH9_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_CH9_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_CH12_TXBD_IDX=0x%x\n",
				 MAC_REG_R32(R_AX_CH12_TXBD_IDX));
		PLTFM_MSG_ALWAYS("R_AX_RXQ_RXBD_IDX_V1=0x%x\n",
				 MAC_REG_R32(R_AX_RXQ_RXBD_IDX_V1));
		PLTFM_MSG_ALWAYS("R_AX_RPQ_RXBD_IDX_V1=0x%x\n",
				 MAC_REG_R32(R_AX_RPQ_RXBD_IDX_V1));
	}

	return MACSUCCESS;
}

u32 ss_stat_chk_8852c(struct mac_ax_adapter *adapter)
{
	u32 val32 = 0;
	u32 r_val32;
	u8 fw_vld;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (adapter->hw_info->chip_id) {
	case MAC_AX_CHIP_ID_8852A:
		fw_vld = SS_FW_SUPPORT_8852A;
		break;
	case MAC_AX_CHIP_ID_8852B:
		fw_vld = SS_FW_SUPPORT_8852B;
		break;
	default:
		fw_vld = 0;
		break;
	}

	r_val32 = MAC_REG_R32(R_AX_SS_DBG_3);
	if (r_val32 & B_AX_SS_HW_DECR_LEN_UDN)
		val32 |= SS_TX_HW_LEN_UDN;
	if (r_val32 & B_AX_SS_SW_DECR_LEN_UDN)
		val32 |= SS_TX_SW_LEN_UDN;
	if (r_val32 & B_AX_SS_HW_ADD_LEN_OVF)
		val32 |= SS_TX_HW_LEN_OVF;

	r_val32 = MAC_REG_R32(R_AX_SS_DBG_2);
	if (fw_vld) {
		if (GET_FIELD(r_val32, B_AX_SS_FWTX_STAT) != 1)
			val32 |= SS_STAT_FWTX;
	}
	if (GET_FIELD(r_val32, B_AX_SS_RPTA_STAT) != 1)
		val32 |= SS_STAT_RPTA;
	if (GET_FIELD(r_val32, B_AX_SS_WDEA_STAT_V1) != 1)
		val32 |= SS_STAT_WDEA;
	if (GET_FIELD(r_val32, B_AX_SS_PLEA_STAT_V1) != 1)
		val32 |= SS_STAT_PLEA;

	r_val32 = MAC_REG_R32(R_AX_SS_DBG_1);
	if (GET_FIELD(r_val32, B_AX_SS_ULRU_STAT) > 1)
		val32 |= SS_STAT_ULRU;
	if (GET_FIELD(r_val32, B_AX_SS_DLTX_STAT) > 1)
		val32 |= SS_STAT_DLTX;

	return val32;
}

u32 tx_flow_ptcl_dbg_port_8852c(struct mac_ax_adapter *adapter, u8 band)
{
	u8 i;
	u16 val16;
	u32 ret = MACSUCCESS;
	struct mac_ax_dbg_port_info info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ALWAYS("[ERR] check cmac en %d\n", ret);
		return ret;
	}
	info = band == MAC_AX_BAND_1 ? dbg_port_ptcl_c1 : dbg_port_ptcl_c0;
	PLTFM_MUTEX_LOCK(&adapter->hw_info->dbg_port_lock);
	adapter->hw_info->dbg_port_cnt++;
	if (adapter->hw_info->dbg_port_cnt != 1) {
		PLTFM_MSG_ERR("[ERR]dbg port sel %x lock cnt %d\n",
			      info.sel_addr, adapter->hw_info->dbg_port_cnt);
		ret = MACCMP;
		goto err;
	}

	val16 = MAC_REG_R16(info.sel_addr);
	val16 |= B_AX_PTCL_DBG_EN;
	MAC_REG_W16(info.sel_addr, val16);
	PLTFM_MSG_ALWAYS("Sel addr = 0x%X\n", info.sel_addr);
	PLTFM_MSG_ALWAYS("Read addr = 0x%X\n", info.rd_addr);

	info.srt = PTCL_SEL_FSM_0;
	info.end = PTCL_SEL_FSM_1;
	for (i = 0; i < PTCL_DBG_DMP_CNT; i++)
		print_dbg_port(adapter, &info);

	info.srt = PTCL_SEL_PHY_DBG;
	info.end = PTCL_SEL_PHY_DBG;
	for (i = 0; i < PTCL_DBG_DMP_CNT; i++)
		print_dbg_port(adapter, &info);

err:
	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);
	return ret;
}

u32 tx_flow_sch_dbg_port_8852c(struct mac_ax_adapter *adapter, u8 band)
{
	u8 i;
	u32 val32, ret = MACSUCCESS;
	struct mac_ax_dbg_port_info info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ALWAYS("[ERR] check cmac en %d\n", ret);
		return ret;
	}

	info = band == MAC_AX_BAND_1 ? dbg_port_sch_c1 : dbg_port_sch_c0;
	PLTFM_MUTEX_LOCK(&adapter->hw_info->dbg_port_lock);
	adapter->hw_info->dbg_port_cnt++;
	if (adapter->hw_info->dbg_port_cnt != 1) {
		PLTFM_MSG_ERR("[ERR]dbg port sel %x lock cnt %d\n",
			      info.sel_addr, adapter->hw_info->dbg_port_cnt);
		ret = MACCMP;
		goto err;
	}

	val32 = MAC_REG_R32(info.sel_addr);
	val32 |= B_AX_SCH_DBG_EN;
	MAC_REG_W32(info.sel_addr, val32);

	info.srt = SCH_SEL_PREBKF_DBG_1;
	info.end = SCH_SEL_TX_NAV_ABORT_DBG;

	PLTFM_MSG_ALWAYS("Sel addr = 0x%X\n", info.sel_addr);
	PLTFM_MSG_ALWAYS("Read addr = 0x%X\n", info.rd_addr);

	for (i = 0; i < SCH_DBG_DMP_CNT; i++)
		print_dbg_port(adapter, &info);

err:
	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);
	return ret;
}

u32 dbg_port_sel_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_dbg_port_info **info, u32 sel)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u16 val16;
	u8 val8, index;
	u32 ret = MACSUCCESS;

	PLTFM_MUTEX_LOCK(&adapter->hw_info->dbg_port_lock);
	adapter->hw_info->dbg_port_cnt++;

	if (adapter->hw_info->dbg_port_cnt != 1) {
		PLTFM_MSG_ERR("[ERR]dbg port sel %d lock cnt %d\n", sel,
			      adapter->hw_info->dbg_port_cnt);
		ret = MACCMP;
		goto err;
	}

	switch (sel) {
	case MAC_AX_DBG_PORT_SEL_PTCL_C0:
		*info = &dbg_port_ptcl_c0;
		val16 = MAC_REG_R16(R_AX_PTCL_DBG);
		val16 |= B_AX_PTCL_DBG_EN;
		MAC_REG_W16(R_AX_PTCL_DBG, val16);
		PLTFM_MSG_ALWAYS("Enable PTCL C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PTCL_C1:
		*info = &dbg_port_ptcl_c1;
		val16 = MAC_REG_R16(R_AX_PTCL_DBG_C1);
		val16 |= B_AX_PTCL_DBG_EN;
		MAC_REG_W16(R_AX_PTCL_DBG_C1, val16);
		PLTFM_MSG_ALWAYS("Enable PTCL C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_SCH_C0:
		*info = &dbg_port_sch_c0;
		val32 = MAC_REG_R32(R_AX_SCH_DBG_SEL);
		val32 |= B_AX_SCH_DBG_EN;
		MAC_REG_W32(R_AX_SCH_DBG_SEL, val32);
		PLTFM_MSG_ALWAYS("Enable SCH C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_SCH_C1:
		*info = &dbg_port_sch_c1;
		val32 = MAC_REG_R32(R_AX_SCH_DBG_SEL_C1);
		val32 |= B_AX_SCH_DBG_EN;
		MAC_REG_W32(R_AX_SCH_DBG_SEL_C1, val32);
		PLTFM_MSG_ALWAYS("Enable SCH C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TMAC_C0:
		*info = &dbg_port_tmac_v1_c0;
		val32 = MAC_REG_R32(R_AX_DBGSEL_TRXPTCL);
		val32 = SET_CLR_WORD(val32, TRXPTRL_DBG_SEL_TMAC,
				     B_AX_DBGSEL_TRXPTCL);
		MAC_REG_W32(R_AX_DBGSEL_TRXPTCL, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, TMAC_DBG_SEL_C0, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, TMAC_DBG_SEL_C0, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable TMAC C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TMAC_C1:
		*info = &dbg_port_tmac_v1_c1;
		val32 = MAC_REG_R32(R_AX_DBGSEL_TRXPTCL_C1);
		val32 = SET_CLR_WORD(val32, TRXPTRL_DBG_SEL_TMAC,
				     B_AX_DBGSEL_TRXPTCL);
		MAC_REG_W32(R_AX_DBGSEL_TRXPTCL_C1, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, TMAC_DBG_SEL_C1, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, TMAC_DBG_SEL_C1, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable TMAC C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_C0:
		*info = &dbg_port_rmac_c0;
		val32 = MAC_REG_R32(R_AX_DBGSEL_TRXPTCL);
		val32 = SET_CLR_WORD(val32, TRXPTRL_DBG_SEL_RMAC,
				     B_AX_DBGSEL_TRXPTCL);
		MAC_REG_W32(R_AX_DBGSEL_TRXPTCL, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, RMAC_DBG_SEL_C0, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, RMAC_DBG_SEL_C0, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);

		val8 = MAC_REG_R8(R_AX_DBGSEL_TRXPTCL);
		val8 = (u8)(SET_CLR_WORD(val8, RMAC_CMAC_DBG_SEL,
					 B_AX_DBGSEL_TRXPTCL));
		MAC_REG_W8(R_AX_DBGSEL_TRXPTCL, val8);
		PLTFM_MSG_ALWAYS("Enable RMAC C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_C1:
		*info = &dbg_port_rmac_c1;
		val32 = MAC_REG_R32(R_AX_DBGSEL_TRXPTCL_C1);
		val32 = SET_CLR_WORD(val32, TRXPTRL_DBG_SEL_RMAC,
				     B_AX_DBGSEL_TRXPTCL);
		MAC_REG_W32(R_AX_DBGSEL_TRXPTCL_C1, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, RMAC_DBG_SEL_C1, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, RMAC_DBG_SEL_C1, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);

		val8 = MAC_REG_R8(R_AX_DBGSEL_TRXPTCL_C1);
		val8 = (u8)(SET_CLR_WORD(val8, RMAC_CMAC_DBG_SEL,
					 B_AX_DBGSEL_TRXPTCL));
		MAC_REG_W8(R_AX_DBGSEL_TRXPTCL_C1, val8);
		PLTFM_MSG_ALWAYS("Enable RMAC C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMACST_C0:
		*info = &dbg_port_rmacst_c0;
		PLTFM_MSG_ALWAYS("Enable RMAC state C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMACST_C1:
		*info = &dbg_port_rmacst_c1;
		PLTFM_MSG_ALWAYS("Enable RMAC state C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_PLCP_C0:
		*info = &dbg_port_rmac_plcp_c0;
		PLTFM_MSG_ALWAYS("Enable RMAC PLCP C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_PLCP_C1:
		*info = &dbg_port_rmac_plcp_c1;
		PLTFM_MSG_ALWAYS("Enable RMAC PLCP C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TRXPTCL_C0:
		*info = &dbg_port_trxptcl_v1_c0;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, TRXPTCL_DBG_SEL_C0, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, TRXPTCL_DBG_SEL_C0, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable TRXPTCL C0 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TRXPTCL_C1:
		*info = &dbg_port_trxptcl_v1_c1;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, TRXPTCL_DBG_SEL_C1, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, TRXPTCL_DBG_SEL_C1, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable TRXPTCL C1 dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C0:
		*info = &dbg_port_tx_infol_c0;
		val32 = MAC_REG_R32(R_AX_TCR1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1, val32);
		PLTFM_MSG_ALWAYS("Enable tx infol dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C0:
		*info = &dbg_port_tx_infoh_c0;
		val32 = MAC_REG_R32(R_AX_TCR1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1, val32);
		PLTFM_MSG_ALWAYS("Enable tx infoh dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C1:
		*info = &dbg_port_tx_infol_c1;
		val32 = MAC_REG_R32(R_AX_TCR1_C1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1_C1, val32);
		PLTFM_MSG_ALWAYS("Enable tx infol dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C1:
		*info = &dbg_port_tx_infoh_c1;
		val32 = MAC_REG_R32(R_AX_TCR1_C1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1_C1, val32);
		PLTFM_MSG_ALWAYS("Enable tx infoh dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C0:
		*info = &dbg_port_txtf_infol_c0;
		val32 = MAC_REG_R32(R_AX_TCR1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1, val32);
		PLTFM_MSG_ALWAYS("Enable tx tf infol dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C0:
		*info = &dbg_port_txtf_infoh_c0;
		val32 = MAC_REG_R32(R_AX_TCR1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1, val32);
		PLTFM_MSG_ALWAYS("Enable tx tf infoh dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C1:
		*info = &dbg_port_txtf_infol_c1;
		val32 = MAC_REG_R32(R_AX_TCR1_C1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1_C1, val32);
		PLTFM_MSG_ALWAYS("Enable tx tf infol dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C1:
		*info = &dbg_port_txtf_infoh_c1;
		val32 = MAC_REG_R32(R_AX_TCR1_C1);
		val32 |= B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1_C1, val32);
		PLTFM_MSG_ALWAYS("Enable tx tf infoh dump.\n");
		break;
#if 0
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA0_C0:
		*info = &dbg_port_cmac_dma0_c0;

		val32 = MAC_REG_R32(R_AX_TXDMA_DBG) | B_AX_TXDMA_DBG_EN;
		MAC_REG_W32(R_AX_TXDMA_DBG, val32);

		val32 = MAC_REG_R32(R_AX_RXDMA_CTRL_0) | B_AX_RXDMA_DBGOUT_EN;
		MAC_REG_W32(R_AX_RXDMA_CTRL_0, val32);

		val32 = MAC_REG_R32(R_AX_DLE_CTRL) | B_AX_DMA_DBG_SEL;
		MAC_REG_W32(R_AX_DLE_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C0, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C0, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		PLTFM_MSG_ALWAYS("Enable cmac0 dma0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C0:
		*info = &dbg_port_cmac_dma1_c0;

		val32 = MAC_REG_R32(R_AX_TXDMA_DBG) | B_AX_TXDMA_DBG_EN;
		MAC_REG_W32(R_AX_TXDMA_DBG, val32);

		val32 = MAC_REG_R32(R_AX_RXDMA_CTRL_0) | B_AX_RXDMA_DBGOUT_EN;
		MAC_REG_W32(R_AX_RXDMA_CTRL_0, val32);

		val32 = MAC_REG_R32(R_AX_DLE_CTRL) & ~B_AX_DMA_DBG_SEL;
		MAC_REG_W32(R_AX_DLE_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C0, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C0, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		PLTFM_MSG_ALWAYS("Enable cmac0 dma1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA0_C1:
		*info = &dbg_port_cmac_dma0_c1;

		val32 = MAC_REG_R32(R_AX_TXDMA_DBG_C1) | B_AX_TXDMA_DBG_EN;
		MAC_REG_W32(R_AX_TXDMA_DBG_C1, val32);

		val32 = MAC_REG_R32(R_AX_RXDMA_CTRL_0_C1) |
			B_AX_RXDMA_DBGOUT_EN;
		MAC_REG_W32(R_AX_RXDMA_CTRL_0_C1, val32);

		val32 = MAC_REG_R32(R_AX_DLE_CTRL_C1) | B_AX_DMA_DBG_SEL;
		MAC_REG_W32(R_AX_DLE_CTRL_C1, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C1, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C1, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		PLTFM_MSG_ALWAYS("Enable cmac1 dma0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C1:
		*info = &dbg_port_cmac_dma1_c1;

		val32 = MAC_REG_R32(R_AX_TXDMA_DBG_C1) | B_AX_TXDMA_DBG_EN;
		MAC_REG_W32(R_AX_TXDMA_DBG_C1, val32);

		val32 = MAC_REG_R32(R_AX_RXDMA_CTRL_0_C1) |
			B_AX_RXDMA_DBGOUT_EN;
		MAC_REG_W32(R_AX_RXDMA_CTRL_0_C1, val32);

		val32 = MAC_REG_R32(R_AX_DLE_CTRL_C1) & ~B_AX_DMA_DBG_SEL;
		MAC_REG_W32(R_AX_DLE_CTRL_C1, val32);

		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C1, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, CMAC_DMA_DBG_SEL_C1, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		PLTFM_MSG_ALWAYS("Enable cmac1 dma1 dump.\n");
		break;
#endif

	case MAC_AX_DBG_PORT_SEL_WDE_BUFMGN_CTL:
		*info = &dbg_port_wde_bufmgn_ctl;
		PLTFM_MSG_ALWAYS("Enable wde bufmgn ctl dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_BUFMGN_ARB:
		*info = &dbg_port_wde_bufmgn_arb;
		PLTFM_MSG_ALWAYS("Enable wde bufmgn arb dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_CTL:
		*info = &dbg_port_wde_quemgn_ctl;
		PLTFM_MSG_ALWAYS("Enable wde quemgn ctl dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_INFO:
		*info = &dbg_port_wde_quemgn_info;
		PLTFM_MSG_ALWAYS("Enable wde quemgn info dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_ARB:
		*info = &dbg_port_wde_quemgn_arb;
		PLTFM_MSG_ALWAYS("Enable wde quemgn arb dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT0:
		*info = &dbg_port_wde_portif0;
		PLTFM_MSG_ALWAYS("Enable wde port 0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT1:
		*info = &dbg_port_wde_portif1;
		PLTFM_MSG_ALWAYS("Enable wde port 1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT3:
		*info = &dbg_port_wde_portif3;
		PLTFM_MSG_ALWAYS("Enable wde port 3 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT4:
		*info = &dbg_port_wde_portif4;
		PLTFM_MSG_ALWAYS("Enable wde port 4 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT6:
		*info = &dbg_port_wde_portif6;
		PLTFM_MSG_ALWAYS("Enable wde port 6 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT7:
		*info = &dbg_port_wde_portif7;
		PLTFM_MSG_ALWAYS("Enable wde port 7 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_BUFMGN_CTL:
		*info = &dbg_port_ple_bufmgn_ctl;
		PLTFM_MSG_ALWAYS("Enable ple bufmgn ctl dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_BUFMGN_ARB:
		*info = &dbg_port_ple_bufmgn_ctl;
		PLTFM_MSG_ALWAYS("Enable ple bufmgn arb dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_CTL:
		*info = &dbg_port_ple_quemgn_ctl;
		PLTFM_MSG_ALWAYS("Enable ple quemgn ctl dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_INFO:
		*info = &dbg_port_ple_quemgn_info;
		PLTFM_MSG_ALWAYS("Enable ple quemgn info dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_ARB:
		*info = &dbg_port_ple_quemgn_arb;
		PLTFM_MSG_ALWAYS("Enable ple quemgn arb dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT0:
		*info = &dbg_port_ple_portif0;
		PLTFM_MSG_ALWAYS("Enable ple port 0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT1:
		*info = &dbg_port_ple_portif1;
		PLTFM_MSG_ALWAYS("Enable ple port 1_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT2:
		*info = &dbg_port_ple_portif2_0;
		PLTFM_MSG_ALWAYS("Enable ple port 2 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT2_1:
		*info = &dbg_port_ple_portif2_1;
		PLTFM_MSG_ALWAYS("Enable ple port 2_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT3:
		*info = &dbg_port_ple_portif3;
		PLTFM_MSG_ALWAYS("Enable ple port 3 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT4:
		*info = &dbg_port_ple_portif4;
		PLTFM_MSG_ALWAYS("Enable ple port 4 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT5:
		*info = &dbg_port_ple_portif5;
		PLTFM_MSG_ALWAYS("Enable ple port 5 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT6:
		*info = &dbg_port_ple_portif6;
		PLTFM_MSG_ALWAYS("Enable ple port 6 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_CTL:
		*info = &dbg_port_wdrls_ctl;
		PLTFM_MSG_ALWAYS("Enable wdrls ctl dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_RPTGEN0:
		*info = &dbg_port_wdrls_rptgen0;
		PLTFM_MSG_ALWAYS("Enable wdrls rptgen0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_RPTGEN1:
		*info = &dbg_port_wdrls_rptgen1;
		PLTFM_MSG_ALWAYS("Enable wdrls rptgen1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_PLED_CH0:
		*info = &dbg_port_wdrls_pledchn0;
		PLTFM_MSG_ALWAYS("Enable wdrls pledchn0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_PLED_CH1:
		*info = &dbg_port_wdrls_pledchn1;
		PLTFM_MSG_ALWAYS("Enable wdrls pledchn1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_FETPKT:
		*info = &dbg_port_txpktctrl_fetpkt;
		PLTFM_MSG_ALWAYS("Enable txpktctrl fetpkt dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_CMDPSR:
		*info = &dbg_port_txpktctrl_b0_cmdpsr;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 cmdpsr dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_CMACDMAIF:
		*info = &dbg_port_txpktctrl_b0_cmacdmaif;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 cmacdmaif dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_PRELD0:
		*info = &dbg_port_txpktctrl_b0_preld0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 preld0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_PRELD1:
		*info = &dbg_port_txpktctrl_b0_preld1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 preld1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT0_0:
		*info = &dbg_port_txpktctrl_b0_unit0_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit0_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT0_1:
		*info = &dbg_port_txpktctrl_b0_unit0_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit0_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT1_0:
		*info = &dbg_port_txpktctrl_b0_unit1_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit1_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT1_1:
		*info = &dbg_port_txpktctrl_b0_unit1_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit1_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT2_0:
		*info = &dbg_port_txpktctrl_b0_unit2_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit2_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT2_1:
		*info = &dbg_port_txpktctrl_b0_unit2_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit2_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT3_0:
		*info = &dbg_port_txpktctrl_b0_unit3_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit3_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT3_1:
		*info = &dbg_port_txpktctrl_b0_unit3_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit3_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT4_0:
		*info = &dbg_port_txpktctrl_b0_unit4_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit4_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT4_1:
		*info = &dbg_port_txpktctrl_b0_unit4_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit4_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT5_0:
		*info = &dbg_port_txpktctrl_b0_unit5_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit5_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT5_1:
		*info = &dbg_port_txpktctrl_b0_unit5_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit5_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT6_0:
		*info = &dbg_port_txpktctrl_b0_unit6_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit6_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT6_1:
		*info = &dbg_port_txpktctrl_b0_unit6_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit6_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT7_0:
		*info = &dbg_port_txpktctrl_b0_unit7_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit7_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT7_1:
		*info = &dbg_port_txpktctrl_b0_unit7_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b0 unit7_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_CMDPSR:
		*info = &dbg_port_txpktctrl_b1_cmdpsr;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b1 cmdpsr dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_CMACDMAIF:
		*info = &dbg_port_txpktctrl_b1_cmacdmaif;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b1 cmacdmaif dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_UNIT0_0:
		*info = &dbg_port_txpktctrl_b1_unit0_0;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b1 unit0_0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_UNIT0_1:
		*info = &dbg_port_txpktctrl_b1_unit0_1;
		PLTFM_MSG_ALWAYS("Enable txpktctrl b1 unit0_1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PKTINFO:
		*info = &dbg_port_pktinfo;
		PLTFM_MSG_ALWAYS("Enable pktinfo dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_MPDUINFO_B0:
		*info = &dbg_port_mpduinfo_b0;
		PLTFM_MSG_ALWAYS("Enable mpduinfo b0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_MPDUINFO_B1:
		*info = &dbg_port_mpduinfo_b1;
		PLTFM_MSG_ALWAYS("Enable mpduinfo b1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PRELD_B0:
		*info = &dbg_port_preld_b0;
		PLTFM_MSG_ALWAYS("Enable preld b0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PRELD_B1:
		*info = &dbg_port_preld_b1;
		PLTFM_MSG_ALWAYS("Enable preld b1 dump.\n");
		break;

#if 0
	case MAC_AX_DBG_PORT_SEL_PCIE_TXDMA:
		*info = &dbg_port_pcie_txdma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_TXDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_TXDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie txdma dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_RXDMA:
		*info = &dbg_port_pcie_rxdma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_RXDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_RXDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie rxdma dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_CVT:
		*info = &dbg_port_pcie_cvt;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_CVT_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_CVT_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie cvt dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC04:
		*info = &dbg_port_pcie_emac04;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC04_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC04_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 0-4 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC5:
		*info = &dbg_port_pcie_emac5;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC5_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC5_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 5 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC6:
		*info = &dbg_port_pcie_emac6;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC6_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC6_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 6 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC7:
		*info = &dbg_port_pcie_emac7;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC7_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC7_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 7 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_PNP_IO:
		*info = &dbg_port_pcie_pnp_io;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_PNP_IO_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_PNP_IO_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie p np io dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC814:
		*info = &dbg_port_pcie_emac814;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC814_DBG_SEL,
				     B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC814_DBG_SEL,
				     B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 8-14 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC15:
		*info = &dbg_port_pcie_emac15;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC15_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC15_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 15 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC16:
		*info = &dbg_port_pcie_emac16;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC16_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC16_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 16 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC17:
		*info = &dbg_port_pcie_emac17;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC17_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC17_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 17 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC18:
		*info = &dbg_port_pcie_emac18;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC18_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PCIE_EMAC18_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable pcie emac 18 dump.\n");
		break;
#endif
	case MAC_AX_DBG_PORT_SEL_USB2_PHY:
		*info = &dbg_port_usb2_phy;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_PHY_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_PHY_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_PHY dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE:
		*info = &dbg_port_usb2_sie;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_SIE_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_SIE_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_SIE dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_UTMI:
		*info = &dbg_port_usb2_utmi;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_UTMI_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_UTMI_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_UTMI dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE_MMU:
		*info = &dbg_port_usb2_sie_mmu;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_SIE_MMU_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_SIE_MMU_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_SIE_MMU dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE_PCE:
		*info = &dbg_port_usb2_sie_pce;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_SIE_PCE_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_SIE_PCE_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_SIE_PCE dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_UTMI_IF:
		*info = &dbg_port_usb2_utmi_if;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_UTMI_IF_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_UTMI_IF_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_UTMI_IF dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLTX:
		*info = &dbg_port_usb_wltx;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_WLTX_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_WLTX_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_WLTX dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLRX:
		*info = &dbg_port_usb_wlrx;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_WLRX_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_WLRX_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_WLRX dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB3:
		*info = &dbg_port_usb3;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB3_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB3_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB3 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_SETUP:
		*info = &dbg_port_usb_setup;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_SETUP_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_SETUP_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_SETUP dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLTX_DMA:
		*info = &dbg_port_usb_wltx_dma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_WLTXDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_WLTXDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_WLTX_DMA dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLRX_DMA:
		*info = &dbg_port_usb_wlrx_dma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_WLRXDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_WLRXDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_WLRX_DMA dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_AINST:
		*info = &dbg_port_usb_ainst;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_AINST_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_AINST_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_AINST_DMA dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_MISC:
		*info = &dbg_port_usb_misc;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_MISC_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_MISC_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_MISC dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_BTTX:
		*info = &dbg_port_usb_bttx;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB_BTTX_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB_BTTX_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB_BTTX dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_BT:
		*info = &dbg_port_usb2_bt;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, USB2_BT_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, USB2_BT_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_ALWAYS("Enable USB2_BT dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX1:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX2:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX3:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX4:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX5:
		if (sel == MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0) {
			val32 = MAC_REG_R32(R_AX_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, DISPATCHER_DBG_SEL,
					     B_AX_DBG_SEL0);
			MAC_REG_W32(R_AX_DBG_CTRL, val32);

			val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
			val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
			MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		}
		*info = &dbg_port_dspt_hdt_tx0_5;
		index = (u8)(sel - MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX6:
		*info = &dbg_port_dspt_hdt_tx6;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 6, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx6 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX7:
		*info = &dbg_port_dspt_hdt_tx7;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 7, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx7 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX8:
		*info = &dbg_port_dspt_hdt_tx8;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 8, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx8 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX9:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXA:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXB:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXC:
		*info = &dbg_port_dspt_hdt_tx9_C;
		index = (u8)(sel + 9 - MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX9);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXD:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXE:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXF:
		*info = &dbg_port_dspt_hdt_txD_F;
		index = (u8)(sel + 0xD - MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXD);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt tx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX0:
		*info = &dbg_port_dspt_cdt_tx0;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX1:
		*info = &dbg_port_dspt_cdt_tx1;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX3:
		*info = &dbg_port_dspt_cdt_tx3;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 3, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx3 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX4:
		*info = &dbg_port_dspt_cdt_tx4;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 4, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx4 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX5:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX6:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX7:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX8:
		*info = &dbg_port_dspt_cdt_tx5_8;
		index = (u8)(sel + 5 - MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX5);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX9:
		*info = &dbg_port_dspt_cdt_tx9;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 9, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx9 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXA:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXB:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXC:
		*info = &dbg_port_dspt_cdt_txA_C;
		index = (u8)(sel + 0xA - MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXA);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt tx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX0:
		*info = &dbg_port_dspt_hdt_rx0;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt rx0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX1:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX2:
		*info = &dbg_port_dspt_hdt_rx1_2;
		index = (u8)(sel + 1 - MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX1);
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, index, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt rx%x dump.\n", index);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX3:
		*info = &dbg_port_dspt_hdt_rx3;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 3, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt rx3 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX4:
		*info = &dbg_port_dspt_hdt_rx4;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 4, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt rx4 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX5:
		*info = &dbg_port_dspt_hdt_rx5;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 5, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher hdt rx5 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_0:
		*info = &dbg_port_dspt_cdt_rx_p0_0;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 3, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 0, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt rx part0 0 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_1:
		*info = &dbg_port_dspt_cdt_rx_p0_1;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 3, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 1, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt rx part0 1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_2:
		*info = &dbg_port_dspt_cdt_rx_p0_2;
		val16 = MAC_REG_R16((*info)->sel_addr);
		val16 = SET_CLR_WORD(val16, 3, B_AX_DISPATCHER_INTN_SEL);
		val16 = SET_CLR_WORD(val16, 2, B_AX_DISPATCHER_CH_SEL);
		MAC_REG_W16((*info)->sel_addr, val16);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt rx part0 2 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P1:
		*info = &dbg_port_dspt_cdt_rx_p1;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 3, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher cdt rx part1 dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_STF_CTRL:
		*info = &dbg_port_dspt_stf_ctrl;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 4, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher stf control dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_ADDR_CTRL:
		*info = &dbg_port_dspt_addr_ctrl;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 5, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher addr control dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_WDE_INTF:
		*info = &dbg_port_dspt_wde_intf;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 6, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher wde interface dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_PLE_INTF:
		*info = &dbg_port_dspt_ple_intf;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 7, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher ple interface dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_FLOW_CTRL:
		*info = &dbg_port_dspt_flow_ctrl;
		val8 = MAC_REG_R8((*info)->sel_addr);
		val8 = SET_CLR_WORD(val8, 8, B_AX_DISPATCHER_INTN_SEL);
		MAC_REG_W8((*info)->sel_addr, val8);
		PLTFM_MSG_ALWAYS("Enable Dispatcher flow control dump.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_TXDMA:
		*info = &dbg_port_paxi_txdma;
		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_WARN("%s ind access sel %d start\n", __func__, sel);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, AXIDMA_BASE_ADDR);
		PLTFM_MSG_ALWAYS("Enable PAXIDMA TXDMA dump\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_RXDMA:
		*info = &dbg_port_paxi_rxdma;
		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_WARN("%s ind access sel %d start\n", __func__, sel);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, AXIDMA_BASE_ADDR);
		PLTFM_MSG_ALWAYS("Enable PAXIDMA RXDMA dump\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_MST:
		*info = &dbg_port_paxi_mst;
		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_WARN("%s ind access sel %d start\n", __func__, sel);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, AXIDMA_BASE_ADDR);
		PLTFM_MSG_ALWAYS("Enable PAXIDMA MST dump\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_INT:
		*info = &dbg_port_paxi_int;
		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, PAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);
		PLTFM_MSG_WARN("%s ind access sel %d start\n", __func__, sel);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, AXIDMA_BASE_ADDR);
		PLTFM_MSG_ALWAYS("Enable PAXIDMA INT dump\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_TXDMA:
		*info = &dbg_port_haxi_txdma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable HAXIDMA TXDMA dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_RXDMA:
		*info = &dbg_port_haxi_rxdma;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable HAXIDMA RXDMA dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_MST:
		*info = &dbg_port_haxi_mst;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable HAXIDMA MST dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_INT:
		*info = &dbg_port_haxi_int;
		val32 = MAC_REG_R32(R_AX_DBG_CTRL);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL0);
		val32 = SET_CLR_WORD(val32, HAXIDMA_DBG_SEL, B_AX_DBG_SEL1);
		MAC_REG_W32(R_AX_DBG_CTRL, val32);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		PLTFM_MSG_ALWAYS("Enable HAXIDMA INT dbgport.\n");
		break;
	case MAC_AX_DBG_PORT_SEL_STA_SCH:
		*info = &dbg_port_sta_sch;
		PLTFM_MSG_ALWAYS("Enable STA_SCH dbgport.\n");
		break;
	default:
		PLTFM_MSG_ALWAYS("Dbg port select err\n");
		*info = NULL;
		ret = MACDBGPORTSEL;
		goto err;
	}

	return MACSUCCESS;
err:
	if (ret == MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dbg port sel has no err code\n");
		ret = MACPROCERR;
	}
	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);
	return ret;
}

u32 dbg_port_sel_rst_8852c(struct mac_ax_adapter *adapter, u32 sel)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	switch (sel) {
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C0:
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C0:
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C0:
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C0:
		val32 = MAC_REG_R32(R_AX_TCR1);
		val32 &= ~B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1, val32);
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C1:
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C1:
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C1:
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C1:
		val32 = MAC_REG_R32(R_AX_TCR1_C1);
		val32 &= ~B_AX_TCR_FORCE_READ_TXDFIFO;
		MAC_REG_W32(R_AX_TCR1_C1, val32);
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_TXDMA:
	case MAC_AX_DBG_PORT_SEL_PAXI_RXDMA:
	case MAC_AX_DBG_PORT_SEL_PAXI_MST:
	case MAC_AX_DBG_PORT_SEL_PAXI_INT:
		adapter->hw_info->ind_aces_cnt--;
		PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
		PLTFM_MSG_WARN("%s ind access sel %d end\n", __func__, sel);
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u8 is_dbg_port_not_valid_8852c(struct mac_ax_adapter *adapter, u32 dbg_sel)
{
	if (adapter->hw_info->intf != MAC_AX_INTF_PCIE &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_PCIE_TXDMA &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_PCIE_EMAC18)
		return 1;
	if (adapter->hw_info->intf != MAC_AX_INTF_USB &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_USB2_PHY &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_USB2_BT)
		return 1;
	if (check_mac_en(adapter, 0, MAC_AX_DMAC_SEL) != MACSUCCESS &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_WDE_BUFMGN_CTL &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_PRELD_B1)
		return 1;
	if (check_mac_en(adapter, 0, MAC_AX_DMAC_SEL) != MACSUCCESS &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0 &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_DSPT_FLOW_CTRL)
		return 1;
	if (check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL) != MACSUCCESS &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C0)
		return 1;
	if (check_mac_en(adapter, MAC_AX_BAND_1, MAC_AX_CMAC_SEL) != MACSUCCESS &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_PTCL_C1 &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C1)
		return 1;
	if (dbg_sel >= MAC_AX_DBG_PORT_SEL_AXI_TXDMA_CTRL &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_AXI_PAGE_FLOW_CTRL)
		return 1;
	if ((adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY ||
	     adapter->hw_info->is_sec_ic) &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_PAXI_TXDMA &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_PAXI_INT)
		return 1;
	if (check_mac_en(adapter, 0, MAC_AX_DMAC_SEL) != MACSUCCESS &&
	    dbg_sel >= MAC_AX_DBG_PORT_SEL_HAXI_TXDMA &&
	    dbg_sel <= MAC_AX_DBG_PORT_SEL_HAXI_INT)
		return 1;
	if (check_mac_en(adapter, 0, MAC_AX_DMAC_SEL) != MACSUCCESS &&
	    dbg_sel == MAC_AX_DBG_PORT_SEL_STA_SCH)
		return 1;

	return 0;
}

#endif /* MAC_AX_8852C_SUPPORT */
