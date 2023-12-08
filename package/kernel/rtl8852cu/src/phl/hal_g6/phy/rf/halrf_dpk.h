/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef __HALRF_DPK_H__
#define __HALRF_DPK_H__

/*@--------------------------Define Parameters-------------------------------*/
#define AVG_THERMAL_NUM_DPK 8
#define THERMAL_DPK_AVG_NUM 1
#define DPK_BKUP_NUM 2

enum dpk_id {
	LBK_RXIQK	= 0x06,
	SYNC		= 0x10,
	MDPK_IDL	= 0x11,
	MDPK_MPA	= 0x12,
	GAIN_LOSS	= 0x13,
	GAIN_CAL	= 0x14,
	DPK_RXAGC	= 0x15,
	KIP_PRESET	= 0x16,
	KIP_RESTORE	= 0x17,
	DPK_TXAGC	= 0x19,
	D_KIP_PRESET	= 0x28,
	D_TXAGC		= 0x29,
	D_RXAGC		= 0x2a,
	D_SYNC		= 0x2b,
	D_GAIN_LOSS	= 0x2c,
	D_MDPK_IDL	= 0x2d,
	D_MDPK_LDL	= 0x2e,
	D_GAIN_NORM	= 0x2f,
	D_KIP_THERMAL	= 0x30,
	D_KIP_RESTORE	= 0x31
};

struct dpk_bkup_para {
	enum band_type	band;		/* 2.4G,5G,6G*/
	enum channel_width	bw;
	u8	ch;
	u8	path_ok;
	u8	txagc_dpk;		/*txagc@dpk with path*/	
	u8	ther_dpk;		/*thermal@dpk with path*/	
	//u8	trk_idx_dpk;		/*track_idx@dpk with path*/	
	//u8	ther_tssi;		/*thermal@tssi with path*/
	u8	gs;
	u16	pwsf;
};

/*@---------------------------End Define Parameters---------------------------*/

struct halrf_dpk_info {

	bool	is_dpk_enable;
	bool	is_dpk_track_en;
	bool	is_dpk_reload_en;
	bool	is_dpk_pwr_unlmt;
	u8	dpk_gs[2];			/*PHY*/
	u8	ther_avg[KPATH][AVG_THERMAL_NUM_DPK];	/*path*/
	u8	pre_pwsf[KPATH];
	u8	ther_avg_idx;
	u32	dpk_cal_cnt;
	u32	dpk_ok_cnt;
	u32	dpk_reload_cnt;
	u16	dc_i[KPATH][DPK_BKUP_NUM];		/*path*/
	u16	dc_q[KPATH][DPK_BKUP_NUM];		/*path*/
	u8	corr_val[KPATH][DPK_BKUP_NUM];		/*path*/
	u8	corr_idx[KPATH][DPK_BKUP_NUM];		/*path*/
	u8	cur_idx[KPATH];				/*path*/
	u8	cur_k_set;
	u32	dpk_time;
	u8	max_dpk_txagc[KPATH];
	u8	ov_flag[KPATH];				/*path*/
	u32	dpk_sync[KPATH];			/*path*/
	u8	rxbb_ov[KPATH];				/*path*/
	u32	rek_cnt[KPATH][2];			/*path/is_first*/
	u32	rc_mtx[KPATH][434][2];			/*path/addr/rpt*/
	u32	rx_sram[KPATH][512];			/*path/addr/rpt*/
	u32	dpk_dciq[KPATH];
	u32	dpk_pas[KPATH][32];
	u32	dpk_coef[KPATH][KPATH][28];		/*path/is_first/addr*/
	s16	dpk_coef_i[2][20];			/*is_first/addr*/
	s16	dpk_coef_q[2][20];			/*is_first/addr*/
	u8	c_chk[KPATH];
	u8	rek_chk[KPATH][2][5];			/*path/is_first/rek_cnt*/
	u32	dpk_rxiqc[KPATH];			/*path*/
	u8	dpk_order[KPATH];			/*path*/
#if 0
	u8	ov_pa[KPATH];				/*path*/
	u32	dpk_pwr[KPATH];
	u32	dpk_frac[KPATH][9];
	u32	dpk_sync_cfg[KPATH];			/*path*/
	u32	tpg_setting[KPATH];			/*path*/
	u32	mdpk_filter_set[KPATH];			/*path*/
	u32	dpk_sync1[KPATH];			/*path*/
	u32	dpk_dciq1[KPATH];
	u32	dpk_pwr1[KPATH];
	u32	dpk_sync_cfg1[KPATH];			/*path*/
	u32	dpk_frac1[KPATH][9];
	u32	tpg_setting1[KPATH];			/*path*/
	u32	mdpk_filter_set1[KPATH];		/*path*/
#endif

	struct dpk_bkup_para bp[KPATH][DPK_BKUP_NUM];	/*path/index*/
};

#endif /*__HALRF_DPK_H__*/

