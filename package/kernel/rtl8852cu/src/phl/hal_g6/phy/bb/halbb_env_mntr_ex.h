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
#ifndef __HALBB_ENV_MNTR_EX_H__
#define __HALBB_ENV_MNTR_EX_H__

/*--------------------------[Define] ---------------------------------------*/

/*NHM*/
#define	NHM_TH_NUM		11	/*threshold number of NHM*/
#define	NHM_RPT_NUM		12
/*IFS-CLM*/
#define	IFS_CLM_NUM		4
#define	BE_IFS_CLM_NUM		5

/*--------------------------[Enum]------------------------------------------*/
enum mntr_result_lv {
	CCX_FAIL		= 0,
	NHM_SUCCESS		= BIT(0),
	CLM_SUCCESS		= BIT(1),
	IFS_CLM_SUCCESS		= BIT(2),
	EDCCA_CLM_SUCCESS		= BIT(3),
	CCX_SUCCESS		= 0xf, /*exclude FAHM*/
};

enum halbb_racing_lv {
	RAC_RELEASE		= 0,
	RAC_LV_1		= 1,	/* Low Priority function */
	RAC_LV_2		= 2,	/* Middle Priority function */
	RAC_LV_3		= 3,	/* High priority function (ex: Check hang function) */
	RAC_LV_4		= 4,	/* Debug function (the highest priority) */
	RAC_MAX_NUM		= 5
};

enum ccx_edcca_opt_sc_idx {
	CCX_EDCCA_P0	= 0,	/*p20*/
	CCX_EDCCA_S1	= 1,	/*s20*/
	CCX_EDCCA_S2	= 2,	/*s40, opposite of p20*/
	CCX_EDCCA_S3	= 3,	/*s40, opposite of s20*/
};

enum nhm_opt_cca_all {
	NHM_EXCLUDE_CCA		= 0,
	NHM_INCLUDE_CCA		= 1,
	NHM_CCA_INIT
};

enum nhm_opt_nav_en {
	NHM_NAV_EN_DISABLED	= 0,
	NHM_NAV_EN_ENABLED	= 1,
	NHM_NAV_EN_INIT
};

enum nhm_opt_rssi_th_en {
	NHM_RSSI_TH_EN_DISABLED	= 0,
	NHM_RSSI_TH_EN_ENABLED	= 1,
	NHM_RSSI_TH_EN_INIT
};

enum clm_opt_input {
	CLM_CCA_P20		= 0,
	CLM_CCA_S20		= 1,
	CLM_CCA_S40		= 2,
	CLM_CCA_S80		= 3,
	CLM_FROM_DBG		= 4,
	CLM_TXON_CCA		= 5,
	CLM_CCA_S80_S40_S20	= 6,
	CLM_CCA_S80_S40_S20_P20	= 7,
	CLM_CCA_INIT
};

enum be_clm_opt_input {
	BE_CLM_CCA_P20			= 0,
	BE_CLM_CCA_S20			= 1,
	BE_CLM_CCA_S40			= 2,
	BE_CLM_CCA_S80			= 3,
	BE_CLM_CCA_S160			= 4,
	BE_CLM_FROM_DBG			= 5,
	BE_CLM_TXON_CCA			= 6,
	BE_CLM_CCA_S160_S80_S40_S20	= 7,
	BE_CLM_CCA_S160_S80_S40_S20_P20	= 8,
	BE_CLM_CCA_INIT
};

enum clm_opt_nav_en {
	CLM_NAV_EN_DISABLED	= 0,
	CLM_NAV_EN_ENABLED	= 1,
	CLM_NAV_EN_INIT
};

enum clm_opt_rssi_th_en {
	CLM_RSSI_TH_EN_DISABLED	= 0,
	CLM_RSSI_TH_EN_ENABLED	= 1,
	CLM_RSSI_TH_EN_INIT
};

enum nhm_application {
	NHM_INIT		= 0,
	NHM_BACKGROUND		= 1, /*IEEE 11K for background*/
	NHM_ACS			= 2,
	NHM_FW_CTRL		= 3,
	NHM_DBG_11K		= 4, /*IEEE 11K for dbg cmd*/
	NHM_DBG_MANUAL		= 5 /*nhm_th[0] & th_ofst is manual*/
};

enum clm_application {
	CLM_INIT		= 0,
	CLM_BACKGROUND		= 1,/*default*/
	CLM_ACS			= 2,
	CLM_FW_CTRL		= 3,
	CLM_DBG			= 4
};

enum ifs_clm_application {
	IFS_CLM_INIT		= 0,
	IFS_CLM_BACKGROUND	= 1,/*default*/
	IFS_CLM_ACS		= 2,
	IFS_CLM_FW_CTRL		= 3,
	IFS_CLM_DBG		= 4,
	IFS_CLM_DBG_MANUAL	= 5
};

enum edcca_clm_application {
	EDCCA_CLM_INIT		= 0,
	EDCCA_CLM_BACKGROUND	= 1,/*default*/
	EDCCA_CLM_ACS		= 2,
	EDCCA_CLM_FW_CTRL	= 3,
	EDCCA_CLM_DBG		= 4
};

/*--------------------------[Structure]-------------------------------------*/
struct bb_info;

struct env_trig_rpt {
	u8			ccx_rpt_stamp;
};

struct env_mntr_rpt {
	u8			ccx_rpt_stamp;
	u8			ccx_rpt_result;
	u8			clm_ratio; /*percent*/
	u16			clm_result; /*sample cnt*/
	u8			nhm_rpt[NHM_RPT_NUM]; /*percent*/
	u8			nhm_ratio; /*percent*/
	u8			nhm_tx_ratio; /*percent*/
	u8			nhm_cca_ratio; /*percent*/
	u8			nhm_idle_ratio; /*percent*/
	u16			nhm_tx_cnt; /*sample cnt*/
	u16			nhm_cca_cnt; /*sample cnt*/
	u16			nhm_idle_cnt; /*sample cnt*/
	u8			nhm_pwr; /*110+x(dBm), unit = 1dB*/
	u8			nhm_pwr_0p5; /*110+x/2(dBm), unit = 0.5dB*/
	u8			ifs_clm_tx_ratio; /*percent*/
	u8			ifs_clm_edcca_excl_cca_ratio; /*percent*/
	u8			ifs_clm_cck_fa_ratio; /*percent*/
	u8			ifs_clm_ofdm_fa_ratio; /*percent*/
	u8			ifs_clm_cck_cca_excl_fa_ratio; /*percent*/
	u8			ifs_clm_ofdm_cca_excl_fa_ratio; /*percent*/
	u16			ifs_clm_cck_fa_permil; /*permil*/
	u16			ifs_clm_ofdm_fa_permil; /*permil*/
	u16			ifs_clm_total_ifs; /*cnt*/
	u16			ifs_clm_his[BE_IFS_CLM_NUM]; /*cnt*/
	u32			ifs_clm_ifs_avg[BE_IFS_CLM_NUM]; /*us*/
	u32			ifs_clm_cca_avg[BE_IFS_CLM_NUM]; /*us*/
	u8			edcca_clm_ratio; /*percent*/
};

struct ccx_para_info {
	enum halbb_racing_lv		rac_lv;
	u16				mntr_time;	/*0~2097ms*/
	enum ccx_edcca_opt_sc_idx	ccx_edcca_opt_sc_idx;
	enum clm_application		clm_app;
	enum clm_opt_input		clm_input_opt;
	enum clm_opt_nav_en		clm_nav_en;
	enum clm_opt_rssi_th_en		clm_rssi_th_en;
	u8				clm_rssi_th;
	enum nhm_application		nhm_app;
	u8				nhm_manual_th_ofst;
	u8				nhm_manual_th0;	/*dbg manual mode*/
	enum nhm_opt_cca_all		nhm_incld_cca; /*Include CCA*/
	enum nhm_opt_nav_en		nhm_nav_en;
	enum nhm_opt_rssi_th_en		nhm_rssi_th_en;
	u8				nhm_rssi_th;
	enum ifs_clm_application	ifs_clm_app;
	u32				ifs_clm_manual_th_times;
	u32				ifs_clm_manual_th0;/*us*/
	enum edcca_clm_application	edcca_clm_app;
};

/*--------------------------[Prptotype]-------------------------------------*/
void halbb_env_mntr_get_bg_result(struct bb_info *bb,
				  struct env_mntr_rpt *bg_rpt,
				  enum phl_phy_idx phy_idx);
void halbb_env_mntr_get_bg_setting(struct bb_info *bb,
				   struct ccx_para_info *bg_para,
				   enum phl_phy_idx phy_idx);
u8 halbb_env_mntr_trigger(struct bb_info *bb, struct ccx_para_info *para,
			  struct env_trig_rpt *trig_rpt, enum phl_phy_idx phy_idx);
u8 halbb_env_mntr_result(struct bb_info *bb, struct env_mntr_rpt *rpt, enum phl_phy_idx phy_idx);

u8 halbb_env_mntr_get_802_11_k_rsni(struct bb_info *bb, s8 rcpi, s8 anpi);

#endif

