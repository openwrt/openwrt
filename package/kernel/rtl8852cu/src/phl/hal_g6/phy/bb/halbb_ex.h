/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#ifndef __HALBB_EX_H__
#define __HALBB_EX_H__
#include "halbb_ic_hw_info.h"

/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

enum halbb_pause_type {
	HALBB_PAUSE		= 1,	/*Pause & Set new value*/
	HALBB_PAUSE_NO_SET	= 2,	/*Pause & Stay in current value*/
	HALBB_RESUME		= 3,
	HALBB_RESUME_NO_RECOVERY = 4,
};

enum halbb_pause_lv_type {
	HALBB_PAUSE_RELEASE	= -1,
	HALBB_PAUSE_LV_0	= 0,	/* @Low Priority function */
	HALBB_PAUSE_LV_1	= 1,	/* @Middle Priority function */
	HALBB_PAUSE_LV_2	= 2,	/* @High priority function (ex: Check hang function) */
	HALBB_PAUSE_LV_3	= 3,	/* @Debug function (the highest priority) */
	HALBB_PAUSE_MAX_NUM	= 4
};

enum halbb_pause_rpt {
	PAUSE_FAIL		= 0,
	PAUSE_SUCCESS		= 1
};

/*---[BB Components]---*/
enum habb_fun_t {
	F_RA			= 0,
	F_FA_CNT		= 1,
	HALBB_FUN_RSVD_2	= 2,
	F_DFS			= 3,
	F_EDCCA			= 4,
	F_ENV_MNTR		= 5,
	F_CFO_TRK		= 6,
	F_PWR_CTRL		= 7,
	F_RUA_TBL		= 8,
	F_AUTO_DBG		= 9,
	F_ANT_DIV		= 10,
	F_DIG			= 11,
	F_PATH_DIV		= 12,
	F_UL_TB_CTRL	= 13,
	F_DCR			= 31,
	F_DEFAULT		= 0xff
};

enum bb_watchdog_mode_t {
	BB_WATCHDOG_NORMAL = 0,
	BB_WATCHDOG_LOW_IO = 1,
	BB_WATCHDOG_NON_IO = 2,
};

struct halbb_func_info {
	char name[16];
	u8 id;
};

static const struct halbb_func_info halbb_func_i[] = {
	{"ra", F_RA}, /*@do not move this element to other position*/
	{"fa_cnt", F_FA_CNT}, /*@do not move this element to other position*/
	{"rsvd2", HALBB_FUN_RSVD_2},
	{"dfs", F_DFS},
	{"edcca", F_EDCCA},
	{"env_mntr", F_ENV_MNTR},
	{"cfo_trk", F_CFO_TRK},
	{"pwr_ctrl", F_PWR_CTRL},
	{"rua_tbl", F_RUA_TBL},
	{"auto_dbg", F_AUTO_DBG},
	{"ant_div", F_ANT_DIV},
	{"dig", F_DIG},
	{"path_div", F_PATH_DIV},
	{"ul_tb", F_UL_TB_CTRL},
};

/*@=[HALBB supportability]=======================================*/
enum habb_supportability_t {
	BB_RA			= BIT(F_RA),
	BB_FA_CNT		= BIT(F_FA_CNT),
	BB_FUN_RSVD_2		= BIT(HALBB_FUN_RSVD_2),
	BB_DFS			= BIT(F_DFS),
	BB_EDCCA		= BIT(F_EDCCA),
	BB_ENVMNTR		= BIT(F_ENV_MNTR),
	BB_CFO_TRK		= BIT(F_CFO_TRK),
	BB_PWR_CTRL		= BIT(F_PWR_CTRL),
	BB_RUA_TBL		= BIT(F_RUA_TBL),
	BB_AUTO_DBG		= BIT(F_AUTO_DBG),
	BB_ANT_DIV		= BIT(F_ANT_DIV),
	BB_DIG			= BIT(F_DIG),
	BB_PATH_DIV		= BIT(F_PATH_DIV),
	BB_UL_TB_CTRL	= BIT(F_UL_TB_CTRL),
	BB_DCR			= BIT(F_DCR)
};

/*@=[HALBB Debug Component]=====================================*/
enum halbb_dbg_comp_t {
	/*=== [DM Part] ==========================*/
	DBG_RA			= BIT(F_RA),
	DBG_FA_CNT		= BIT(F_FA_CNT),
	DBG_HALBB_FUN_RSVD_2	= BIT(HALBB_FUN_RSVD_2),
	DBG_DFS			= BIT(F_DFS),
	DBG_EDCCA		= BIT(F_EDCCA),
	DBG_ENV_MNTR		= BIT(F_ENV_MNTR),
	DBG_CFO_TRK		= BIT(F_CFO_TRK),
	DBG_PWR_CTRL		= BIT(F_PWR_CTRL),
	DBG_RUA_TBL		= BIT(F_RUA_TBL),
	DBG_AUTO_DBG		= BIT(F_AUTO_DBG),
	DBG_ANT_DIV		= BIT(F_ANT_DIV),
	DBG_DIG			= BIT(F_DIG),
	DBG_PATH_DIV		= BIT(F_PATH_DIV),
	DBG_UL_TB_CTRL		= BIT(F_UL_TB_CTRL),
	/*=== [Non-DM Part] ======================*/
	DBG_BIT14		= BIT(14),
	DBG_BIT15		= BIT(15),
	DBG_BIT16		= BIT(16),
	DBG_BIT17		= BIT(17),
	DBG_SNIFFER		= BIT(18),
	DBG_CH_INFO		= BIT(19),
	DBG_PHY_STS		= BIT(20),
	DBG_CONNECT		= BIT(21),
	DBG_FW_INFO		= BIT(22),
	DBG_COMMON_FLOW		= BIT(23),
	DBG_IC_API		= BIT(24),
	DBG_DBG_API		= BIT(25),
	DBG_DBCC		= BIT(26),
	DBG_DM_SUMMARY		= BIT(27),
	DBG_PHY_CONFIG		= BIT(28),
	DBG_INIT		= BIT(29),
	DBG_CMN			= BIT(30),
	DBG_DCR			= BIT(F_DCR)
};
/*@--------------------------[Structure]-------------------------------------*/

#if 0
/*For development use only, and will move to "struct rtw_rssi_info" in near furture*/
struct bb_rssi_info{
	u8 rssi; /*avg RSSI among all RF path, dbm = RSSI - 110*/
	u16 rssi_acc; /*U(16,4) version of rssi*/
	u8 rssi_cck; /*instance value of CCK RSSI*/
	u8 rssi_ofdm;  /*instance value of OFDM RSSI*/
};
#endif

struct bb_sta_info {
	u8 sta_status_tmp;
};

/*@--------------------------[Prptotype]-------------------------------------*/

struct bb_info;
bool halbb_sta_info_init(struct bb_info *bb,
			 struct rtw_phl_stainfo_t *phl_sta_info);
bool halbb_sta_info_deinit(struct bb_info *bb,
			   struct rtw_phl_stainfo_t *phl_sta_info);
bool halbb_sta_info_add_entry(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_info);
bool halbb_sta_info_delete_entry(struct bb_info *bb,
				 struct rtw_phl_stainfo_t *phl_sta_info);
void halbb_media_status_update(struct bb_info *bb,
			       struct rtw_phl_stainfo_t *phl_sta_info,
			       bool is_connected);
void halbb_watchdog_reset(struct bb_info *bb);
void halbb_watchdog(struct bb_info *bb, enum bb_watchdog_mode_t mode,
		    enum phl_phy_idx phy_idx);
u8 halbb_wifi_event_notify(struct bb_info *bb, enum phl_msg_evt_id event, enum phl_phy_idx phy_idx);
void halbb_bb_cmd_notify(struct bb_info *bb, void *bb_cmd, enum phl_phy_idx phy_idx);
u8 halbb_pause_func(struct bb_info *bb, enum habb_fun_t pause_func,
		    enum halbb_pause_type pause_type,
		    enum halbb_pause_lv_type lv,
		    u8 val_lehgth,
		    u32 *val_buf, enum phl_phy_idx phy_idx);
#endif
