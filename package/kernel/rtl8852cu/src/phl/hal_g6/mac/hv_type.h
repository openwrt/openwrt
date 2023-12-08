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

#ifndef _HV_AX_TYPE_H_
#define _HV_AX_TYPE_H_

#include "pltfm_cfg.h"
#include "chip_cfg.h"

typedef unsigned long long u64;

#define SS_LINK_SIZE		256
#define DL_RUGRP_SIZE		8
#define DL_MUTBL_SIZE		5

#define HV_AX_FPGA 0
#define HV_AX_ASIC 1
#define HV_AX_PXP  2

/**
 * @enum hv_ax_ss_wmm
 *
 * @brief hv_ax_ss_wmm
 *
 * @var hv_ax_ss_wmm::HV_AX_SS_WMM0
 * Please Place Description here.
 * @var hv_ax_ss_wmm::HV_AX_SS_WMM1
 * Please Place Description here.
 * @var hv_ax_ss_wmm::HV_AX_SS_WMM2
 * Please Place Description here.
 * @var hv_ax_ss_wmm::HV_AX_SS_WMM3
 * Please Place Description here.
 * @var hv_ax_ss_wmm::HV_AX_SS_UL
 * Please Place Description here.
 */
enum hv_ax_ss_wmm {
	HV_AX_SS_WMM0,
	HV_AX_SS_WMM1,
	HV_AX_SS_WMM2,
	HV_AX_SS_WMM3,
	HV_AX_SS_UL,
};

/**
 * @enum hv_ax_freerun_cfg
 *
 * @brief hv_ax_freerun_cfg
 *
 * @var hv_ax_freerun_cfg::HV_AX_FREERUN_EN
 * Please Place Description here.
 * @var hv_ax_freerun_cfg::HV_AX_FREERUN_DIS
 * Please Place Description here.
 * @var hv_ax_freerun_cfg::HV_AX_FREERUN_RST
 * Please Place Description here.
 */
enum hv_ax_freerun_cfg {
	HV_AX_FREERUN_EN,
	HV_AX_FREERUN_DIS,
	HV_AX_FREERUN_RST,
};

/**
 * @enum hv_ax_ss_quota_mode_cfg
 *
 * @brief hv_ax_ss_quota_mode_cfg
 *
 * @var hv_ax_ss_quota_mode_cfg::HV_AX_SS_QUOTA_MODE_GET
 * Please Place Description here.
 * @var hv_ax_ss_quota_mode_cfg::HV_AX_SS_QUOTA_MODE_SET
 * Please Place Description here.
 */
enum hv_ax_ss_quota_mode_cfg {
	HV_AX_SS_QUOTA_MODE_GET,
	HV_AX_SS_QUOTA_MODE_SET,
};

/**
 * @enum hv_ax_ss_wmm_tbl_cfg
 *
 * @brief hv_ax_ss_wmm_tbl_cfg
 *
 * @var hv_ax_ss_wmm_tbl_cfg::HV_AX_SS_WMM_TBL_SET
 * Please Place Description here.
 */
enum hv_ax_ss_wmm_tbl_cfg {
	HV_AX_SS_WMM_TBL_SET,
};

/**
 * @enum hv_ax_sta_len_cmd
 *
 * @brief hv_ax_sta_len_cmd
 *
 * @var hv_ax_sta_len_cmd::HV_AX_STA_LEN_INCR
 * Please Place Description here.
 * @var hv_ax_sta_len_cmd::HV_AX_STA_LEN_DECR
 * Please Place Description here.
 */
enum hv_ax_sta_len_cmd {
	HV_AX_STA_LEN_INCR,
	HV_AX_STA_LEN_DECR,
};

/**
 * @enum hv_ax_sta_len_cfg
 *
 * @brief hv_ax_sta_len_cfg
 *
 * @var hv_ax_sta_len_cfg::HV_AX_STA_LEN_CFG_GET
 * Please Place Description here.
 * @var hv_ax_sta_len_cfg::HV_AX_STA_LEN_CFG_SET
 * Please Place Description here.
 * @var hv_ax_sta_len_cfg::HV_AX_STA_LEN_CFG_GET_INDIR
 * Please Place Description here.
 * @var hv_ax_sta_len_cfg::HV_AX_STA_LEN_CFG_SET_INDIR
 * Please Place Description here.
 */
enum hv_ax_sta_len_cfg {
	HV_AX_STA_LEN_CFG_GET,
	HV_AX_STA_LEN_CFG_SET,
	HV_AX_STA_LEN_CFG_GET_INDIR,
	HV_AX_STA_LEN_CFG_SET_INDIR,
};

/**
 * @enum hv_ax_sta_quota_cfg
 *
 * @brief hv_ax_sta_quota_cfg
 *
 * @var hv_ax_sta_quota_cfg::HV_AX_STA_QUOTA_CFG_VAL_GET
 * Please Place Description here.
 * @var hv_ax_sta_quota_cfg::HV_AX_STA_QUOTA_CFG_VAL_SET
 * Please Place Description here.
 * @var hv_ax_sta_quota_cfg::HV_AX_STA_QUOTA_CFG_SETTING_GET
 * Please Place Description here.
 * @var hv_ax_sta_quota_cfg::HV_AX_STA_QUOTA_CFG_SETTING_SET
 * Please Place Description here.
 */
enum hv_ax_sta_quota_cfg {
	HV_AX_STA_QUOTA_CFG_VAL_GET,
	HV_AX_STA_QUOTA_CFG_VAL_SET,
	HV_AX_STA_QUOTA_CFG_SETTING_GET,
	HV_AX_STA_QUOTA_CFG_SETTING_SET,
};

/**
 * @enum hv_ax_sta_muru_cfg
 *
 * @brief hv_ax_sta_muru_cfg
 *
 * @var hv_ax_sta_muru_cfg::HV_AX_STA_MURU_CFG_GET
 * Please Place Description here.
 * @var hv_ax_sta_muru_cfg::HV_AX_STA_MURU_CFG_SET
 * Please Place Description here.
 */
enum hv_ax_sta_muru_cfg {
	HV_AX_STA_MURU_CFG_GET,
	HV_AX_STA_MURU_CFG_SET,
};

/**
 * @enum hv_ax_ss_quota_mode
 *
 * @brief hv_ax_ss_quota_mode
 *
 * @var hv_ax_ss_quota_mode::HV_AX_SS_QUOTA_MODE_TIME
 * Please Place Description here.
 * @var hv_ax_ss_quota_mode::HV_AX_SS_QUOTA_MODE_CNT
 * Please Place Description here.
 */
enum hv_ax_ss_quota_mode {
	HV_AX_SS_QUOTA_MODE_TIME = 0,
	HV_AX_SS_QUOTA_MODE_CNT = 1,
};

/**
 * @enum hv_ax_ss_link_cfg
 *
 * @brief hv_ax_ss_link_cfg
 *
 * @var hv_ax_ss_link_cfg::HV_AX_SS_LINK_CFG_GET
 * Please Place Description here.
 * @var hv_ax_ss_link_cfg::HV_AX_SS_LINK_CFG_ADD
 * Please Place Description here.
 * @var hv_ax_ss_link_cfg::HV_AX_SS_LINK_CFG_DEL
 * Please Place Description here.
 * @var hv_ax_ss_link_cfg::HV_AX_SS_LINK_CFG_CLEAN
 * Please Place Description here.
 */
enum hv_ax_ss_link_cfg {
	HV_AX_SS_LINK_CFG_GET,
	HV_AX_SS_LINK_CFG_ADD,
	HV_AX_SS_LINK_CFG_DEL,
	HV_AX_SS_LINK_CFG_CLEAN,
};

/**
 * @enum hv_ax_sta_bmp_cfg
 *
 * @brief hv_ax_sta_bmp_cfg
 *
 * @var hv_ax_sta_bmp_cfg::HV_AX_STA_BMP_CFG_GET
 * Please Place Description here.
 * @var hv_ax_sta_bmp_cfg::HV_AX_STA_BMP_CFG_SET
 * Please Place Description here.
 */
enum hv_ax_sta_bmp_cfg {
	HV_AX_STA_BMP_CFG_GET,
	HV_AX_STA_BMP_CFG_SET,
};

/**
 * @enum hv_ax_ss_rpt_cfg
 *
 * @brief hv_ax_ss_rpt_cfg
 *
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_UL_RPT_CFG_GET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_UL_RPT_CFG_SET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_SU_RPT_CFG_GET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_SU_RPT_CFG_SET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_MU_RPT_CFG_GET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_MU_RPT_CFG_SET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_RU_RPT_CFG_GET
 * Please Place Description here.
 * @var hv_ax_ss_rpt_cfg::HV_AX_SS_DL_RU_RPT_CFG_SET
 * Please Place Description here.
 */
enum hv_ax_ss_rpt_cfg {
	HV_AX_SS_UL_RPT_CFG_GET,
	HV_AX_SS_UL_RPT_CFG_SET,
	HV_AX_SS_DL_SU_RPT_CFG_GET,
	HV_AX_SS_DL_SU_RPT_CFG_SET,
	HV_AX_SS_DL_MU_RPT_CFG_GET,
	HV_AX_SS_DL_MU_RPT_CFG_SET,
	HV_AX_SS_DL_RU_RPT_CFG_GET,
	HV_AX_SS_DL_RU_RPT_CFG_SET,
};

/**
 * @enum hv_ax_ss_rpt_path_cfg
 *
 * @brief hv_ax_ss_rpt_path_cfg
 *
 * @var hv_ax_ss_rpt_path_cfg::HV_AX_SS_RPT_PATH_CPU
 * Please Place Description here.
 * @var hv_ax_ss_rpt_path_cfg::HV_AX_SS_RPT_PATH_HOST
 * Please Place Description here.
 */
enum hv_ax_ss_rpt_path_cfg {
	HV_AX_SS_RPT_PATH_CPU,
	HV_AX_SS_RPT_PATH_HOST,
};

/**
 * @enum hv_ax_ss_dlru_search_mode
 *
 * @brief hv_ax_ss_dlru_search_mode
 *
 * @var hv_ax_ss_dlru_search_mode::HV_AX_SS_DLRU_SEARCH_LINK_THEN_AC
 * Please Place Description here.
 * @var hv_ax_ss_dlru_search_mode::HV_AX_SS_DLRU_SEARCH_AC
 * Please Place Description here.
 * @var hv_ax_ss_dlru_search_mode::HV_AX_SS_DLRU_SEARCH_LINK_THEN_LEN
 * Please Place Description here.
 * @var hv_ax_ss_dlru_search_mode::HV_AX_SS_DLRU_SEARCH_LEN
 * Please Place Description here.
 */
enum hv_ax_ss_dlru_search_mode {
	HV_AX_SS_DLRU_SEARCH_LINK_THEN_AC = 0,
	HV_AX_SS_DLRU_SEARCH_AC = 1,
	HV_AX_SS_DLRU_SEARCH_LINK_THEN_LEN = 2,
	HV_AX_SS_DLRU_SEARCH_LEN = 3,
};

/**
 * @enum hv_ax_ss_delay_tx_band
 *
 * @brief hv_ax_ss_delay_tx_band
 *
 * @var hv_ax_ss_delay_tx_band::HV_AX_SS_DELAY_TX_DIS
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_band::HV_AX_SS_DELAY_TX_B0
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_band::HV_AX_SS_DELAY_TX_B1
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_band::HV_AX_SS_DELAY_TX_B0_B1
 * Please Place Description here.
 */
enum hv_ax_ss_delay_tx_band {
	HV_AX_SS_DELAY_TX_DIS = 0,
	HV_AX_SS_DELAY_TX_B0 = 1,
	HV_AX_SS_DELAY_TX_B1 = 2,
	HV_AX_SS_DELAY_TX_B0_B1 = 3,
};

enum hv_ax_pwr_seq_sel {
	HV_AX_PWR_SEQ_SEL_ON_NIC,
	HV_AX_PWR_SEQ_SEL_OFF_NIC,
	HV_AX_PWR_SEQ_SEL_CARD_DIS,
	HV_AX_PWR_SEQ_SEL_ENTER_LPS,
	HV_AX_PWR_SEQ_SEL_LEAVE_LPS,
	HV_AX_PWR_SEQ_SEL_IPS,
	HV_AX_PWR_SEQ_SEL_ON_AP,
	HV_AX_PWR_SEQ_SEL_OFF_AP,
};

/**
 * @enum mac_ax_plat_module
 *
 * @brief mac_ax_plat_module
 *
 * @var mac_ax_plat_module::SPIC
 * Please Place Description here.
 * @var mac_ax_plat_module::CPU_PLATFORM
 * Please Place Description here.
 * @var mac_ax_plat_module::EFUSE_CTRL
 * Please Place Description here.
 * @var mac_ax_plat_module::IDDMA
 * Please Place Description here.
 * @var mac_ax_plat_module::AXIDMA
 * Please Place Description here.
 * @var mac_ax_plat_module::IPSEC
 * Please Place Description here.
 * @var mac_ax_plat_module::UART
 * Please Place Description here.
 * @var mac_ax_plat_module::HIOE
 * Please Place Description here.
 * @var mac_ax_plat_module::WATCHDOG
 * Please Place Description here.
 * @var mac_ax_plat_module::SECURITY
 * Please Place Description here.
 * @var mac_ax_plat_module::PLAT_MODULE_MAX
 * Please Place Description here.
 */
enum mac_ax_plat_module {
	SPIC,
	CPU_PLATFORM,
	EFUSE_CTRL,
	IDDMA,
	AXIDMA,
	IPSEC,
	UART,
	HIOE,
	WATCHDOG,
	SECURITY,
	PLAT_MODULE_MAX
};

/**
 * @struct hv_ax_lifetime_mg2_cfg
 * @brief hv_ax_lifetime_mg2_cfg
 *
 * @var hv_ax_lifetime_mg2_cfg::band
 * Please Place Description here.
 * @var hv_ax_lifetime_mg2_cfg::en
 * Please Place Description here.
 * @var hv_ax_lifetime_mg2_cfg::val
 * Please Place Description here.
 */
struct hv_ax_lifetime_mg2_cfg {
	u8 band;
	u8 en;
	u16 val;
};

/**
 * @struct hv_ax_ss_delay_tx_info
 * @brief hv_ax_ss_delay_tx_info
 *
 * @var hv_ax_ss_delay_tx_info::band_sel
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::vovi_to_0
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::bebk_to_0
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::vovi_to_1
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::bebk_to_1
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::vovi_len_0
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::bebk_len_0
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::vovi_len_1
 * Please Place Description here.
 * @var hv_ax_ss_delay_tx_info::bebk_len_1
 * Please Place Description here.
 */
struct hv_ax_ss_delay_tx_info {
	enum hv_ax_ss_delay_tx_band band_sel;
	u8 vovi_to_0;
	u8 bebk_to_0;
	u8 vovi_to_1;
	u8 bebk_to_1;
	u8 vovi_len_0;
	u8 bebk_len_0;
	u8 vovi_len_1;
	u8 bebk_len_1;
};

/**
 * @struct hv_ax_ss_search_info
 * @brief hv_ax_ss_search_info
 *
 * @var hv_ax_ss_search_info::wmm
 * Please Place Description here.
 * @var hv_ax_ss_search_info::ac
 * Please Place Description here.
 * @var hv_ax_ss_search_info::ul
 * Please Place Description here.
 * @var hv_ax_ss_search_info::twt_grp
 * Please Place Description here.
 * @var hv_ax_ss_search_info::mode_sel
 * Please Place Description here.
 * @var hv_ax_ss_search_info::macid
 * Please Place Description here.
 * @var hv_ax_ss_search_info::search_fail
 * Please Place Description here.
 */
struct hv_ax_ss_search_info {
	u8 wmm;
	u8 ac;
	u8 ul;
	u8 twt_grp;
	u8 mode_sel;
	u8 macid;
	u8 search_fail;
};

/**
 * @struct hv_ax_ss_dl_rpt_info
 * @brief hv_ax_ss_dl_rpt_info
 *
 * @var hv_ax_ss_dl_rpt_info::wmm0_max
 * Please Place Description here.
 * @var hv_ax_ss_dl_rpt_info::wmm1_max
 * Please Place Description here.
 * @var hv_ax_ss_dl_rpt_info::twt_wmm0_max
 * Please Place Description here.
 * @var hv_ax_ss_dl_rpt_info::twt_wmm1_max
 * Please Place Description here.
 */
struct hv_ax_ss_dl_rpt_info {
	u8 wmm0_max;
	u8 wmm1_max;
	u8 twt_wmm0_max;
	u8 twt_wmm1_max;
};

/**
 * @struct hv_ax_ss_ul_rpt_info
 * @brief hv_ax_ss_ul_rpt_info
 *
 * @var hv_ax_ss_ul_rpt_info::ul_wmm_sel
 * Please Place Description here.
 * @var hv_ax_ss_ul_rpt_info::ul_su_max
 * Please Place Description here.
 * @var hv_ax_ss_ul_rpt_info::twt_ul_su_max
 * Please Place Description here.
 * @var hv_ax_ss_ul_rpt_info::ul_ru_max
 * Please Place Description here.
 */
struct hv_ax_ss_ul_rpt_info {
	u8 ul_wmm_sel;
	u8 ul_su_max;
	u8 twt_ul_su_max;
	u8 ul_ru_max;
};

/**
 * @struct hv_ax_ss_link_info
 * @brief hv_ax_ss_link_info
 *
 * @var hv_ax_ss_link_info::wmm
 * Please Place Description here.
 * @var hv_ax_ss_link_info::ac
 * Please Place Description here.
 * @var hv_ax_ss_link_info::ul
 * Please Place Description here.
 * @var hv_ax_ss_link_info::link_list
 * Please Place Description here.
 * @var hv_ax_ss_link_info::link_head
 * Please Place Description here.
 * @var hv_ax_ss_link_info::link_tail
 * Please Place Description here.
 * @var hv_ax_ss_link_info::link_len
 * Please Place Description here.
 * @var hv_ax_ss_link_info::macid0
 * Please Place Description here.
 * @var hv_ax_ss_link_info::macid1
 * Please Place Description here.
 * @var hv_ax_ss_link_info::macid2
 * Please Place Description here.
 * @var hv_ax_ss_link_info::link_bitmap
 * Please Place Description here.
 */
struct hv_ax_ss_link_info {
	u8 wmm;
	u8 ac;
	u8 ul;
	u8 link_list[SS_LINK_SIZE];
	u8 link_head;
	u8 link_tail;
	u8 link_len;
	u8 macid0;
	u8 macid1;
	u8 macid2;
	u8 link_bitmap[SS_LINK_SIZE];
};

/**
 * @struct hv_ax_sta_dl_rugrp_ctrl
 * @brief hv_ax_sta_dl_rugrp_ctrl
 *
 * @var hv_ax_sta_dl_rugrp_ctrl::grpid
 * Please Place Description here.
 * @var hv_ax_sta_dl_rugrp_ctrl::grp_vld
 * Please Place Description here.
 * @var hv_ax_sta_dl_rugrp_ctrl::macid
 * Please Place Description here.
 * @var hv_ax_sta_dl_rugrp_ctrl::dis_ac
 * Please Place Description here.
 */
struct hv_ax_sta_dl_rugrp_ctrl {
	u8 grpid;
	u8 grp_vld;
	u8 macid[DL_RUGRP_SIZE];
	u8 dis_ac[DL_RUGRP_SIZE];
};

/**
 * @struct hv_ax_sta_dl_mutbl_ctrl
 * @brief hv_ax_sta_dl_mutbl_ctrl
 *
 * @var hv_ax_sta_dl_mutbl_ctrl::tbl_id
 * Please Place Description here.
 * @var hv_ax_sta_dl_mutbl_ctrl::tbl_vld
 * Please Place Description here.
 * @var hv_ax_sta_dl_mutbl_ctrl::macid
 * Please Place Description here.
 * @var hv_ax_sta_dl_mutbl_ctrl::score
 * Please Place Description here.
 */
struct hv_ax_sta_dl_mutbl_ctrl {
	u8 tbl_id;
	u8 tbl_vld;
	u8 macid;
	u8 score[DL_MUTBL_SIZE];
};

/**
 * @struct hv_ax_ss_quota_mode_ctrl
 * @brief hv_ax_ss_quota_mode_ctrl
 *
 * @var hv_ax_ss_quota_mode_ctrl::wmm
 * Please Place Description here.
 * @var hv_ax_ss_quota_mode_ctrl::mode
 * Please Place Description here.
 */
struct hv_ax_ss_quota_mode_ctrl {
	enum hv_ax_ss_wmm wmm;
	enum hv_ax_ss_quota_mode mode;
};

/**
 * @struct hv_ax_sta_muru_ctrl
 * @brief hv_ax_sta_muru_ctrl
 *
 * @var hv_ax_sta_muru_ctrl::macid
 * Please Place Description here.
 * @var hv_ax_sta_muru_ctrl::ul_tbl
 * Please Place Description here.
 * @var hv_ax_sta_muru_ctrl::dl_muru_dis
 * Please Place Description here.
 */
struct hv_ax_sta_muru_ctrl {
	u8 macid;
	u32 ul_tbl;
	u32 dl_muru_dis;
};

/**
 * @struct hv_ax_sta_bmp_ctrl
 * @brief hv_ax_sta_bmp_ctrl
 *
 * @var hv_ax_sta_bmp_ctrl::macid
 * Please Place Description here.
 * @var hv_ax_sta_bmp_ctrl::bmp
 * Please Place Description here.
 * @var hv_ax_sta_bmp_ctrl::mask
 * Please Place Description here.
 */
struct hv_ax_sta_bmp_ctrl {
	u8 macid;
	u32 bmp;
	u32 mask;
};

/**
 * @struct hv_ax_sta_quota
 * @brief hv_ax_sta_quota
 *
 * @var hv_ax_sta_quota::macid
 * Please Place Description here.
 * @var hv_ax_sta_quota::vo_quota
 * Please Place Description here.
 * @var hv_ax_sta_quota::vi_quota
 * Please Place Description here.
 * @var hv_ax_sta_quota::be_quota
 * Please Place Description here.
 * @var hv_ax_sta_quota::bk_quota
 * Please Place Description here.
 * @var hv_ax_sta_quota::ul_quota
 * Please Place Description here.
 */
struct hv_ax_sta_quota {
	u8 macid;
	u32 vo_quota;
	u32 vi_quota;
	u32 be_quota;
	u32 bk_quota;
	u32 ul_quota;
};

/**
 * @struct hv_ax_sta_len_ctrl
 * @brief hv_ax_sta_len_ctrl
 *
 * @var hv_ax_sta_len_ctrl::macid
 * Please Place Description here.
 * @var hv_ax_sta_len_ctrl::len
 * Please Place Description here.
 * @var hv_ax_sta_len_ctrl::ac
 * Please Place Description here.
 * @var hv_ax_sta_len_ctrl::cmd
 * Please Place Description here.
 */
struct hv_ax_sta_len_ctrl {
	u8 macid;
	u32 len;
	enum mac_ax_cmac_ac_sel ac;
	enum hv_ax_sta_len_cmd cmd;
};

/**
 * @struct hv_ax_sta_len
 * @brief hv_ax_sta_len
 *
 * @var hv_ax_sta_len::macid
 * Please Place Description here.
 * @var hv_ax_sta_len::vo_len
 * Please Place Description here.
 * @var hv_ax_sta_len::vi_len
 * Please Place Description here.
 * @var hv_ax_sta_len::be_len
 * Please Place Description here.
 * @var hv_ax_sta_len::bk_len
 * Please Place Description here.
 * @var hv_ax_sta_len::bsr_len
 * Please Place Description here.
 * @var hv_ax_sta_len::bsr_ac_type
 * Please Place Description here.
 */
struct hv_ax_sta_len {
	u8 macid;
	u32 vo_len;
	u32 vi_len;
	u32 be_len;
	u32 bk_len;
	u32 bsr_len;
	u8 bsr_ac_type;
};

/**
 * @struct hv_aggregator_t
 * @brief hv_aggregator_t
 *
 * @var hv_aggregator_t::pkt
 * Please Place Description here.
 * @var hv_aggregator_t::len
 * Please Place Description here.
 * @var hv_aggregator_t::agg_num
 * Please Place Description here.
 */
struct hv_aggregator_t {
	u8 *pkt;
	u32 len;
	u32 agg_num;
};

/**
 * @struct hv_dbg_port
 * @brief hv_dbg_port
 *
 * @var hv_dbg_port::info
 * Please Place Description here.
 * @var hv_dbg_port::len
 * Please Place Description here.
 * @var hv_dbg_port::read_addr
 * Please Place Description here.
 * @var hv_dbg_port::sel_addr
 * Please Place Description here.
 * @var hv_dbg_port::dbg_sel
 * Please Place Description here.
 */
struct hv_dbg_port {
	struct hv_dbg_port_info *info;
	u32 len;
	u32 read_addr;
	u32 sel_addr;
	u32 dbg_sel;
};

/**
 * @struct hv_dbg_port_info
 * @brief hv_dbg_port_info
 *
 * @var hv_dbg_port_info::addr
 * Please Place Description here.
 * @var hv_dbg_port_info::val
 * Please Place Description here.
 */
struct hv_dbg_port_info {
	u32 addr;
	u32 val;
};

/**
 * @struct mac_ax_mac_test
 * @brief mac_ax_mac_test
 *
 * @var mac_ax_mac_test::dword0
 * Please Place Description here.
 * @var mac_ax_mac_test::dword1
 * Please Place Description here.
 */
struct mac_ax_mac_test {
	u32 dword0;
	u32 dword1;
};

/**
 * @struct mac_ax_plat_auto_test
 * @brief mac_ax_plat_auto_test
 *
 * @var mac_ax_plat_auto_test::dword0
 * Please Place Description here.
 * @var mac_ax_plat_auto_test::dword1
 * Please Place Description here.
 * @var mac_ax_plat_auto_test::dword2
 * Please Place Description here.
 * @var mac_ax_plat_auto_test::dword3
 * Please Place Description here.
 * @var mac_ax_plat_auto_test::dword4
 * Please Place Description here.
 * @var mac_ax_plat_auto_test::dword5
 * Please Place Description here.
 */
struct mac_ax_plat_auto_test {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
};

/**
 * @struct hv_ctrl_frame_cnt
 * @brief hv_ctrl_frame_cnt
 *
 * @var hv_ctrl_frame_cnt::band
 * Please Place Description here.
 * @var hv_ctrl_frame_cnt::op
 * Please Place Description here.
 * @var hv_ctrl_frame_cnt::stype
 * Please Place Description here.
 * @var hv_ctrl_frame_cnt::idx
 * Please Place Description here.
 * @var hv_ctrl_frame_cnt::rval
 * Please Place Description here.
 * @var hv_ctrl_frame_cnt::tval
 * Please Place Description here.
 */
struct hv_ctrl_frame_cnt {
#define MAC_HV_CTRL_CNT_R 0
#define MAC_HV_CTRL_CNT_W 1
#define MAC_HV_CTRL_CNT_RST 2
#define MAC_HV_CTRL_CNT_RST_ALL 3
	u8 band;
	u8 op;
	u8 stype;
#define MAC_HV_CTRL_CNT_NUM 16
	u8 idx;
	u16 rval;
	u16 tval;
};

/**
 * @struct hv_rx_cnt
 * @brief hv_rx_cnt
 *
 * @var hv_rx_cnt::op
 * Please Place Description here.
 * @var hv_rx_cnt::idx
 * Please Place Description here.
 * @var hv_rx_cnt::band
 * Please Place Description here.
 * @var hv_rx_cnt::type
 * Please Place Description here.
 * @var hv_rx_cnt::val
 * Please Place Description here.
 * @var hv_rx_cnt::subtype
 * Please Place Description here.
 * @var hv_rx_cnt::bssid
 * Please Place Description here.
 * @var hv_rx_cnt::rate
 * Please Place Description here.
 * @var hv_rx_cnt::gi_ltf
 * Please Place Description here.
 * @var hv_rx_cnt::ru
 * Please Place Description here.
 * @var hv_rx_cnt::msk
 * Please Place Description here.
 */
struct hv_rx_cnt {
#define MAC_HV_RX_CNT_R 0
#define MAC_HV_RX_CNT_W 1
#define MAC_HV_RX_CNT_RST 2
	u8 op;
#define MAC_HV_RX_CNT_NUM 48
	u8 idx;
	u8 band;
	u8 type;
	u16 val;
	u8 subtype;
	u8 bssid;
	u16 rate;
	u8 gi_ltf;
	u8 ru;
#define MAC_HV_RX_CNT_MSK_FC BIT(0)
#define MAC_HV_RX_CNT_MSK_BSSID BIT(1)
#define MAC_HV_RX_CNT_MSK_RATE BIT(2)
#define MAC_HV_RX_CNT_MSK_RU BIT(3)
	u8 msk;
};

/**
 * @struct hv_txpkt_info
 * @brief hv_txpkt_info
 * hv_txpkt_info is used for non-normal flow WD fields.
 * Especially for verification WD fields
 *
 */
struct hv_txpkt_info {
	u8 null_0;
	u8 null_1;
	u8 tri_frame;
	u8 ht_data_snd;
	u8 chk_en;
	u16 ndpa_dur;
};

/**
 * @struct hv_ax_ops
 * @brief hv_ax_ops
 *
 * @var hv_ax_ops::tx_post_desc
 * Please Place Description here.
 * @var hv_ax_ops::get_ppdu
 * Please Place Description here.
 * @var hv_ax_ops::chk_ps_dfs
 * Please Place Description here.
 * @var hv_ax_ops::chk_ps_ppdu
 * Please Place Description here.
 * @var hv_ax_ops::chk_ps_ch_info
 * Please Place Description here.
 * @var hv_ax_ops::phy_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_bmp_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_len_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_dl_rugrp_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_muru_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_quota_cfg
 * Please Place Description here.
 * @var hv_ax_ops::sta_link_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_dl_rpt_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_ul_rpt_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_query_search
 * Please Place Description here.
 * @var hv_ax_ops::ss_rpt_path_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_set_bsr_thold
 * Please Place Description here.
 * @var hv_ax_ops::ss_dlru_search_mode
 * Please Place Description here.
 * @var hv_ax_ops::ss_set_delay_tx
 * Please Place Description here.
 * @var hv_ax_ops::sta_dl_mutbl_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_dlmu_search_mode
 * Please Place Description here.
 * @var hv_ax_ops::ss_quota_mode
 * Please Place Description here.
 * @var hv_ax_ops::get_dbg_port_info
 * Please Place Description here.
 * @var hv_ax_ops::get_dle_dfi_info
 * Please Place Description here.
 * @var hv_ax_ops::ss_wmm_tbl_cfg
 * Please Place Description here.
 * @var hv_ax_ops::ss_wmm_sta_move
 * Please Place Description here.
 * @var hv_ax_ops::ss_set_wmm_bmp
 * Please Place Description here.
 * @var hv_ax_ops::cfg_btc_dbg_port
 * Please Place Description here.
 * @var hv_ax_ops::en_btc_rtk_mode
 * Please Place Description here.
 * @var hv_ax_ops::set_ctrl_frame_cnt
 * Please Place Description here.
 * @var hv_ax_ops::set_rx_cnt
 * Please Place Description here.
 * @var hv_ax_ops::set_freerun_cfg
 * Please Place Description here.
 * @var hv_ax_ops::get_freerun_info
 * Please Place Description here.
 * @var hv_ax_ops::set_lifetime_mg2
 * Please Place Description here.
 * @var hv_ax_ops::get_lifetime_mg2
 * Please Place Description here.
 * @var hv_ax_ops::ptn_h2c_common
 * Please Place Description here.
 * @var hv_ax_ops::get_mac_err_isr
 * Please Place Description here.
 * @var hv_ax_ops::get_gpio_status
 * Please Place Description here.
 * @var hv_ax_ops::get_gpio_val
 * Please Place Description here.
 */
struct hv_ax_ops {
	u32 (*tx_post_desc)(struct mac_ax_adapter *adapter,
			    struct hv_aggregator_t *agg);
	u32 (*get_ppdu)(struct mac_ax_adapter *adapter, enum mac_ax_band band);
	u32 (*chk_ps_dfs)(struct mac_ax_adapter *adapter, u8 *data, u32 len);
	u32 (*chk_ps_ppdu)(struct mac_ax_adapter *adapter, u8 *data, u32 len);
	u32 (*chk_ps_ch_info)(struct mac_ax_adapter *adapter, u8 *buf, u32 len);
	u32 (*phy_cfg)(struct mac_ax_adapter *adapter);
	u32 (*sta_bmp_cfg)(struct mac_ax_adapter *adapter,
			   struct hv_ax_sta_bmp_ctrl *ctrl,
			   enum hv_ax_sta_bmp_cfg cfg);
	u32 (*sta_len_cfg)(struct mac_ax_adapter *adapter,
			   struct hv_ax_sta_len *len,
			   enum hv_ax_sta_len_cfg cfg);
	u32 (*sta_dl_rugrp_cfg)(struct mac_ax_adapter *adapter,
				struct hv_ax_sta_dl_rugrp_ctrl *rugrp,
				enum hv_ax_sta_muru_cfg cfg);
	u32 (*sta_muru_cfg)(struct mac_ax_adapter *adapter,
			    struct hv_ax_sta_muru_ctrl *muru,
			    enum hv_ax_sta_muru_cfg cfg);
	u32 (*sta_quota_cfg)(struct mac_ax_adapter *adapter,
			     struct hv_ax_sta_quota *quota,
			     enum hv_ax_sta_quota_cfg cfg);
	u32 (*sta_link_cfg)(struct mac_ax_adapter *adapter,
			    struct hv_ax_ss_link_info *link,
			    enum hv_ax_ss_link_cfg cfg);
	void (*ss_dl_rpt_cfg)(struct mac_ax_adapter *adapter,
			      struct hv_ax_ss_dl_rpt_info *info,
			      enum hv_ax_ss_rpt_cfg cfg);
	void (*ss_ul_rpt_cfg)(struct mac_ax_adapter *adapter,
			      struct hv_ax_ss_ul_rpt_info *info,
			      enum hv_ax_ss_rpt_cfg cfg);
	u32 (*ss_query_search)(struct mac_ax_adapter *adapter,
			       struct hv_ax_ss_search_info *info);
	void (*ss_rpt_path_cfg)(struct mac_ax_adapter *adapter,
				enum hv_ax_ss_rpt_path_cfg cfg);
	void (*ss_set_bsr_thold)(struct mac_ax_adapter *adapter, u16 thold_0,
				 u16 thold_1);
	void (*ss_dlru_search_mode)(struct mac_ax_adapter *adapter,
				    enum hv_ax_ss_dlru_search_mode mode);
	void (*ss_set_delay_tx)(struct mac_ax_adapter *adapter,
				struct hv_ax_ss_delay_tx_info *info);
	u32 (*sta_dl_mutbl_cfg)(struct mac_ax_adapter *adapter,
				struct hv_ax_sta_dl_mutbl_ctrl *mutbl,
				enum hv_ax_sta_muru_cfg cfg);
	void (*ss_dlmu_search_mode)(struct mac_ax_adapter *adapter, u8 mode,
				    u8 score_thr);
	u32 (*ss_quota_mode)(struct mac_ax_adapter *adapter,
			     struct hv_ax_ss_quota_mode_ctrl *ctrl,
			     enum hv_ax_ss_quota_mode_cfg cfg);
	u32 (*get_dbg_port_info)(struct mac_ax_adapter *adapter,
				 struct hv_dbg_port *dbg);
	u32 (*get_dle_dfi_info)(struct mac_ax_adapter *adapter,
				struct hv_dbg_port *dbg);
	void (*ss_wmm_tbl_cfg)(struct mac_ax_adapter *adapter,
			       struct mac_ax_ss_wmm_tbl_ctrl *ctrl,
			       enum hv_ax_ss_wmm_tbl_cfg cfg);
	u32 (*ss_wmm_sta_move)(struct mac_ax_adapter *adapter,
			       enum hv_ax_ss_wmm src_wmm,
			       enum mac_ax_ss_wmm_tbl dst_link);
	u32 (*ss_set_wmm_bmp)(struct mac_ax_adapter *adapter, u8 wmm, u8 macid);
	u32 (*cfg_btc_dbg_port)(struct mac_ax_adapter *adapter);
	u32 (*en_btc_rtk_mode)(struct mac_ax_adapter *adapter);
	u32 (*set_ctrl_frame_cnt)(struct mac_ax_adapter *adapter,
				  struct hv_ctrl_frame_cnt *ctrl);
	u32 (*set_rx_cnt)(struct mac_ax_adapter *adapter,
			  struct hv_rx_cnt *cnt);
	u32 (*set_freerun_cfg)(struct mac_ax_adapter *adapter,
			       enum hv_ax_freerun_cfg cfg);
	u32 (*get_freerun_info)(struct mac_ax_adapter *adapter, u32 *cnt_low,
				u32 *cnt_high);
	u32 (*set_lifetime_mg2)(struct mac_ax_adapter *adapter,
				struct hv_ax_lifetime_mg2_cfg *cfg);
	u32 (*get_lifetime_mg2)(struct mac_ax_adapter *adapter,
				struct hv_ax_lifetime_mg2_cfg *cfg);
	u32 (*ptn_h2c_common)(struct mac_ax_adapter *adapter,
			      struct rtw_g6_h2c_hdr *hdr,
			      u32 *pvalue);
	u32 (*get_mac_err_isr)(struct mac_ax_adapter *adapter);
	u32 (*get_gpio_status)(struct mac_ax_adapter *adapter,
			       enum rtw_mac_gfunc *func, u8 gpio);
	u32 (*get_gpio_val)(struct mac_ax_adapter *adapter, u8 gpio, u8 *val);
	u32 (*get_rxd_drv_info_unit)(struct mac_ax_adapter *adapter);
	u32 (*get_ampdu_cfg)(struct mac_ax_adapter *adapter,
			     struct mac_ax_ampdu_cfg *cfg);
	u32 (*get_edca_param)(struct mac_ax_adapter *adapter,
			      struct mac_ax_edca_param *param);
	u32 (*get_muedca_param)(struct mac_ax_adapter *adapter,
				struct mac_ax_muedca_param *param);
	u32 (*get_muedca_timer)(struct mac_ax_adapter *adapter,
				struct mac_ax_muedca_timer *timer);
	u32 (*get_muedca_ctrl)(struct mac_ax_adapter *adapter,
			       struct mac_ax_muedca_cfg *cfg);
	u32 (*get_ch_stat_cnt)(struct mac_ax_adapter *adapter,
			       struct mac_ax_ch_stat_cnt *cnt);
	u32 (*get_lifetime_cfg)(struct mac_ax_adapter *adapter,
				struct mac_ax_lifetime_cfg *cfg);
	u32 (*get_hw_edcca_param)(struct mac_ax_adapter *adapter,
				  struct mac_ax_edcca_param *param);
	u32 (*set_ofld_cfg)(struct mac_ax_adapter *adapter,
			    struct mac_ax_ofld_cfg *param);
	u32 (*get_macid_pause)(struct mac_ax_adapter *adapter,
			       struct mac_ax_macid_pause_cfg *cfg);
	u32 (*get_hw_sch_tx_en)(struct mac_ax_adapter *adapter,
				struct mac_ax_sch_tx_en_cfg *cfg);
	u32 (*set_hw_muedca_timer)(struct mac_ax_adapter *adapter,
				   struct mac_ax_muedca_timer *timer);
	u32 (*set_hw_ch_busy_cnt)(struct mac_ax_adapter *adapter,
				  struct mac_ax_ch_busy_cnt_cfg *cfg);
	u32 (*run_pwr_seq)(struct mac_ax_adapter *adapter,
			   enum hv_ax_pwr_seq_sel sel);
	u32 (*read_lte)(struct mac_ax_adapter *adapter,
			const u32 offset, u32 *val);
	u32 (*write_lte)(struct mac_ax_adapter *adapter,
			 const u32 offset, u32 val);
	u32 (*c2h_log_test)(struct mac_ax_adapter *adapter, u32 len);
};

#endif

/**
 * @brief mac_plat_auto_test
 *
 * @param *adapter
 * @param *info
 * @param test_module
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_plat_auto_test(struct mac_ax_adapter *adapter,
		       struct mac_ax_plat_auto_test *info,
		       enum mac_ax_plat_module test_module);

/**
 * @brief mac_long_run_test
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_long_run_test(struct mac_ax_adapter *adapter,
		      struct mac_ax_mac_test *info);

/**
 * @brief mac_flash_burn_test
 *
 * @param *adapter
 * @param *fw
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_flash_burn_test(struct mac_ax_adapter *adapter,
			u8 *fw, u32 len);
