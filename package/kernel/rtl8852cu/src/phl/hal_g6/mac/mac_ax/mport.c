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

#include "mport.h"
#include "../mac_ax.h"

#define MAX_TSF_SOURCE_PORT 7
#define MAX_TSF_TARGET_PORT 4
#define MAX_TSF_SYNC_OFFSET 0x3FFFF

static u32 cfg_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_PORT_CFG_P0, R_AX_PORT_CFG_P1, R_AX_PORT_CFG_P2,
	 R_AX_PORT_CFG_P3, R_AX_PORT_CFG_P4},
	{R_AX_PORT_CFG_P0_C1, R_AX_PORT_CFG_P1_C1, R_AX_PORT_CFG_P2_C1,
	 R_AX_PORT_CFG_P3_C1, R_AX_PORT_CFG_P4_C1}
};

static u32 phb_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_TBTT_PROHIB_P0, R_AX_TBTT_PROHIB_P1, R_AX_TBTT_PROHIB_P1,
	 R_AX_TBTT_PROHIB_P1, R_AX_TBTT_PROHIB_P1},
	{R_AX_TBTT_PROHIB_P0_C1, R_AX_TBTT_PROHIB_P1_C1, R_AX_TBTT_PROHIB_P1_C1,
	 R_AX_TBTT_PROHIB_P1_C1, R_AX_TBTT_PROHIB_P1_C1}
};

static u32 ctnarea_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCN_AREA_P0, R_AX_BCN_AREA_P1, R_AX_BCN_AREA_P1,
	 R_AX_BCN_AREA_P1, R_AX_BCN_AREA_P1},
	{R_AX_BCN_AREA_P0_C1, R_AX_BCN_AREA_P1_C1, R_AX_BCN_AREA_P1_C1,
	 R_AX_BCN_AREA_P1_C1, R_AX_BCN_AREA_P1_C1}
};

static u32 mskarea_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCN_AREA_P0, R_AX_BCN_AREA_P1, R_AX_BCN_AREA_P2,
	 R_AX_BCN_AREA_P3, R_AX_BCN_AREA_P4},
	{R_AX_BCN_AREA_P0_C1, R_AX_BCN_AREA_P1_C1, R_AX_BCN_AREA_P2_C1,
	 R_AX_BCN_AREA_P3_C1, R_AX_BCN_AREA_P4_C1}
};

static u32 tbttagg_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_TBTT_AGG_P0, R_AX_TBTT_AGG_P1, R_AX_TBTT_AGG_P2,
	 R_AX_TBTT_AGG_P3, R_AX_TBTT_AGG_P4},
	{R_AX_TBTT_AGG_P0_C1, R_AX_TBTT_AGG_P1_C1, R_AX_TBTT_AGG_P2_C1,
	 R_AX_TBTT_AGG_P3_C1, R_AX_TBTT_AGG_P4_C1}
};

static u32 tbttery_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_TBTTERLYINT_CFG_P0, R_AX_TBTTERLYINT_CFG_P1,
	 R_AX_TBTTERLYINT_CFG_P2, R_AX_TBTTERLYINT_CFG_P3,
	 R_AX_TBTTERLYINT_CFG_P4},
	{R_AX_TBTTERLYINT_CFG_P0_C1, R_AX_TBTTERLYINT_CFG_P1_C1,
	 R_AX_TBTTERLYINT_CFG_P2_C1, R_AX_TBTTERLYINT_CFG_P3_C1,
	 R_AX_TBTTERLYINT_CFG_P4_C1}
};

static u32 bcnspc_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCN_SPACE_CFG_P0, R_AX_BCN_SPACE_CFG_P1, R_AX_BCN_SPACE_CFG_P2,
	 R_AX_BCN_SPACE_CFG_P3, R_AX_BCN_SPACE_CFG_P4},
	{R_AX_BCN_SPACE_CFG_P0_C1, R_AX_BCN_SPACE_CFG_P1_C1,
	 R_AX_BCN_SPACE_CFG_P2_C1, R_AX_BCN_SPACE_CFG_P3_C1,
	 R_AX_BCN_SPACE_CFG_P4_C1}
};

static u32 tbttsht_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_TBTT_SHIFT_P0, R_AX_TBTT_SHIFT_P1, R_AX_TBTT_SHIFT_P2,
	 R_AX_TBTT_SHIFT_P3, R_AX_TBTT_SHIFT_P4},
	{R_AX_TBTT_SHIFT_P0_C1, R_AX_TBTT_SHIFT_P1_C1, R_AX_TBTT_SHIFT_P2_C1,
	 R_AX_TBTT_SHIFT_P3_C1, R_AX_TBTT_SHIFT_P4_C1}
};

static u32 bcndrp_regl[MAC_AX_BAND_NUM] = {
	R_AX_BCN_DROP_ALL0, R_AX_BCN_DROP_ALL0_C1
};

static u32 mbssid_drp_regl[MAC_AX_BAND_NUM] = {
	R_AX_BCN_DROP_ALL0_P0MB, R_AX_BCN_DROP_ALL0_P0MB_C1
};

static u32 bcnpsr_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCN_PSR_RPT_P0, R_AX_BCN_PSR_RPT_P1, R_AX_BCN_PSR_RPT_P2,
	 R_AX_BCN_PSR_RPT_P3, R_AX_BCN_PSR_RPT_P4},
	{R_AX_BCN_PSR_RPT_P0_C1, R_AX_BCN_PSR_RPT_P1_C1, R_AX_BCN_PSR_RPT_P2_C1,
	 R_AX_BCN_PSR_RPT_P3_C1, R_AX_BCN_PSR_RPT_P4_C1}
};

static u32 cnt_tmr_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCN_CNT_TMR_P0, R_AX_BCN_CNT_TMR_P1, R_AX_BCN_CNT_TMR_P2,
	 R_AX_BCN_CNT_TMR_P3, R_AX_BCN_CNT_TMR_P4},
	{R_AX_BCN_CNT_TMR_P0_C1, R_AX_BCN_CNT_TMR_P1_C1, R_AX_BCN_CNT_TMR_P2_C1,
	 R_AX_BCN_CNT_TMR_P3_C1, R_AX_BCN_CNT_TMR_P4_C1}
};

static u32 hiq_win_port_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_P0MB_HGQ_WINDOW_CFG_0, R_AX_PORT_HGQ_WINDOW_CFG,
	 R_AX_PORT_HGQ_WINDOW_CFG + 1, R_AX_PORT_HGQ_WINDOW_CFG + 2,
	 R_AX_PORT_HGQ_WINDOW_CFG + 3},
	{R_AX_P0MB_HGQ_WINDOW_CFG_0_C1, R_AX_PORT_HGQ_WINDOW_CFG_C1,
	 R_AX_PORT_HGQ_WINDOW_CFG_C1 + 1, R_AX_PORT_HGQ_WINDOW_CFG_C1 + 2,
	 R_AX_PORT_HGQ_WINDOW_CFG_C1 + 3}
};

static u32 hiq_win_mbid_regl[MAC_AX_BAND_NUM][MAC_AX_P0_MBID_MAX] = {
	{R_AX_P0MB_HGQ_WINDOW_CFG_0, R_AX_P0MB_HGQ_WINDOW_CFG_0 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_0 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_0 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_1, R_AX_P0MB_HGQ_WINDOW_CFG_1 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_1 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_1 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_2, R_AX_P0MB_HGQ_WINDOW_CFG_2 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_2 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_2 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_3, R_AX_P0MB_HGQ_WINDOW_CFG_3 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_3 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_3 + 3},
	{R_AX_P0MB_HGQ_WINDOW_CFG_0_C1, R_AX_P0MB_HGQ_WINDOW_CFG_0_C1 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_0_C1 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_0_C1 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_1_C1, R_AX_P0MB_HGQ_WINDOW_CFG_1_C1 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_1_C1 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_1_C1 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_2_C1, R_AX_P0MB_HGQ_WINDOW_CFG_2_C1 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_2_C1 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_2_C1 + 3,
	 R_AX_P0MB_HGQ_WINDOW_CFG_3_C1, R_AX_P0MB_HGQ_WINDOW_CFG_3_C1 + 1,
	 R_AX_P0MB_HGQ_WINDOW_CFG_3_C1 + 2, R_AX_P0MB_HGQ_WINDOW_CFG_3_C1 + 3}
};

static u32 dtim_prd_port_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_DTIM_CTRL_P0 + 1, R_AX_DTIM_CTRL_P1 + 1,
	 R_AX_DTIM_CTRL_P2 + 1, R_AX_DTIM_CTRL_P3 + 1,
	 R_AX_DTIM_CTRL_P4 + 1},
	{R_AX_DTIM_CTRL_P0_C1 + 1, R_AX_DTIM_CTRL_P1_C1 + 1,
	 R_AX_DTIM_CTRL_P2_C1 + 1, R_AX_DTIM_CTRL_P3_C1 + 1,
	 R_AX_DTIM_CTRL_P4_C1 + 1}
};

static u32 dtim_prd_mbid_regl[MAC_AX_BAND_NUM][MAC_AX_P0_MBID_MAX] = {
	{R_AX_DTIM_CTRL_P0 + 1, R_AX_DTIM_NUM0 + 1, R_AX_DTIM_NUM0 + 2,
	 R_AX_DTIM_NUM0 + 3, R_AX_DTIM_NUM1, R_AX_DTIM_NUM1 + 1,
	 R_AX_DTIM_NUM1 + 2, R_AX_DTIM_NUM1 + 3, R_AX_DTIM_NUM2,
	 R_AX_DTIM_NUM2 + 1, R_AX_DTIM_NUM2 + 2, R_AX_DTIM_NUM2 + 3,
	 R_AX_DTIM_NUM3, R_AX_DTIM_NUM3 + 1, R_AX_DTIM_NUM3 + 2,
	 R_AX_DTIM_NUM3 + 3},
	{R_AX_DTIM_CTRL_P0_C1 + 1, R_AX_DTIM_NUM0_C1 + 1, R_AX_DTIM_NUM0_C1 + 2,
	 R_AX_DTIM_NUM0_C1 + 3, R_AX_DTIM_NUM1_C1, R_AX_DTIM_NUM1_C1 + 1,
	 R_AX_DTIM_NUM1_C1 + 2, R_AX_DTIM_NUM1_C1 + 3, R_AX_DTIM_NUM2_C1,
	 R_AX_DTIM_NUM2_C1 + 1, R_AX_DTIM_NUM2_C1 + 2, R_AX_DTIM_NUM2_C1 + 3,
	 R_AX_DTIM_NUM3_C1, R_AX_DTIM_NUM3_C1 + 1, R_AX_DTIM_NUM3_C1 + 2,
	 R_AX_DTIM_NUM3_C1 + 3}
};

static u32 dtim_switch[MAC_AX_BAND_NUM] = {
	R_AX_MD_TSFT_STMP_CTL, R_AX_MD_TSFT_STMP_CTL_C1
};

static u32 hiq_nolmt_regl[MAC_AX_BAND_NUM] = {
	R_AX_EN_HGQ_NOLIMIT, R_AX_EN_HGQ_NOLIMIT_C1
};

static u32 mbid_ctrl_regl[MAC_AX_BAND_NUM] = {
	R_AX_MBSSID_CTRL, R_AX_MBSSID_CTRL_C1
};

static u32 bcn_early_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_BCNERLYINT_CFG_P0, R_AX_BCNERLYINT_CFG_P1,
	R_AX_BCNERLYINT_CFG_P2, R_AX_BCNERLYINT_CFG_P3,
	R_AX_BCNERLYINT_CFG_P4},
	{R_AX_BCNERLYINT_CFG_P0_C1, R_AX_BCNERLYINT_CFG_P1_C1,
	R_AX_BCNERLYINT_CFG_P2_C1, R_AX_BCNERLYINT_CFG_P3_C1,
	R_AX_BCNERLYINT_CFG_P4_C1}
};

static u32 tsfl_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_TSFTR_LOW_P0, R_AX_TSFTR_LOW_P1, R_AX_TSFTR_LOW_P2,
	 R_AX_TSFTR_LOW_P3, R_AX_TSFTR_LOW_P4},
	{R_AX_TSFTR_LOW_P0_C1, R_AX_TSFTR_LOW_P1_C1, R_AX_TSFTR_LOW_P2_C1,
	 R_AX_TSFTR_LOW_P3_C1, R_AX_TSFTR_LOW_P4_C1}
};

static u32 ptcl_dbg_regl[MAC_AX_BAND_NUM] = {
	R_AX_PTCL_DBG, R_AX_PTCL_DBG_C1
};

static u32 bss_color_regl[MAC_AX_BAND_NUM][MAC_AX_PORT_NUM] = {
	{R_AX_PTCL_BSS_COLOR_0,
	R_AX_PTCL_BSS_COLOR_0,
	R_AX_PTCL_BSS_COLOR_0,
	R_AX_PTCL_BSS_COLOR_0,
	R_AX_PTCL_BSS_COLOR_1},
	{R_AX_PTCL_BSS_COLOR_0_C1,
	R_AX_PTCL_BSS_COLOR_0_C1,
	R_AX_PTCL_BSS_COLOR_0_C1,
	R_AX_PTCL_BSS_COLOR_0_C1,
	R_AX_PTCL_BSS_COLOR_1_C1}
};

static u32 ptcl_dbg_info_regl[MAC_AX_BAND_NUM] = {
	R_AX_PTCL_DBG_INFO, R_AX_PTCL_DBG_INFO_C1
};

static u32 b_en_l[MAC_AX_PORT_NUM] = {
	B_AX_PORT_FUNC_EN_P0, B_AX_PORT_FUNC_EN_P1, B_AX_PORT_FUNC_EN_P2,
	B_AX_PORT_FUNC_EN_P3, B_AX_PORT_FUNC_EN_P4
};

static u32 b_txbcnrpt_l[MAC_AX_PORT_NUM] = {
	B_AX_TXBCN_RPT_EN_P0, B_AX_TXBCN_RPT_EN_P1, B_AX_TXBCN_RPT_EN_P2,
	B_AX_TXBCN_RPT_EN_P3, B_AX_TXBCN_RPT_EN_P4
};

static u32 b_rxbcnrpt_l[MAC_AX_PORT_NUM] = {
	B_AX_RXBCN_RPT_EN_P0, B_AX_RXBCN_RPT_EN_P1, B_AX_RXBCN_RPT_EN_P2,
	B_AX_RXBCN_RPT_EN_P3, B_AX_RXBCN_RPT_EN_P4
};

static u32 b_rxupd_l[MAC_AX_PORT_NUM] = {
	B_AX_RX_BSSID_FIT_EN_P0, B_AX_RX_BSSID_FIT_EN_P1,
	B_AX_RX_BSSID_FIT_EN_P2, B_AX_RX_BSSID_FIT_EN_P3,
	B_AX_RX_BSSID_FIT_EN_P4
};

static u32 b_rxtsfupd_l[MAC_AX_PORT_NUM] = {
	B_AX_TSF_UDT_EN_P0, B_AX_TSF_UDT_EN_P1, B_AX_TSF_UDT_EN_P2,
	B_AX_TSF_UDT_EN_P3, B_AX_TSF_UDT_EN_P4
};

static u32 b_bcntxen_l[MAC_AX_PORT_NUM] = {
	B_AX_BCNTX_EN_P0, B_AX_BCNTX_EN_P1, B_AX_BCNTX_EN_P2,
	B_AX_BCNTX_EN_P3, B_AX_BCNTX_EN_P4
};

static u32 b_phben_l[MAC_AX_PORT_NUM] = {
	B_AX_TBTT_PROHIB_EN_P0, B_AX_TBTT_PROHIB_EN_P1, B_AX_TBTT_PROHIB_EN_P2,
	B_AX_TBTT_PROHIB_EN_P3, B_AX_TBTT_PROHIB_EN_P4
};

static u32 b_shten_l[MAC_AX_PORT_NUM] = {
	B_AX_TBTT_UPD_SHIFT_SEL_P0, B_AX_TBTT_UPD_SHIFT_SEL_P1,
	B_AX_TBTT_UPD_SHIFT_SEL_P2, B_AX_TBTT_UPD_SHIFT_SEL_P3,
	B_AX_TBTT_UPD_SHIFT_SEL_P4
};

static u32 b_brken_l[MAC_AX_PORT_NUM] = {
	B_AX_BRK_SETUP_P0, B_AX_BRK_SETUP_P1, B_AX_BRK_SETUP_P2,
	B_AX_BRK_SETUP_P3, B_AX_BRK_SETUP_P4
};

static u32 b_rsttsf_l[MAC_AX_PORT_NUM] = {
	B_AX_TSFTR_RST_P0, B_AX_TSFTR_RST_P1, B_AX_TSFTR_RST_P2,
	B_AX_TSFTR_RST_P3, B_AX_TSFTR_RST_P4
};

static u32 b_drpall_l[MAC_AX_PORT_NUM] = {
	B_AX_BCN_DROP_ALL_P0, B_AX_BCN_DROP_ALL_P1, B_AX_BCN_DROP_ALL_P2,
	B_AX_BCN_DROP_ALL_P3, B_AX_BCN_DROP_ALL_P4
};

static u32 b_rptvld_l[MAC_AX_PORT_NUM] = {
	B_AX_RPT_VALID_P0, B_AX_RPT_VALID_P1, B_AX_RPT_VALID_P2,
	B_AX_RPT_VALID_P3, B_AX_RPT_VALID_P4
};

static u32 b_timerr_l[MAC_AX_PORT_NUM] = {
	B_AX_TIM_ILEGAL_P0, B_AX_TIM_ILEGAL_P1, B_AX_TIM_ILEGAL_P2,
	B_AX_TIM_ILEGAL_P3, B_AX_TIM_ILEGAL_P4
};

static u32 b_mbid_en_l[MAC_AX_P0_MBID_MAX - 1] = {
	B_AX_P0MB1_EN, B_AX_P0MB2_EN, B_AX_P0MB3_EN, B_AX_P0MB4_EN,
	B_AX_P0MB5_EN, B_AX_P0MB6_EN, B_AX_P0MB7_EN, B_AX_P0MB8_EN,
	B_AX_P0MB9_EN, B_AX_P0MB10_EN, B_AX_P0MB11_EN, B_AX_P0MB12_EN,
	B_AX_P0MB13_EN, B_AX_P0MB14_EN, B_AX_P0MB15_EN
};

static u32 b_mbid_drp_l[MAC_AX_P0_MBID_MAX - 1] = {
	B_AX_BCN_DROP_ALL_P0MB1, B_AX_BCN_DROP_ALL_P0MB2,
	B_AX_BCN_DROP_ALL_P0MB3, B_AX_BCN_DROP_ALL_P0MB4,
	B_AX_BCN_DROP_ALL_P0MB5, B_AX_BCN_DROP_ALL_P0MB6,
	B_AX_BCN_DROP_ALL_P0MB7, B_AX_BCN_DROP_ALL_P0MB8,
	B_AX_BCN_DROP_ALL_P0MB9, B_AX_BCN_DROP_ALL_P0MB10,
	B_AX_BCN_DROP_ALL_P0MB11, B_AX_BCN_DROP_ALL_P0MB12,
	B_AX_BCN_DROP_ALL_P0MB13, B_AX_BCN_DROP_ALL_P0MB14,
	B_AX_BCN_DROP_ALL_P0MB15
};

static u32 b_hiq_nolmt_mbid_l[MAC_AX_P0_MBID_MAX - 1] = {
	B_AX_HIQ_NO_LMT_EN_P0_VAP1, B_AX_HIQ_NO_LMT_EN_P0_VAP2,
	B_AX_HIQ_NO_LMT_EN_P0_VAP3, B_AX_HIQ_NO_LMT_EN_P0_VAP4,
	B_AX_HIQ_NO_LMT_EN_P0_VAP5, B_AX_HIQ_NO_LMT_EN_P0_VAP6,
	B_AX_HIQ_NO_LMT_EN_P0_VAP7, B_AX_HIQ_NO_LMT_EN_P0_VAP8,
	B_AX_HIQ_NO_LMT_EN_P0_VAP9, B_AX_HIQ_NO_LMT_EN_P0_VAP10,
	B_AX_HIQ_NO_LMT_EN_P0_VAP11, B_AX_HIQ_NO_LMT_EN_P0_VAP12,
	B_AX_HIQ_NO_LMT_EN_P0_VAP13, B_AX_HIQ_NO_LMT_EN_P0_VAP14,
	B_AX_HIQ_NO_LMT_EN_P0_VAP15
};

static u32 b_hiq_nolmt_port_l[MAC_AX_PORT_NUM] = {
	B_AX_HIQ_NO_LMT_EN_P0_ROOT, B_AX_HIQ_NO_LMT_EN_P1,
	B_AX_HIQ_NO_LMT_EN_P2, B_AX_HIQ_NO_LMT_EN_P3, B_AX_HIQ_NO_LMT_EN_P4
};

static u32 mbid_idx_l[MAC_AX_P0_MBID_MAX] = {
	MAC_AX_P0_ROOT, MAC_AX_P0_MBID1, MAC_AX_P0_MBID2, MAC_AX_P0_MBID3,
	MAC_AX_P0_MBID4, MAC_AX_P0_MBID5, MAC_AX_P0_MBID6, MAC_AX_P0_MBID7,
	MAC_AX_P0_MBID8, MAC_AX_P0_MBID9, MAC_AX_P0_MBID10, MAC_AX_P0_MBID11,
	MAC_AX_P0_MBID12, MAC_AX_P0_MBID13, MAC_AX_P0_MBID14, MAC_AX_P0_MBID15
};

static u32 t32_togl_rpt_size =
	sizeof(struct mac_ax_t32_togl_rpt) * MAC_AX_BAND_NUM * MAC_AX_PORT_NUM;
static u32 port_info_size =
	sizeof(struct mac_ax_port_info) * MAC_AX_BAND_NUM * MAC_AX_PORT_NUM;

static u32 _get_max_mbid(struct mac_ax_adapter *adapter, u8 *mbid_max)
{
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
		*mbid_max = MAC_AX_P0_MBID15;
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		*mbid_max = MAC_AX_P0_MBID3;
	else
		return MACCHIPID;

	return MACSUCCESS;
}

u32 get_bp_idx(u8 band, u8 port)
{
	return (band * MAC_AX_BAND_NUM + port);
}

u32 _get_port_cfg(struct mac_ax_adapter *adapter,
		  enum mac_ax_port_cfg_type type,
		  struct mac_ax_port_cfg_para *para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 mbssid_idx = para->mbssid_idx;
	u8 port = para->port;
	u8 band = para->band;
	u32 val32;
	u16 val16;
	u8 mbid_max;
	u32 ret;

	if (!is_curr_dbcc(adapter) && band != MAC_AX_BAND_0) {
		PLTFM_MSG_ERR("[ERR] invalid band idx %d\n", band);
		return MACFUNCINPUT;
	}

	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR] invalid port idx %d\n", port);
		return MACPORTCFGPORT;
	}

	if (mbssid_idx) {
		ret = _get_max_mbid(adapter, &mbid_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] get_max_mbid %d\n", ret);
			return MACPORTCFGPORT;
		}

		if (mbssid_idx > (u32)mbid_max) {
			PLTFM_MSG_ERR("[ERR] invalid mbssid %d\n", mbssid_idx);
			return MACFUNCINPUT;
		}
	}

	switch (type) {
	case MAC_AX_PCFG_NET_TYPE:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val32, B_AX_NET_TYPE_P0);
			break;
		case MAC_AX_PORT_1:
			para->val = GET_FIELD(val32, B_AX_NET_TYPE_P1);
			break;
		case MAC_AX_PORT_2:
			para->val = GET_FIELD(val32, B_AX_NET_TYPE_P2);
			break;
		case MAC_AX_PORT_3:
			para->val = GET_FIELD(val32, B_AX_NET_TYPE_P3);
			break;
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_NET_TYPE_P4);
			break;
		}
		break;

	case MAC_AX_PCFG_FUNC_SW:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		if (val32 & b_en_l[port])
			para->val = 1;
		else
			para->val = 0;
		break;

	case MAC_AX_PCFG_BCN_INTV:
		val32 = MAC_REG_R32(bcnspc_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = mbssid_idx ?
				    GET_FIELD(val32, B_AX_SUB_BCN_SPACE_P0) :
				    GET_FIELD(val32, B_AX_BCN_SPACE_P0);
			break;
		case MAC_AX_PORT_1:
			para->val = GET_FIELD(val32, B_AX_BCN_SPACE_P1);
			break;
		case MAC_AX_PORT_2:
			para->val = GET_FIELD(val32, B_AX_BCN_SPACE_P2);
			break;
		case MAC_AX_PORT_3:
			para->val = GET_FIELD(val32, B_AX_BCN_SPACE_P3);
			break;
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_BCN_SPACE_P4);
			break;
		}
		break;

	case MAC_AX_PCFG_BCN_HOLD_TIME:
		val32 = MAC_REG_R32(phb_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val32, B_AX_TBTT_HOLD_P0);
			break;
		case MAC_AX_PORT_1:
		case MAC_AX_PORT_2:
		case MAC_AX_PORT_3:
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_TBTT_HOLD_P1);
			break;
		}
		break;

	case MAC_AX_PCFG_BCN_ERLY:
		val32 = MAC_REG_R32(bcn_early_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val32, B_AX_BCNERLY_P0);
			break;
		case MAC_AX_PORT_1:
			para->val = GET_FIELD(val32, B_AX_BCNERLY_P1);
			break;
		case MAC_AX_PORT_2:
			para->val = GET_FIELD(val32, B_AX_BCNERLY_P2);
			break;
		case MAC_AX_PORT_3:
			para->val = GET_FIELD(val32, B_AX_BCNERLY_P3);
			break;
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_BCNERLY_P4);
			break;
		}
		break;

	case MAC_AX_PCFG_BCN_SETUP_TIME:
		val32 = MAC_REG_R32(phb_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val32, B_AX_TBTT_SETUP_P0);
			break;
		case MAC_AX_PORT_1:
		case MAC_AX_PORT_2:
		case MAC_AX_PORT_3:
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_TBTT_SETUP_P1);
			break;
		}
		break;

	case MAC_AX_PCFG_TBTT_ERLY:
		val16 = MAC_REG_R16(tbttery_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val16, B_AX_TBTTERLY_P0);
			break;
		case MAC_AX_PORT_1:
			para->val = GET_FIELD(val16, B_AX_TBTTERLY_P1);
			break;
		case MAC_AX_PORT_2:
			para->val = GET_FIELD(val16, B_AX_TBTTERLY_P2);
			break;
		case MAC_AX_PORT_3:
			para->val = GET_FIELD(val16, B_AX_TBTTERLY_P3);
			break;
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val16, B_AX_TBTTERLY_P4);
			break;
		}
		break;
	case MAC_AX_PCFG_BCN_MASK_AREA:
		val32 = MAC_REG_R32(mskarea_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			para->val = GET_FIELD(val32, B_AX_BCN_MSK_AREA_P0);
			break;
		case MAC_AX_PORT_1:
			para->val = GET_FIELD(val32, B_AX_BCN_MSK_AREA_P1);
			break;
		case MAC_AX_PORT_2:
			para->val = GET_FIELD(val32, B_AX_BCN_MSK_AREA_P2);
			break;
		case MAC_AX_PORT_3:
			para->val = GET_FIELD(val32, B_AX_BCN_MSK_AREA_P3);
			break;
		case MAC_AX_PORT_4:
			para->val = GET_FIELD(val32, B_AX_BCN_MSK_AREA_P4);
			break;
		}
		break;
	default:
		PLTFM_MSG_ERR("[ERR] invalid get cfg type %d\n", type);
		return MACPORTCFGTYPE;
	}

	return MACSUCCESS;
}

static u32 _port_cfg(struct mac_ax_adapter *adapter,
		     enum mac_ax_port_cfg_type type,
		     struct mac_ax_port_cfg_para *para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 set_val = para->val;
	u8 port = para->port;
	u8 band = para->band;
	u32 val32;
	u32 w_val32 = MAC_AX_R32_DEAD;

	if (!is_curr_dbcc(adapter) && band != MAC_AX_BAND_0) {
		PLTFM_MSG_ERR("[ERR] invalid band idx %d\n", band);
		return MACFUNCINPUT;
	}

	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR] invalid port idx %d\n", port);
		return MACPORTCFGPORT;
	}

	switch (type) {
	case MAC_AX_PCFG_NET_TYPE:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_NET_TYPE_P0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_NET_TYPE_P1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_NET_TYPE_P2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_NET_TYPE_P3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_NET_TYPE_P4);
			break;
		}
		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	default:
		PLTFM_MSG_ERR("[ERR] invalid cfg type %d\n", type);
		return MACPORTCFGTYPE;
	}

	return MACSUCCESS;
}

static u32 _bcn_setup_chk(struct mac_ax_adapter *adapter, u8 band, u8 port,
			  u32 *set_val)
{
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo_self, *pinfo_tmp;
	u32 ret;
	u32 up_lmt = 0xFFFFFFFF;
	u8 port_idx;

	pinfo_self = &adapter->port_info[get_bp_idx(band, port)];

	if (port == MAC_AX_PORT_0 && pinfo_self->stat == PORT_ST_DIS)
		return MACSUCCESS;

	if (*set_val > B_AX_TBTT_SETUP_P0_MSK) {
		PLTFM_MSG_ERR("[ERR] illegal bcn setup time %d\n", *set_val);
		return MACFUNCINPUT;
	}

	for (port_idx = MAC_AX_PORT_0; port_idx < MAC_AX_PORT_NUM; port_idx++) {
		pinfo_tmp = &adapter->port_info[get_bp_idx(band, port_idx)];
		if (pinfo_tmp->stat == PORT_ST_DIS)
			continue;
		if (port != MAC_AX_PORT_0 && port_idx == MAC_AX_PORT_0)
			continue;

		cfg_para.band = band;
		cfg_para.port = port_idx;
		cfg_para.mbssid_idx = 0;
		cfg_para.val = 0;
		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get port cfg bcn erly fail %d\n",
				      ret);
			return ret;
		}

		if (cfg_para.val < up_lmt)
			up_lmt = cfg_para.val;

		if (port == MAC_AX_PORT_0)
			break;
	}

	if (*set_val >= up_lmt) {
		PLTFM_MSG_WARN("[WARN]setup time %d over hw limit\n", *set_val);
		*set_val = up_lmt - 1;
		return MACSETVALERR;
	}

	return MACSUCCESS;
}

static u32 _bcn_mask_chk(struct mac_ax_adapter *adapter, u8 band, u8 port,
			 u32 *set_val)
{
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo;
	u32 ret;

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	if (pinfo->stat == PORT_ST_DIS)
		return MACSUCCESS;

	if (*set_val > B_AX_BCN_MSK_AREA_P0_MSK) {
		PLTFM_MSG_ERR("[ERR] illegal mask area %d\n", *set_val);
		return MACFUNCINPUT;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get port cfg bcn hold fail %d\n", ret);
		return ret;
	}
	if (*set_val > cfg_para.val) {
		PLTFM_MSG_WARN("[WARN]mask area %d over hw limit\n", *set_val);
		*set_val = cfg_para.val;
		return MACSETVALERR;
	}

	return MACSUCCESS;
}

static u32 _bcn_hold_chk(struct mac_ax_adapter *adapter, u8 band, u8 port,
			 u32 *set_val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo_self, *pinfo_tmp;
	u32 ret, bcn_erly, bcn_spc, val32;
	u8 port_idx;
	u32 low_lmt = 0, up_lmt = 0xFFFFFFFF;

	pinfo_self = &adapter->port_info[get_bp_idx(band, port)];

	if (port == MAC_AX_PORT_0 && pinfo_self->stat == PORT_ST_DIS)
		return MACSUCCESS;

	if (*set_val > B_AX_TBTT_HOLD_P0_MSK) {
		PLTFM_MSG_ERR("[ERR] illegal hold time %d\n", *set_val);
		return MACFUNCINPUT;
	}

	for (port_idx = MAC_AX_PORT_0; port_idx < MAC_AX_PORT_NUM; port_idx++) {
		pinfo_tmp = &adapter->port_info[get_bp_idx(band, port_idx)];
		if (pinfo_tmp->stat == PORT_ST_DIS)
			continue;
		if (port != MAC_AX_PORT_0 && port_idx == MAC_AX_PORT_0)
			continue;

		cfg_para.band = band;
		cfg_para.port = port_idx;
		cfg_para.mbssid_idx = 0;
		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_MASK_AREA,
				    &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get port cfg mask area fail %d\n",
				      ret);
			return ret;
		}

		if (cfg_para.val > low_lmt)
			low_lmt = cfg_para.val;

		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get port cfg bcn erly fail %d\n",
				      ret);
			return ret;
		}
		bcn_erly = cfg_para.val;

		if (port == MAC_AX_PORT_0 &&
		    GET_FIELD(MAC_REG_R32(mbid_ctrl_regl[band]), B_AX_P0MB_NUM))
			cfg_para.mbssid_idx = 1;
		else
			cfg_para.mbssid_idx = 0;
		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get port cfg bcn intv fail %d\n",
				      ret);
			return ret;
		}
		bcn_spc = cfg_para.val;

		val32 = bcn_spc * TU_TO_BCN_SET - bcn_erly;
		if (val32 < up_lmt)
			up_lmt = val32;

		if (port == MAC_AX_PORT_0)
			break;
	}

	if (*set_val < low_lmt) {
		PLTFM_MSG_WARN("[WARN]hold time %d below hw limit\n", *set_val);
		*set_val = low_lmt;
		return MACSETVALERR;
	}

	if (*set_val >= up_lmt) {
		PLTFM_MSG_WARN("[WARN]hold time %d over hw limit\n", *set_val);
		*set_val = up_lmt - 1;
		return MACSETVALERR;
	}

	return MACSUCCESS;
}

static u32 _bcn_erly_chk(struct mac_ax_adapter *adapter, u8 band, u8 port,
			 u32 *set_val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo;
	u32 ret, bcn_hold, bcn_spc, val32;

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	if (pinfo->stat == PORT_ST_DIS)
		return MACSUCCESS;

	if (*set_val > B_AX_BCNERLY_P0_MSK || !*set_val) {
		PLTFM_MSG_ERR("[ERR] illegal bcn erly %d\n", *set_val);
		return MACFUNCINPUT;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_SETUP_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get port cfg bcn setup fail %d\n", ret);
		return ret;
	}
	if (*set_val <= cfg_para.val) {
		PLTFM_MSG_WARN("[WARN]bcn erly %d below hw limit\n", *set_val);
		*set_val = cfg_para.val + 1;
		return MACSETVALERR;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_TBTT_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get port cfg tbtt erly fail %d\n", ret);
		return ret;
	}
	val32 = cfg_para.val / BCN_SET_TO_US;
	if (*set_val <= val32) {
		PLTFM_MSG_WARN("[WARN]bcn erly %d below hw limit\n", *set_val);
		*set_val = val32 + 1;
		return MACSETVALERR;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	if (port == MAC_AX_PORT_0 &&
	    GET_FIELD(MAC_REG_R32(mbid_ctrl_regl[band]), B_AX_P0MB_NUM))
		cfg_para.mbssid_idx = 1;
	else
		cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get port cfg bcn intv fail %d\n", ret);
		return ret;
	}
	bcn_spc = cfg_para.val;

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] get port cfg bcn hold fail %d\n", ret);
		return ret;
	}
	bcn_hold = cfg_para.val;

	val32 = bcn_spc * TU_TO_BCN_SET - bcn_hold;
	if (*set_val >= val32) {
		PLTFM_MSG_WARN("[WARN]bcn erly %d over hw limit\n", *set_val);
		*set_val = val32 - 1;
		return MACSETVALERR;
	}

	return MACSUCCESS;
}

static u32 _tbtt_erly_chk(struct mac_ax_adapter *adapter, u8 band, u8 port,
			  u32 *set_val)
{
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo;
	u32 ret, val32;

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	if (pinfo->stat == PORT_ST_DIS)
		return MACSUCCESS;

	if (*set_val > B_AX_TBTTERLY_P0_MSK) {
		PLTFM_MSG_ERR("[ERR] illegal tbtt erly %d\n", *set_val);
		return MACFUNCINPUT;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;
	cfg_para.val = 0;
	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get port cfg bcn erly fail %d\n", ret);
		return ret;
	}
	val32 = cfg_para.val * BCN_SET_TO_US;
	if (*set_val >= val32) {
		PLTFM_MSG_WARN("[WARN] tbtt erly %d over hw limit\n", *set_val);
		*set_val = val32 - 1;
		return MACSETVALERR;
	}

	return MACSUCCESS;
}

static u32 _set_max_mbid_num(struct mac_ax_adapter *adapter,
			     struct mac_ax_port_cfg_para *para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	u8 port = para->port;
	u8 band = para->band;
	u32 mbid_num = para->mbssid_idx;
	u32 ret;
	u32 bcn_erly;
	u32 hold_time;
	u32 subspc;
	u32 val32, w_val32;

	if (mbid_num && !(mbid_num % 2))
		mbid_num++;

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;

	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d get cfg bcn_erly fail %d\n",
			      band, port, ret);
		return ret;
	}
	bcn_erly = cfg_para.val;

	ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d get cfg hold_time fail %d\n",
			      band, port, ret);
		return ret;
	}
	hold_time = cfg_para.val;

	subspc = para->val / (mbid_num + 1);
	if (bcn_erly + hold_time > subspc * TU_TO_BCN_SET) {
		PLTFM_MSG_ERR("[ERR] BcnSubspc not enough for erly and hold time\n");
		para->val = (bcn_erly + hold_time) / TU_TO_BCN_SET * (mbid_num + 1);
		return MACSUBSPCERR;
	}

	//set interval
	cfg_para.val = para->val;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg bcn intv fail %d\n",
			      band, port, ret);
		return ret;
	}

	//set mbssid num
	val32 = MAC_REG_R32(mbid_ctrl_regl[band]);
	w_val32 = SET_CLR_WORD(val32, mbid_num, B_AX_P0MB_NUM);
	if (val32 != w_val32)
		MAC_REG_W32(mbid_ctrl_regl[band], w_val32);

	//set subspace
	val32 = MAC_REG_R32(bcnspc_regl[band][MAC_AX_PORT_0]);
	if (mbid_num)
		w_val32 = SET_CLR_WORD(val32, subspc, B_AX_SUB_BCN_SPACE_P0);
	else
		w_val32 = SET_CLR_WORD(val32, 0, B_AX_SUB_BCN_SPACE_P0);
	if (w_val32 != val32)
		MAC_REG_W32(bcnspc_regl[band][MAC_AX_PORT_0], w_val32);

	return MACSUCCESS;
}

static u32 port0_mbid_set(struct mac_ax_adapter *adapter, u8 band, u8 mbid_num,
			  u8 mbid_max)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, w_val32, i;

	val32 = MAC_REG_R32(mbid_ctrl_regl[band]);
	w_val32 = SET_CLR_WORD(val32, mbid_num, B_AX_P0MB_NUM);

	for (i = 0; i < mbid_max; i++) {
		if (i >= (MAC_AX_P0_MBID_MAX - 1)) {
			PLTFM_MSG_ERR("mbid en idx %d over array %d\n", i,
				      MAC_AX_P0_MBID_MAX - 1);
			return MACCMP;
		}
		w_val32 &= ~b_mbid_en_l[i];
	}

	if (w_val32 != val32)
		MAC_REG_W32(mbid_ctrl_regl[band], w_val32);

	return MACSUCCESS;
}

static u32 port0_subspc_set(struct mac_ax_adapter *adapter, u8 band,
			    u8 mbid_num, u32 *bcn_erly, u32 *hold_time)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 subspc_u32;
	u32 val32, w_val32;
	u32 ret = MACSUCCESS;

	val32 = MAC_REG_R32(bcnspc_regl[band][MAC_AX_PORT_0]);
	subspc_u32 = mbid_num ?
		     (GET_FIELD(val32, B_AX_BCN_SPACE_P0) / (mbid_num + 1)) : 0;
	if (subspc_u32 > B_AX_SUB_BCN_SPACE_P0_MSK) {
		subspc_u32 = B_AX_SUB_BCN_SPACE_P0_MSK;
		PLTFM_MSG_WARN("[WARN] sub space set to max %d\n", subspc_u32);
	}

	w_val32 = SET_CLR_WORD(val32, subspc_u32, B_AX_SUB_BCN_SPACE_P0);
	if (w_val32 != val32)
		MAC_REG_W32(bcnspc_regl[band][MAC_AX_PORT_0], w_val32);

	subspc_u32 *= TU_TO_BCN_SET;
	if (mbid_num && (BCN_HOLD_DEF + BCN_ERLY_DEF) > subspc_u32) {
		*bcn_erly = subspc_u32 * BCN_ERLY_RATIO / BCN_ITVL_RATIO;
		*hold_time = subspc_u32 * BCN_HOLD_RATIO / BCN_ITVL_RATIO;
	}

	return ret;
}

u32 dly_port_tu(struct mac_ax_adapter *adapter, u8 band, u8 port, u32 dly_tu)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 cnt;
	u32 ori_tsf;
	u32 dly_us = dly_tu * TU_UNIT;

	cnt = dly_tu * PORT_DLY_TU_CNT_LMT;
	ori_tsf = MAC_REG_R32(tsfl_regl[band][port]);
	PLTFM_DELAY_US(TU_UNIT);
	do {
		val32 = MAC_REG_R32(tsfl_regl[band][port]);
		if ((val32 >= ori_tsf &&
		     (val32 - ori_tsf) >= dly_us) ||
		    (val32 < ori_tsf &&
		     (ori_tsf - val32 + 1) >= dly_us))
			break;

		if (val32 == ori_tsf) {
			PLTFM_MSG_ERR("B%dP%d tsf not running 0x%X\n",
				      band, port, val32);
			return MACHWERR;
		}

		PLTFM_DELAY_US(TU_UNIT);
		cnt--;
	} while (cnt);

	if (!cnt)
		return MACPOLLTO;

	return MACSUCCESS;
}

u32 dly_port_us(struct mac_ax_adapter *adapter, u8 band, u8 port, u32 dly_us)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 cnt;
	u32 ori_tsf;

	cnt = dly_us * PORT_DLY_US_CNT_LMT;
	ori_tsf = MAC_REG_R32(tsfl_regl[band][port]);
	PLTFM_DELAY_US(10);
	do {
		val32 = MAC_REG_R32(tsfl_regl[band][port]);
		if ((val32 >= ori_tsf &&
		     (val32 - ori_tsf) >= dly_us) ||
		    (val32 < ori_tsf &&
		     (ori_tsf - val32 + 1) >= dly_us))
			break;

		if (val32 == ori_tsf) {
			PLTFM_MSG_ERR("B%dP%d tsf not running 0x%X\n",
				      band, port, val32);
			return MACHWERR;
		}

		PLTFM_DELAY_US(10);
		cnt--;
	} while (cnt);

	if (!cnt)
		return MACPOLLTO;

	return MACSUCCESS;
}

u32 rst_port_info(struct mac_ax_adapter *adapter, u8 band)
{
	u8 p_idx;

	for (p_idx = MAC_AX_PORT_0; p_idx < MAC_AX_PORT_NUM; p_idx++)
		PLTFM_MEMSET(&adapter->port_info[get_bp_idx(band, p_idx)], 0,
			     sizeof(struct mac_ax_port_info));

	return MACSUCCESS;
}

static u32 chk_bcnq_empty(struct mac_ax_adapter *adapter, u8 band, u8 port,
			  u32 bcn_spc, u8 *empty)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 bcnq_num_sh, bcnq_num_msk;
	u32 cnt, emp_cont_cnt;
	u32 ret;

	*empty = 1;

	val32 = MAC_REG_R32(ptcl_dbg_regl[band]);
	val32 = SET_CLR_WORD(val32, PTCL_DBG_BCNQ_NUM0, B_AX_PTCL_DBG_SEL);
	MAC_REG_W32(ptcl_dbg_regl[band], val32);
	PLTFM_DELAY_US(PTCL_DBG_DLY_US);

	cnt = CHK_BCNQ_CNT * bcn_spc;
	emp_cont_cnt = 0;
	do {
		val32 = MAC_REG_R32(ptcl_dbg_info_regl[band]);
		bcnq_num_sh = port * 2;
		bcnq_num_msk = 3;
		val32 = GET_FIEL2(val32, bcnq_num_sh, bcnq_num_msk);

		if (val32)
			emp_cont_cnt = 0;
		else
			emp_cont_cnt++;

		if (emp_cont_cnt >= BCNQ_EMP_CONT_CNT) {
			if (port == MAC_AX_PORT_0)
				break;
			return MACSUCCESS;
		}

		ret = dly_port_tu(adapter, band, port, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] dly B%dP%d %d tu fail %d\n",
				      band, port, 1, ret);
			return ret;
		}
		cnt--;
	} while (cnt);

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR] chk bcnq empty0 %d timeout\n", val32);
		*empty = 0;
		return MACPOLLTO;
	}

	val32 = MAC_REG_R32(ptcl_dbg_regl[band]);
	val32 = SET_CLR_WORD(val32, PTCL_DBG_BCNQ_NUM1, B_AX_PTCL_DBG_SEL);
	MAC_REG_W32(ptcl_dbg_regl[band], val32);
	PLTFM_DELAY_US(PTCL_DBG_DLY_US);

	cnt = CHK_BCNQ_CNT * bcn_spc;
	emp_cont_cnt = 0;
	do {
		val32 = MAC_REG_R32(ptcl_dbg_info_regl[band]);

		if (val32)
			emp_cont_cnt = 0;
		else
			emp_cont_cnt++;

		if (emp_cont_cnt >= BCNQ_EMP_CONT_CNT)
			return MACSUCCESS;

		ret = dly_port_tu(adapter, band, port, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] dly B%dP%d %d tu fail %d\n",
				      band, port, 1, ret);
			return ret;
		}
		cnt--;
	} while (cnt);

	PLTFM_MSG_ERR("[ERR] chk bcnq empty1 %d timeout\n", val32);
	*empty = 0;
	return MACPOLLTO;
}

static u32 fast_bcn_drop(struct mac_ax_adapter *adapter, u8 band, u8 port,
			 struct mac_ax_port_info *pinfo)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	u32 ret = MACSUCCESS;
	u32 port_drp_sel;
	u32 mbid_drp_sel = 0;
	u32 val32;
	u32 bcn_spc;
	u8 mbid_num;
	u8 is_empty;
	u32 i;

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;

	port_drp_sel = (port == MAC_AX_PORT_0 ? B_AX_BCN_DROP_ALL_P0 : 0) |
		       (port == MAC_AX_PORT_1 ? B_AX_BCN_DROP_ALL_P1 : 0) |
		       (port == MAC_AX_PORT_2 ? B_AX_BCN_DROP_ALL_P2 : 0) |
		       (port == MAC_AX_PORT_3 ? B_AX_BCN_DROP_ALL_P3 : 0) |
		       (port == MAC_AX_PORT_4 ? B_AX_BCN_DROP_ALL_P4 : 0);
	val32 = MAC_REG_R32(bcndrp_regl[band]) | port_drp_sel;
	MAC_REG_W32(bcndrp_regl[band], val32);

	if (port == MAC_AX_PORT_0 && pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) {
		mbid_drp_sel = B_AX_BCN_DROP_ALL_P0MB1 |
			       B_AX_BCN_DROP_ALL_P0MB2 |
			       B_AX_BCN_DROP_ALL_P0MB3;
		if (!(is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		      is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)))
			mbid_drp_sel |= B_AX_BCN_DROP_ALL_P0MB4 |
					B_AX_BCN_DROP_ALL_P0MB5 |
					B_AX_BCN_DROP_ALL_P0MB6 |
					B_AX_BCN_DROP_ALL_P0MB7 |
					B_AX_BCN_DROP_ALL_P0MB8 |
					B_AX_BCN_DROP_ALL_P0MB9 |
					B_AX_BCN_DROP_ALL_P0MB10 |
					B_AX_BCN_DROP_ALL_P0MB11 |
					B_AX_BCN_DROP_ALL_P0MB12 |
					B_AX_BCN_DROP_ALL_P0MB13 |
					B_AX_BCN_DROP_ALL_P0MB14 |
					B_AX_BCN_DROP_ALL_P0MB15;
		val32 = MAC_REG_R32(mbssid_drp_regl[band]) | mbid_drp_sel;
		MAC_REG_W32(mbssid_drp_regl[band], val32);
	}

	cfg_para.val = 1;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_SETUP_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg func setup time fail %d\n", ret);
		return ret;
	}

	cfg_para.val = 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_MASK_AREA, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg func mask area fail %d\n", ret);
		return ret;
	}

	cfg_para.val = 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg func hold time fail %d\n", ret);
		return ret;
	}

	cfg_para.val = 2;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg func bcn early fail %d\n", ret);
		return ret;
	}

	cfg_para.val = 1;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TBTT_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg tbtt early fail %d\n", ret);
		return ret;
	}

	if (port == MAC_AX_PORT_0 && pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) {
		ret = _get_max_mbid(adapter, &mbid_num);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] get_max_mbid %d\n", ret);
			return MACPORTCFGPORT;
		}
		bcn_spc = (u32)(mbid_num + 1) * BCN_FAST_DRP_TBTT;

		val32 = MAC_REG_R32(bcnspc_regl[band][MAC_AX_PORT_0]);
		val32 = SET_CLR_WORD(val32, BCN_FAST_DRP_TBTT,
				     B_AX_SUB_BCN_SPACE_P0);
		MAC_REG_W32(bcnspc_regl[band][MAC_AX_PORT_0], val32);
		val32 = SET_CLR_WORD(val32, bcn_spc, B_AX_BCN_SPACE_P0);
		MAC_REG_W32(bcnspc_regl[band][MAC_AX_PORT_0], val32);

		val32 = MAC_REG_R32(mbid_ctrl_regl[band]);
		val32 = SET_CLR_WORD(val32, mbid_num, B_AX_P0MB_NUM);
		for (i = 0; i < mbid_num; i++)
			val32 |= b_mbid_en_l[i];
		MAC_REG_W32(mbid_ctrl_regl[band], val32);

	} else {
		bcn_spc = BCN_FAST_DRP_TBTT;
		cfg_para.val = bcn_spc;
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] port cfg func bcn intv fail %d\n",
				      ret);
			return ret;
		}
	}

	cfg_para.val = 1;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TX_SW, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] port cfg tx sw fail %d\n", ret);
		return ret;
	}

	ret = chk_bcnq_empty(adapter, band, port, bcn_spc, &is_empty);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk bcnq empty fail %d\n", ret);
		return ret;
	}

	val32 = MAC_REG_R32(bcndrp_regl[band]) & ~port_drp_sel;
	MAC_REG_W32(bcndrp_regl[band], val32);

	if (port == MAC_AX_PORT_0 && pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) {
		val32 = MAC_REG_R32(mbssid_drp_regl[band]) & ~mbid_drp_sel;
		MAC_REG_W32(mbssid_drp_regl[band], val32);
	}

	val32 = MAC_REG_R32(cfg_regl[band][port]) & ~b_phben_l[port];
	MAC_REG_W32(cfg_regl[band][port], val32);

	ret = dly_port_tu(adapter, band, port, bcn_spc + 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d dly %d tu fail %d\n",
			      band, port, bcn_spc + 1, ret);
		return ret;
	}

	return MACSUCCESS;
}

u32 _patch_port_dis_flow(struct mac_ax_adapter *adapter, u8 band, u8 port,
			 struct mac_ax_port_info *pinfo)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	u32 bcn_set_bk = MAC_AX_R32_DEAD;
	u32 bcn_spc;
	u32 val32;
	u32 ret = MACSUCCESS;
	u16 val16;
	u8 patch_flag, phb_bkp_flag;
	u8 mbid_max;

	patch_flag = chk_patch_port_dis_flow(adapter) ? 1 : 0;
	phb_bkp_flag = patch_flag && port != MAC_AX_PORT_0 ? 1 : 0;

	if (phb_bkp_flag)
		bcn_set_bk = MAC_REG_R32(phb_regl[band][port]);

	if (pinfo->stat == PORT_ST_AP || pinfo->stat == PORT_ST_ADHOC) {
		ret = fast_bcn_drop(adapter, band, port, pinfo);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] fast bcn drop fail %d\n",
				      ret);
			goto end;
		}
	}

	if (patch_flag) {
		cfg_para.band = band;
		cfg_para.port = port;
		cfg_para.mbssid_idx = 0;
		cfg_para.val = 0;
		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] get port cfg bcn intv fail %d\n",
				      ret);
			goto end;
		}
		bcn_spc = cfg_para.val;

		val32 = MAC_REG_R32(phb_regl[band][port]);
		val32 = SET_CLR_WORD(val32, 0, B_AX_TBTT_SETUP_P0);
		val32 = SET_CLR_WORD(val32, 1, B_AX_TBTT_HOLD_P0);
		MAC_REG_W32(phb_regl[band][port], val32);

		val16 = MAC_REG_R16(tbttery_regl[band][port]);
		val16 = SET_CLR_WORD(val16, 0, B_AX_TBTTERLY_P0);
		MAC_REG_W16(tbttery_regl[band][port], val16);

		val16 = MAC_REG_R16(bcn_early_regl[band][port]);
		val16 = SET_CLR_WORD(val16, 0, B_AX_BCNERLY_P0);
		MAC_REG_W16(bcn_early_regl[band][port], val16);

		ret = dly_port_tu(adapter, band, port, bcn_spc + 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d dly %d tu fail %d\n",
				      band, port, bcn_spc, ret);
			goto end;
		}
	}

	if (port == MAC_AX_PORT_0 && pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) {
		ret = _get_max_mbid(adapter, &mbid_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] get_max_mbid %d\n", ret);
			return MACPORTCFGPORT;
		}

		ret = port0_mbid_set(adapter, band, 0, mbid_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d mbid set fail %d\n",
				      band, port, ret);
			return ret;
		}
	}

	val32 = MAC_REG_R32(cfg_regl[band][port]) &
		~(b_en_l[port] | b_brken_l[port]);
	MAC_REG_W32(cfg_regl[band][port], val32);
	val32 |= b_rsttsf_l[port];
	MAC_REG_W32(cfg_regl[band][port], val32);
	MAC_REG_W32(cnt_tmr_regl[band][port], 0);

	pinfo->stat = PORT_ST_DIS;
end:
	if (phb_bkp_flag) {
		if (bcn_set_bk != MAC_AX_R32_DEAD) {
			MAC_REG_W32(phb_regl[band][port], bcn_set_bk);
		} else {
			PLTFM_MSG_ERR("[ERR]prev ret %d\n", ret);
			PLTFM_MSG_ERR("[ERR]B%dP%d phb reg dead\n", band, port);
			ret = MACCMP;
		}
	}

	return ret;
}

u32 _patch_tbtt_shift_setval(struct mac_ax_adapter *adapter, u32 bcnspc,
			     u32 *shift_val)
{
	if (!chk_patch_tbtt_shift_setval(adapter))
		return MACSUCCESS;

	if (!*shift_val)
		return MACSUCCESS;

	if (*shift_val >= bcnspc)
		return MACCMP;

	*shift_val = (bcnspc - *shift_val) | TBTT_SHIFT_OFST_MSB;

	return MACSUCCESS;
}

u32 mac_port_cfg(struct mac_ax_adapter *adapter,
		 enum mac_ax_port_cfg_type type,
		 struct mac_ax_port_cfg_para *para)
{
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_port_info *pinfo;
	u8 band = para->band;
	u8 port = para->port;

#if 0 // MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
#if MAC_AX_PHL_H2C
		struct rtw_h2c_pkt *h2cb;
#else
		struct h2c_buf *h2cb;
#endif
		struct fwcmd_port_cfg *hdr;
		u32 ret = MACSUCCESS;
		u32 cnt;

		if (para->band >= MAC_AX_BAND_NUM) {
			PLTFM_MSG_ERR("[ERR]invalid band %d\n", para->band);
			return MACFUNCINPUT;
		}

		if (para->port >= MAC_AX_PORT_NUM) {
			PLTFM_MSG_ERR("[ERR]invalid port %d\n", para->port);
			return MACFUNCINPUT;
		}

		pinfo = &adapter->port_info[get_bp_idx(band, port)];

		cnt = PORT_H2C_DLY_CNT;
		while (pinfo->h2c_sm == MAC_AX_PORT_H2C_BUSY) {
			cnt--;
			if (!cnt) {
				PLTFM_MSG_ERR("[ERR]B%dP%d port H2C busy\n",
					      band, port);
				return MACPROCBUSY;
			}
			PLTFM_DELAY_US(PORT_H2C_DLY_US);
		}

		if (pinfo->h2c_sm == MAC_AX_PORT_H2C_FAIL) {
			PLTFM_MSG_ERR("[ERR]prev B%dP%d port H2C fail\n",
				      band, port);
			return MACPROCERR;
		}

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
		if (!h2cb)
			return MACNPTR;

		hdr = (struct fwcmd_port_cfg *)
		      h2cb_put(h2cb, sizeof(struct fwcmd_port_cfg));
		if (!hdr) {
			ret = MACNOBUF;
			goto fail;
		}

		hdr->dword0 =
			cpu_to_le32(SET_WORD(type, FWCMD_H2C_PORT_CFG_TYPE) |
				    SET_WORD(para->mbssid_idx,
					     FWCMD_H2C_PORT_CFG_MBSSID_IDX) |
				    SET_WORD(port, FWCMD_H2C_PORT_CFG_PORT) |
				    (band ? FWCMD_H2C_PORT_CFG_BAND : 0));

		hdr->dword1 =
			cpu_to_le32(SET_WORD(para->val, FWCMD_H2C_PORT_CFG_VAL));

		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_MPORT,
				      FWCMD_H2C_FUNC_PORT_CFG,
				      0,
				      0);
		if (ret)
			goto fail;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret)
			goto fail;

#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
		if (ret)
			goto fail;

		h2cb_free(adapter, h2cb);

		h2c_end_flow(adapter);

		pinfo->h2c_sm = MAC_AX_PORT_H2C_BUSY;

		return MACSUCCESS;
fail:
		h2cb_free(adapter, h2cb);

		return ret;
	}
#endif

	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 mbssid_idx = para->mbssid_idx;
	u32 set_val = para->val;
	u8 val8, w_val8;
	u16 val16;
	u16 w_val16 = MAC_AX_R16_DEAD;
	u32 val32;
	u32 w_val32 = MAC_AX_R32_DEAD;
	struct mac_ax_pkt_drop_info info;
	u8 mbid_max;
	u8 i = 0, j = 0;
	u32 ret = MACSUCCESS;

	if (!is_curr_dbcc(adapter) && band != MAC_AX_BAND_0) {
		PLTFM_MSG_ERR("[ERR] invalid band idx %d\n", band);
		return MACFUNCINPUT;
	}

	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR] invalid port idx %d\n", port);
		return MACPORTCFGPORT;
	}

	ret = _get_max_mbid(adapter, &mbid_max);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] get_max_mbid %d\n", ret);
		return MACPORTCFGPORT;
	}

	if (mbssid_idx) {
		if (mbssid_idx > (u32)mbid_max) {
			PLTFM_MSG_ERR("[ERR] invalid MBSSID number %d\n",
				      mbssid_idx);
			return MACFUNCINPUT;
		}
	}

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	switch (type) {
	case MAC_AX_PCFG_FUNC_SW:
		if (set_val) {
			PLTFM_MSG_ERR("[ERR] use port init to enable port\n");
			return MACFUNCINPUT;
		}

		if (pinfo->stat == PORT_ST_DIS) {
			PLTFM_MSG_WARN("[WARN]B%dP%d stat already disable\n",
				       band, port);
			break;
		}

		ret = _patch_port_dis_flow(adapter, band, port, pinfo);

		if (ret == MACSUCCESS) {
			j = (port == 0 && pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) ?
			     mbid_max : 0;
			for (i = 0; i <= j; i++) {
				info.band = band;
				info.port = port;
				info.mbssid = (u8)mbid_idx_l[i];
				info.sel = info.mbssid ?
					   MAC_AX_PKT_DROP_SEL_HIQ_MBSSID :
					   MAC_AX_PKT_DROP_SEL_HIQ_PORT;
				ret = adapter->ops->pkt_drop(adapter, &info);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("[ERR]B%dP%d mbid%d hiq drop %d\n",
						      band, port, info.mbssid, ret);
					break;
				}
			}
		}
		pinfo->mbssid_en_stat = MAC_AX_MBSSID_INIT;
		break;

	case MAC_AX_PCFG_TX_SW:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_bcntxen_l[port] :
				    val32 & ~b_bcntxen_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_TX_RPT:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_txbcnrpt_l[port] :
				    val32 & ~b_txbcnrpt_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_RX_SW:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_rxupd_l[port] :
				    val32 & ~b_rxupd_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_RX_RPT:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_rxbcnrpt_l[port] :
				    val32 & ~b_rxbcnrpt_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_RX_SYNC:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_rxtsfupd_l[port] :
				    val32 & ~b_rxtsfupd_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_BCN_PRCT:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		w_val32 = set_val ? val32 | b_phben_l[port] | b_brken_l[port] :
				    val32 & ~(b_phben_l[port] |
					      b_brken_l[port]);
		if (w_val32 != val32)
			MAC_REG_W32(cfg_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_TBTT_AGG:
		if (set_val > B_AX_TBTT_AGG_NUM_P0_MSK) {
			PLTFM_MSG_ERR("[ERR] illegal tbtt agg %d\n", set_val);
			return MACFUNCINPUT;
		}

		val16 = MAC_REG_R16(tbttagg_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTT_AGG_NUM_P0);
			break;
		case MAC_AX_PORT_1:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTT_AGG_NUM_P1);
			break;
		case MAC_AX_PORT_2:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTT_AGG_NUM_P2);
			break;
		case MAC_AX_PORT_3:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTT_AGG_NUM_P3);
			break;
		case MAC_AX_PORT_4:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTT_AGG_NUM_P4);
			break;
		}
		if (w_val16 != val16 && w_val16 != MAC_AX_R16_DEAD)
			MAC_REG_W16(tbttagg_regl[band][port], w_val16);
		break;

	case MAC_AX_PCFG_TBTT_SHIFT:
		if (set_val > B_AX_TBTT_SHIFT_OFST_P0_MSK) {
			PLTFM_MSG_ERR("[ERR] illegal tbtt shift %d\n", set_val);
			return MACFUNCINPUT;
		}

		cfg_para.band = band;
		cfg_para.port = port;
		cfg_para.mbssid_idx = 0;
		cfg_para.val = 0;
		ret = _get_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get port cfg bcn intv fail %d\n", ret);
			return ret;
		}
		if (set_val >= cfg_para.val) {
			PLTFM_MSG_ERR("tbtt shift %d over bcn spc %d\n",
				      set_val, cfg_para.val);
			return MACCMP;
		}

		ret = _patch_tbtt_shift_setval(adapter, cfg_para.val, &set_val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("patch tbtt shift fail %d\n", ret);
			return ret;
		}

		val32 = MAC_REG_R32(tbttsht_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SHIFT_OFST_P0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SHIFT_OFST_P1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SHIFT_OFST_P2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SHIFT_OFST_P3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SHIFT_OFST_P4);
			break;
		}
		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(tbttsht_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_RST_TSF:
		val32 = MAC_REG_R32(cfg_regl[band][port]);
		val32 = val32 | b_rsttsf_l[port];
		MAC_REG_W32(cfg_regl[band][port], val32);
		break;

	case MAC_AX_PCFG_RST_TPR:
		val32 = MAC_REG_R32(bcnpsr_regl[band][port]);
		val32 |= b_rptvld_l[port];
		MAC_REG_W32(bcnpsr_regl[band][port], val32);
		break;

	case MAC_AX_PCFG_HIQ_WIN:
		if (set_val > B_AX_HGQWND_0_MSK) {
			PLTFM_MSG_ERR("[ERR] illegal hiq win %d\n", set_val);
			return MACFUNCINPUT;
		}

		if (port == MAC_AX_PORT_0)
			MAC_REG_W8(hiq_win_mbid_regl[band][mbssid_idx],
				   (u8)set_val);
		else
			MAC_REG_W8(hiq_win_port_regl[band][port],
				   (u8)set_val);
		break;

	case MAC_AX_PCFG_HIQ_DTIM:
		if (set_val > B_AX_DTIM_NUM_P0_MSK) {
			PLTFM_MSG_ERR("[ERR] illegal dtim prd %d\n", set_val);
			return MACFUNCINPUT;
		}

		val8 = MAC_REG_R8(dtim_switch[band]);
		w_val8 = val8 | B_AX_UPD_TIMIE | B_AX_UPD_HGQMD;
		if (w_val8 != val8)
			MAC_REG_W8(dtim_switch[band], w_val8);

		if (port == MAC_AX_PORT_0)
			MAC_REG_W8(dtim_prd_mbid_regl[band][mbssid_idx],
				   (u8)set_val);
		else
			MAC_REG_W8(dtim_prd_port_regl[band][port],
				   (u8)set_val);
		break;

	case MAC_AX_PCFG_HIQ_NOLIMIT:
		val32 = MAC_REG_R32(hiq_nolmt_regl[band]);
		if (port == MAC_AX_PORT_0 && mbssid_idx)
			w_val32 = set_val ?
				  val32 | b_hiq_nolmt_mbid_l[mbssid_idx - 1] :
				  val32 & ~b_hiq_nolmt_mbid_l[mbssid_idx - 1];
		else
			w_val32 = set_val ?
				  val32 | b_hiq_nolmt_port_l[port] :
				  val32 & ~b_hiq_nolmt_port_l[port];
		if (w_val32 != val32)
			MAC_REG_W32(hiq_nolmt_regl[band], w_val32);
		break;

	case MAC_AX_PCFG_BCN_SETUP_TIME:
		ret = _bcn_setup_chk(adapter, band, port, &para->val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_WARN("[WARN]bcn setup chk %d\n", ret);
			return ret;
		}

		val32 = MAC_REG_R32(phb_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SETUP_P0);
			break;
		case MAC_AX_PORT_1:
		case MAC_AX_PORT_2:
		case MAC_AX_PORT_3:
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_SETUP_P1);
			break;
		}
		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(phb_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_BCN_HOLD_TIME:
		ret = _bcn_hold_chk(adapter, band, port, &para->val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_WARN("[WARN]bcn hold chk %d\n", ret);
			return ret;
		}

		val32 = MAC_REG_R32(phb_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_HOLD_P0);
			break;
		case MAC_AX_PORT_1:
		case MAC_AX_PORT_2:
		case MAC_AX_PORT_3:
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_TBTT_HOLD_P1);
			break;
		}

		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(phb_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_MBSSID_EN:
		if (port != MAC_AX_PORT_0 || !mbssid_idx) {
			PLTFM_MSG_ERR("[ERR] mbssid en invalid for P%d MB%d\n",
				      port, mbssid_idx);
			return MACFUNCINPUT;
		}

		val32 = MAC_REG_R32(mbid_ctrl_regl[band]);
		if (mbssid_idx > GET_FIELD(val32, B_AX_P0MB_NUM)) {
			PLTFM_MSG_ERR("[ERR] mbssid %d over %d\n", set_val,
				      GET_FIELD(val32, B_AX_P0MB_NUM));
			return MACFUNCINPUT;
		}

		w_val32 = set_val ? val32 | b_mbid_en_l[mbssid_idx - 1] :
				    val32 & ~b_mbid_en_l[mbssid_idx - 1];

		if (w_val32 != val32)
			MAC_REG_W32(mbid_ctrl_regl[band], w_val32);

		info.band = band;
		info.port = port;
		info.mbssid = (u8)mbssid_idx;
		info.sel = set_val ? MAC_AX_PKT_DROP_SEL_REL_HIQ_MBSSID :
				     MAC_AX_PKT_DROP_SEL_HIQ_MBSSID;
		ret = adapter->ops->pkt_drop(adapter, &info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] B%d P%d MB%d hiq drop %d\n",
				      band, port, mbssid_idx, ret);
			return ret;
		}
		pinfo->mbssid_en_stat = MAC_AX_MBSSID_ENABLED;
		break;

	case MAC_AX_PCFG_BCN_ERLY:
		ret = _bcn_erly_chk(adapter, band, port, &para->val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_WARN("[WARN]bcn erly chk %d\n", ret);
			return ret;
		}

		val32 = MAC_REG_R32(bcn_early_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val, B_AX_BCNERLY_P0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val, B_AX_BCNERLY_P1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val, B_AX_BCNERLY_P2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val, B_AX_BCNERLY_P3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val, B_AX_BCNERLY_P4);
			break;
		}

		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(bcn_early_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_BCN_MASK_AREA:
		ret = _bcn_mask_chk(adapter, band, port, &para->val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_WARN("[WARN]bcn mask chk %d\n", ret);
			return ret;
		}

		val32 = MAC_REG_R32(mskarea_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_MSK_AREA_P0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_MSK_AREA_P1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_MSK_AREA_P2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_MSK_AREA_P3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_MSK_AREA_P4);
			break;
		}

		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(mskarea_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_TBTT_ERLY:
		ret = _tbtt_erly_chk(adapter, band, port, &para->val);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_WARN("[WARN]tbtt erly chk %d\n", ret);
			return ret;
		}

		val16 = MAC_REG_R16(tbttery_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTTERLY_P0);
			break;
		case MAC_AX_PORT_1:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTTERLY_P1);
			break;
		case MAC_AX_PORT_2:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTTERLY_P2);
			break;
		case MAC_AX_PORT_3:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTTERLY_P3);
			break;
		case MAC_AX_PORT_4:
			w_val16 = SET_CLR_WORD(val16, set_val,
					       B_AX_TBTTERLY_P4);
			break;
		}

		if (w_val16 != val16 && w_val16 != MAC_AX_R16_DEAD)
			MAC_REG_W16(tbttery_regl[band][port], w_val16);
		break;

	case MAC_AX_PCFG_BSS_CLR:
		if (set_val > B_AX_BSS_COLOB_AX_PORT_0_MSK) {
			PLTFM_MSG_ERR("[ERR] illegal bss color %d\n", set_val);
			return MACFUNCINPUT;
		}

		val32 = MAC_REG_R32(bss_color_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BSS_COLOB_AX_PORT_0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BSS_COLOB_AX_PORT_1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BSS_COLOB_AX_PORT_2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BSS_COLOB_AX_PORT_3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BSS_COLOB_AX_PORT_4);
			break;
		}

		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(bss_color_regl[band][port], w_val32);
		break;

	case MAC_AX_PCFG_BCN_DRP_ALL:
		if (port == MAC_AX_PORT_0 && mbssid_idx &&
		    pinfo->mbssid_en_stat == MAC_AX_MBSSID_ENABLED) {
			val32 = MAC_REG_R32(mbssid_drp_regl[band]);
			w_val32 = set_val ?
				  val32 | b_mbid_drp_l[mbssid_idx - 1] :
				  val32 & ~b_mbid_drp_l[mbssid_idx - 1];
			if (w_val32 != val32)
				MAC_REG_W32(mbssid_drp_regl[band], w_val32);
		} else {
			val32 = MAC_REG_R32(bcndrp_regl[band]);
			w_val32 = set_val ? val32 | b_drpall_l[port] :
					    val32 & ~b_drpall_l[port];
			if (w_val32 != val32)
				MAC_REG_W32(bcndrp_regl[band], w_val32);
		}
		break;

	case MAC_AX_PCFG_MBSSID_NUM:
		if (port != MAC_AX_PORT_0 || pinfo->stat != PORT_ST_AP) {
			PLTFM_MSG_ERR("[ERR]port is not 0 or is not AP\n");
			return MACPORTCFGPORT;
		}
		_set_max_mbid_num(adapter, para);
		break;

	case MAC_AX_PCFG_BCN_INTV:
		if (set_val > B_AX_BCN_SPACE_P0_MSK || !set_val) {
			PLTFM_MSG_ERR("[ERR] illegal bcn itvl %d\n", set_val);
			return MACFUNCINPUT;
		}

		val32 = MAC_REG_R32(bcnspc_regl[band][port]);
		switch (port) {
		case MAC_AX_PORT_0:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_SPACE_P0);
			break;
		case MAC_AX_PORT_1:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_SPACE_P1);
			break;
		case MAC_AX_PORT_2:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_SPACE_P2);
			break;
		case MAC_AX_PORT_3:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_SPACE_P3);
			break;
		case MAC_AX_PORT_4:
			w_val32 = SET_CLR_WORD(val32, set_val,
					       B_AX_BCN_SPACE_P4);
			break;
		}
		if (w_val32 != val32 && val32 != MAC_AX_R32_DEAD)
			MAC_REG_W32(bcnspc_regl[band][port], w_val32);
		break;

	default:
		PLTFM_MSG_ERR("[ERR] invalid cfg type %d\n", type);
		return MACPORTCFGTYPE;
	}

	return ret;
}

u32 mac_port_init(struct mac_ax_adapter *adapter,
		  struct mac_ax_port_init_para *para)
{
	struct mac_ax_port_info *pinfo;
	u8 band = para->band_idx;
	u8 port = para->port_idx;
	u8 net_type = para->net_type;

#if 0 // MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
#if MAC_AX_PHL_H2C
		struct rtw_h2c_pkt *h2cb;
#else
		struct h2c_buf *h2cb;
#endif
		struct fwcmd_port_init *hdr;
		u32 ret = MACSUCCESS;
		u32 cnt;

		if (band >= MAC_AX_BAND_NUM) {
			PLTFM_MSG_ERR("[ERR]invalid band %d\n", band);
			return MACFUNCINPUT;
		}

		if (port >= MAC_AX_PORT_NUM) {
			PLTFM_MSG_ERR("[ERR]invalid port %d\n", port);
			return MACFUNCINPUT;
		}

		pinfo = &adapter->port_info[get_bp_idx(band, port)];
		if (pinfo->h2c_sm == MAC_AX_PORT_H2C_FAIL) {
			PLTFM_MSG_ERR("[ERR]prev B%dP%d port H2C fail\n",
				      band, port);
			return MACPROCERR;
		}

		cnt = PORT_H2C_DLY_CNT;
		while (pinfo->h2c_sm == MAC_AX_PORT_H2C_BUSY) {
			cnt--;
			if (!cnt) {
				PLTFM_MSG_ERR("[ERR]B%dP%d port H2C busy\n",
					      band, port);
				return MACPROCBUSY;
			}
			PLTFM_DELAY_US(PORT_H2C_DLY_US);
		}

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
		if (!h2cb)
			return MACNPTR;

		hdr = (struct fwcmd_port_init *)
		      h2cb_put(h2cb, sizeof(struct fwcmd_port_init));
		if (!hdr) {
			ret = MACNOBUF;
			goto fail;
		}

		hdr->dword0 =
			cpu_to_le32(SET_WORD(para->bss_color,
					     FWCMD_H2C_PORT_INIT_BSS_COLOR) |
				    SET_WORD(para->mbid_num,
					     FWCMD_H2C_PORT_INIT_MBSSID_NUM) |
				    SET_WORD(para->dtim_period,
					     FWCMD_H2C_PORT_INIT_DTIM_PRD) |
				    SET_WORD(net_type,
					     FWCMD_H2C_PORT_INIT_NET_TYPE) |
				    SET_WORD(port, FWCMD_H2C_PORT_INIT_PORT) |
				    (band ? FWCMD_H2C_PORT_INIT_BAND : 0));

		hdr->dword1 =
			cpu_to_le32(SET_WORD(para->hiq_win,
					     FWCMD_H2C_PORT_INIT_HIQ_WND) |
				    SET_WORD(para->bcn_interval,
					     FWCMD_H2C_PORT_INIT_BCN_ITVL));

		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_MPORT,
				      FWCMD_H2C_FUNC_PORT_INIT,
				      0,
				      0);
		if (ret)
			goto fail;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret)
			goto fail;

#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
		if (ret)
			goto fail;

		h2cb_free(adapter, h2cb);

		h2c_end_flow(adapter);

		pinfo->h2c_sm = MAC_AX_PORT_H2C_BUSY;

		return MACSUCCESS;
fail:
		h2cb_free(adapter, h2cb);

		return ret;
	}
#endif

	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_port_cfg_para cfg_para;
	struct mac_ax_pkt_drop_info info;
	u8 mbid_num;
	u8 mbid_max;
	u8 i = 0;
	u32 ret = MACSUCCESS;
	u32 val32;
	u32 bcn_erly = BCN_ERLY_DEF;
	u32 hold_time = BCN_HOLD_DEF;

	if (!is_curr_dbcc(adapter) && band != MAC_AX_BAND_0) {
		PLTFM_MSG_ERR("[ERR]invalid band idx %d\n", band);
		return MACFUNCINPUT;
	}

	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid port idx %d\n", port);
		return MACPORTCFGPORT;
	}

	mbid_num = net_type == MAC_AX_NET_TYPE_AP && port == MAC_AX_BAND_0 ?
		   para->mbid_num : 0;

	ret = _get_max_mbid(adapter, &mbid_max);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get_max_mbid %d\n", ret);
		return MACPORTCFGPORT;
	}

	if (mbid_num > mbid_max) {
		PLTFM_MSG_ERR("[ERR]invalid MBSSID number %d\n", mbid_num);
		return MACFUNCINPUT;
	}
	if (mbid_num && !(mbid_num % 2))
		mbid_num++;

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	if (mbid_num)
		pinfo->mbssid_en_stat = MAC_AX_MBSSID_ENABLED;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]chk mac en %d\n", ret);
		return ret;
	}

	cfg_para.band = band;
	cfg_para.port = port;
	cfg_para.mbssid_idx = 0;

	if (pinfo->stat != PORT_ST_DIS) {
		cfg_para.val = 0;
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_FUNC_SW, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d cfg func sw 0 fail %d\n",
				      band, port, ret);
			return ret;
		}
	}

	cfg_para.val = 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TX_RPT, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg tx rpt fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_RX_RPT, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg rx rpt fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = net_type;
	ret = _port_cfg(adapter, MAC_AX_PCFG_NET_TYPE, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg net type fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = net_type == MAC_AX_NET_TYPE_NO_LINK ? 0 : 1;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_PRCT, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg bcn prct fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = (net_type == MAC_AX_NET_TYPE_INFRA ||
			net_type == MAC_AX_NET_TYPE_ADHOC) ? 1 : 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_RX_SW, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg rx sw fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = (net_type == MAC_AX_NET_TYPE_INFRA ||
			net_type == MAC_AX_NET_TYPE_ADHOC) ? 1 : 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_RX_SYNC, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg rx sync fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = (net_type == MAC_AX_NET_TYPE_AP ||
			net_type == MAC_AX_NET_TYPE_ADHOC) ? 1 : 0;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TX_SW, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg tx sw fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = (u32)para->bcn_interval;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_INTV, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg bcn intv fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = (u32)para->bss_color;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BSS_CLR, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg bss_color fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = TBTT_AGG_DEF;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TBTT_AGG, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg tbtt agg fail %d\n",
			      band, port, ret);
		return ret;
	}

	for (i = 0; i <= mbid_num; i++) {
		cfg_para.mbssid_idx = mbid_idx_l[i];
		cfg_para.val = para->hiq_win;
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_HIQ_WIN, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d mbid%d cfg hiq win %d\n",
				      band, port, cfg_para.mbssid_idx, ret);
			return ret;
		}
		cfg_para.val = (u32)para->dtim_period;
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_HIQ_DTIM, &cfg_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d mbid%d cfg hiq dtim %d\n",
				      band, port, cfg_para.mbssid_idx, ret);
			return ret;
		}

		info.band = band;
		info.port = port;
		info.mbssid = (u8)mbid_idx_l[i];
		//enable mbssid hiq drop, disable port hiq drop
		info.sel = info.mbssid ?
			   MAC_AX_PKT_DROP_SEL_HIQ_MBSSID :
			   MAC_AX_PKT_DROP_SEL_REL_HIQ_PORT;
		ret = adapter->ops->pkt_drop(adapter, &info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d mbid%d hiq drop %d\n",
				      band, port, info.mbssid, ret);
			return ret;
		}
	}

	if (port == MAC_AX_PORT_0) {
		ret = port0_subspc_set(adapter, band, mbid_num,
				       &bcn_erly, &hold_time);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d subspc set fail %d\n",
				      band, port, ret);
			return ret;
		}
	}

	cfg_para.val = hold_time;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME, &cfg_para);
	if (ret == MACSETVALERR)
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_HOLD_TIME,
				   &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg hold time fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = BCN_MASK_DEF;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_MASK_AREA, &cfg_para);
	if (ret == MACSETVALERR)
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_MASK_AREA,
				   &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg mask area fail %d\n",
			      band, port, ret);
		return ret;
	}

	val32 = MAC_REG_R32(cfg_regl[band][port]) | b_en_l[port];
	MAC_REG_W32(cfg_regl[band][port], val32);

	switch (net_type) {
	case MAC_AX_NET_TYPE_NO_LINK:
		pinfo->stat = PORT_ST_NOLINK;
		break;
	case MAC_AX_NET_TYPE_ADHOC:
		pinfo->stat = PORT_ST_ADHOC;
		break;
	case MAC_AX_NET_TYPE_INFRA:
		pinfo->stat = PORT_ST_INFRA;
		break;
	case MAC_AX_NET_TYPE_AP:
		pinfo->stat = PORT_ST_AP;
		break;
	}

	ret = dly_port_us(adapter, band, port, BCN_ERLY_SET_DLY);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d dly %d us fail %d\n",
			      band, port, BCN_ERLY_SET_DLY, ret);
		return ret;
	}

	cfg_para.val = bcn_erly;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
	if (ret == MACSETVALERR)
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg bcn early fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = BCN_SETUP_DEF;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_SETUP_TIME, &cfg_para);
	if (ret == MACSETVALERR)
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_BCN_SETUP_TIME,
				   &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg setup time fail %d\n",
			      band, port, ret);
		return ret;
	}

	cfg_para.val = TBTT_ERLY_DEF;
	ret = mac_port_cfg(adapter, MAC_AX_PCFG_TBTT_ERLY, &cfg_para);
	if (ret == MACSETVALERR)
		ret = mac_port_cfg(adapter, MAC_AX_PCFG_TBTT_ERLY, &cfg_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d cfg tbtt early fail %d\n",
			      band, port, ret);
		return ret;
	}

	if (port == MAC_AX_PORT_0) {
		ret = port0_mbid_set(adapter, band, mbid_num, mbid_max);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]B%dP%d mbid set fail %d\n",
				      band, port, ret);
			return ret;
		}
	}

	return ret;
}

u32 mac_tsf_sync(struct mac_ax_adapter *adapter, u8 from_port, u8 to_port,
		 s32 sync_offset, enum mac_ax_tsf_sync_act action)
{
	u32 abs_sync_offset, val32, ret = MACSUCCESS;
	u16 to_port_reg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (from_port > MAX_TSF_SOURCE_PORT || to_port > MAX_TSF_TARGET_PORT ||
	    from_port == to_port) {
		PLTFM_MSG_ERR("[ERR] Invalid tsf_sync port.\n");
		return MACFUNCINPUT;
	}

	abs_sync_offset = sync_offset > 0 ? sync_offset : -sync_offset;

	if (abs_sync_offset > MAX_TSF_SYNC_OFFSET) {
		PLTFM_MSG_ERR("[ERR] Invalid tsf_sync offset.\n");
		return MACFUNCINPUT;
	}

	if (sync_offset < 0)
		abs_sync_offset |= BIT18;

	switch (to_port) {
	case MAC_AX_PORT_0:
		to_port_reg = R_AX_PORT_0_TSF_SYNC;
		break;
	case MAC_AX_PORT_1:
		to_port_reg = R_AX_PORT_1_TSF_SYNC;
		break;
	case MAC_AX_PORT_2:
		to_port_reg = R_AX_PORT_2_TSF_SYNC;
		break;
	case MAC_AX_PORT_3:
		to_port_reg = R_AX_PORT_3_TSF_SYNC;
		break;
	case MAC_AX_PORT_4:
		to_port_reg = R_AX_PORT_4_TSF_SYNC;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] Invalid tsf_sync input.\n");
		return MACFUNCINPUT;
	}

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD(to_port_reg,
				     B_AX_P0_TSFTR_SYNC_OFFSET_MSK <<
				     B_AX_P0_TSFTR_SYNC_OFFSET_SH,
				     abs_sync_offset, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail;"
				      "offset: %u, ret: %u\n",
				      __func__, to_port_reg, ret);
			return ret;
		}
		ret = MAC_REG_W_OFLD(to_port_reg,
				     B_AX_P0_SYNC_PORT_SRC_SEL_MSK <<
				     B_AX_P0_SYNC_PORT_SRC_SEL_SH,
				     from_port, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail;"
				      "offset: %u, ret: %u\n",
				      __func__, to_port_reg, ret);
			return ret;
		}

		switch (action) {
		case MAC_AX_TSF_SYNC_NOW_ONCE:
			ret = MAC_REG_W_OFLD(to_port_reg, B_AX_P0_SYNC_NOW_P,
					     1, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: write offload fail;"
					      "offset: %u, ret: %u\n",
					      __func__, to_port_reg, ret);
				return ret;
			}
			ret = MAC_REG_P_OFLD(to_port_reg, B_AX_P0_SYNC_NOW_P,
					     0, 1);
			break;
		case MAC_AX_TSF_EN_SYNC_AUTO:
			ret = MAC_REG_W_OFLD(to_port_reg, B_AX_P0_AUTO_SYNC,
					     1, 1);
			break;
		case MAC_AX_TSF_DIS_SYNC_AUTO:
			ret = MAC_REG_W_OFLD(to_port_reg, B_AX_P0_AUTO_SYNC,
					     0, 1);
			break;
		default:
			PLTFM_MSG_ERR("[ERR] Invalid tsf_sync input.\n");
			return MACFUNCINPUT;
		}

		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail;"
				      "offset: %u, ret: %u\n",
				      __func__, to_port_reg, ret);
			return ret;
		}

		return ret;
	}
#endif
	val32 = MAC_REG_R32(to_port_reg);
	val32 = SET_CLR_WORD(val32, abs_sync_offset, B_AX_P0_TSFTR_SYNC_OFFSET);
	val32 = SET_CLR_WORD(val32, from_port, B_AX_P0_SYNC_PORT_SRC_SEL);
	MAC_REG_W32(to_port_reg, val32);

	switch (action) {
	case MAC_AX_TSF_SYNC_NOW_ONCE:
		MAC_REG_W32(to_port_reg, val32 | B_AX_P0_SYNC_NOW_P);
		break;
	case MAC_AX_TSF_EN_SYNC_AUTO:
		MAC_REG_W32(to_port_reg, val32 | B_AX_P0_AUTO_SYNC);
		break;
	case MAC_AX_TSF_DIS_SYNC_AUTO:
		MAC_REG_W32(to_port_reg, val32 & ~B_AX_P0_AUTO_SYNC);
		break;
	default:
		PLTFM_MSG_ERR("[ERR] Invalid tsf_sync input.\n");
		return MACFUNCINPUT;
	}

	return ret;
}

u32 mac_parse_bcn_stats_c2h(struct mac_ax_adapter *adapter,
			    u8 *content, struct mac_ax_bcn_cnt *val)

{
	u32 *pdata, data0, data1;
	u8 port_mbssid;

	pdata = (u32 *)content;
	data0 = le32_to_cpu(*pdata);
	data1 = le32_to_cpu(*(pdata + 1));

	port_mbssid = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_PORT_MBSSID_IDX);
	if (port_mbssid < MAC_AX_P0_MBID_LAST) {
		val->port = 0;
		val->mbssid = port_mbssid;
	} else {
		val->port = port_mbssid -  MAC_AX_P0_MBID_LAST + 1;
		val->mbssid = 0;
	}
	val->band = data0 & FWCMD_C2H_BCN_CNT_BAND_IDX ? 1 : 0;

	val->cca_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_CCA_FAIL_CNT);
	val->edcca_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_EDCCA_FAIL_CNT);
	val->nav_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_NAV_FAIL_CNT);
	val->txon_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_TXON_FAIL_CNT);
	val->mac_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_MAC_FAIL_CNT);
	val->others_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_OTHERS_FAIL_CNT);
	val->lock_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_LOCK_FAIL_CNT);
	val->cmp_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_CMP_FAIL_CNT);
	val->invalid_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_INVALID_FAIL_CNT);
	val->srchend_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_SRCHEND_FAIL_CNT);
	val->ok_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_OK_CNT);

	return MACSUCCESS;
}

u32 get_bcn_stats_event(struct mac_ax_adapter *adapter,
			struct rtw_c2h_info *c2h,
			enum phl_msg_evt_id *id, u8 *c2h_info)
{
	struct mac_ax_bcn_cnt *info;
	u32 *pdata, data0, data1;
	u8 port_mbssid;

	info = (struct mac_ax_bcn_cnt *)c2h_info;
	pdata = (u32 *)c2h->content;
	data0 = le32_to_cpu(*pdata);
	data1 = le32_to_cpu(*(pdata + 1));

	port_mbssid = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_PORT_MBSSID_IDX);
	if (port_mbssid < MAC_AX_P0_MBID_LAST) {
		info->port = 0;
		info->mbssid = port_mbssid;
	} else {
		info->port = port_mbssid -  MAC_AX_P0_MBID_LAST + 1;
		info->mbssid = 0;
	}
	info->band = data0 & FWCMD_C2H_BCN_CNT_BAND_IDX ? 1 : 0;

	info->cca_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_CCA_FAIL_CNT);
	info->edcca_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_EDCCA_FAIL_CNT);
	info->nav_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_NAV_FAIL_CNT);
	info->txon_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_TXON_FAIL_CNT);
	info->mac_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_MAC_FAIL_CNT);
	info->others_cnt = GET_FIELD(data0, FWCMD_C2H_BCN_CNT_OTHERS_FAIL_CNT);
	info->lock_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_LOCK_FAIL_CNT);
	info->cmp_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_CMP_FAIL_CNT);
	info->invalid_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_INVALID_FAIL_CNT);
	info->srchend_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_SRCHEND_FAIL_CNT);
	info->ok_cnt = GET_FIELD(data1, FWCMD_C2H_BCN_CNT_OK_CNT);

	*id = MSG_EVT_BCN_CNT_RPT;

	return MACSUCCESS;
}

u32 mac_tsf32_togl_h2c(struct mac_ax_adapter *adapter,
		       struct mac_ax_t32_togl_info *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_tsf32_togl *hdr;
	u32 ret = MACSUCCESS;

	if (info->band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid band %d\n", info->band);
		return MACFUNCINPUT;
	}

	if (info->port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid port %d\n", info->port);
		return MACFUNCINPUT;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_tsf32_togl *)
	      h2cb_put(h2cb, sizeof(struct fwcmd_tsf32_togl));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->port, FWCMD_H2C_TSF32_TOGL_PORT) |
			    SET_WORD(info->early, FWCMD_H2C_TSF32_TOGL_EARLY) |
			    (info->band ? FWCMD_H2C_TSF32_TOGL_BAND : 0) |
			    (info->en ? FWCMD_H2C_TSF32_TOGL_EN : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_TSF32_TOGL,
			      0,
			      0);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_get_t32_togl_rpt(struct mac_ax_adapter *adapter,
			 struct mac_ax_t32_togl_rpt *ret_rpt)
{
	struct mac_ax_t32_togl_rpt *rpt;
	u8 b_idx, p_idx;

	for (b_idx = MAC_AX_BAND_0; b_idx < MAC_AX_BAND_NUM; b_idx++) {
		for (p_idx = MAC_AX_PORT_0; p_idx < MAC_AX_PORT_NUM; p_idx++) {
			rpt = &adapter->t32_togl_rpt[get_bp_idx(b_idx, p_idx)];
			if (!rpt->valid)
				continue;
			PLTFM_MEMCPY(ret_rpt, rpt,
				     sizeof(struct mac_ax_t32_togl_rpt));
			rpt->valid = 0;
			return MACSUCCESS;
		}
	}

	PLTFM_MSG_WARN("[WARN]no tsf32 togl rpt find\n");
	return MACNOITEM;
}

u32 mport_info_init(struct mac_ax_adapter *adapter)
{
	u8 b_idx, p_idx;
	u32 idx;

	adapter->t32_togl_rpt =
		(struct mac_ax_t32_togl_rpt *)PLTFM_MALLOC(t32_togl_rpt_size);
	adapter->port_info =
		(struct mac_ax_port_info *)PLTFM_MALLOC(port_info_size);
	for (b_idx = MAC_AX_BAND_0; b_idx < MAC_AX_BAND_NUM; b_idx++) {
		for (p_idx = MAC_AX_PORT_0; p_idx < MAC_AX_PORT_NUM; p_idx++) {
			idx = get_bp_idx(b_idx, p_idx);
			PLTFM_MEMSET(&adapter->t32_togl_rpt[idx], 0,
				     sizeof(struct mac_ax_t32_togl_rpt));
			PLTFM_MEMSET(&adapter->port_info[idx], 0,
				     sizeof(struct mac_ax_port_info));
		}
	}

	return MACSUCCESS;
}

u32 mport_info_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_FREE(adapter->t32_togl_rpt, t32_togl_rpt_size);
	PLTFM_FREE(adapter->port_info, port_info_size);

	return MACSUCCESS;
}

