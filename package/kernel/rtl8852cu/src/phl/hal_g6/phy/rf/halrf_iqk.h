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
#ifndef _HALRF_IQK_H_
#define _HALRF_IQK_H_

/*@--------------------------Define Parameters-------------------------------*/
#define TXIQK 0
#define RXIQK 1
#define RXIQK1 2
#define RXIQK2 3
#define NBTXK 4
#define NBRXK 5
//#define NUM 2

#define ID_TXAGC 0x0
#define ID_FLoK_coarse 0x1
#define ID_FLoK_fine 0x2
#define ID_TXK 0x3
#define ID_RXAGC 0x4
#define ID_RXK 0x5
#define ID_NBTXK 0x6
#define ID_NBRXK 0x7
#define ID_FLOK_vbuffer 0x8
#define ID_A_FLoK_coarse 0x9
#define ID_G_FLoK_coarse 0xa
#define ID_A_FLoK_fine 0xb
#define ID_G_FLoK_fine 0xc
#define ID_TX_PAD_GainGapK 0xe
#define ID_TX_PA_GainGapK 0xf
#define ID_IQK_Restore 0x10

/*@-----------------------End Define Parameters-----------------------*/

struct halrf_iqk_info {
	bool lok_cor_fail[2][NUM]; /*channel/path */
	bool lok_fin_fail[2][NUM]; /*channel/path */
	bool iqk_tx_fail[2][NUM]; /*channel/path */
	bool iqk_rx_fail[2][NUM]; /*channel/path */
	u32 iqk_cnt;
	u32 iqk_fail_cnt;
	bool segment_iqk;
	bool is_iqk_enable;
	bool is_iqk_init;
	bool is_reload;
	u32 iqk_channel[2];
	u8 iqk_band[NUM];
	u8 iqk_ch[NUM];
	u8 iqk_bw[NUM];
	u8 kcount;
	u8 iqk_times;
	u8 rxiqk_step;
	u8 iqk_step;
	u8 version;
	u32 lok_idac[2][NUM];
	u32 lok_vbuf[2][NUM];
	u32 iqc_gain;
	u32 rftxgain[NUM];
	u32 rfrxgain[NUM];
	u32 nb_txcfir[NUM];
	u32 nb_rxcfir[NUM];
	u32 rximr[NUM];
	u32 syn1to2;
	u32 bp_txkresult[2];
	u32 bp_rxkresult[2];
	u32 bp_lokresult[2];
	u32 bp_iqkenable[2];
	u32 reload_cnt;
	bool is_wb_txiqk[2];
	bool is_wb_rxiqk[2];
	bool is_nbiqk;
	bool iqk_fft_en;
	bool iqk_xym_en;
	bool iqk_sram_en;
	bool iqk_cfir_en;
	u8 ther_avg[2][8];	/*path*/
	u8 ther_avg_idx;
	u8 thermal[2];
	bool thermal_rek_en;
	u8 iqk_mcc_ch[2][NUM];
	u8 iqk_table_idx[NUM];
	bool is_fw_iqk;
	u32	time;
	u32 lok_0x58[2];
	u32 lok_0x5c[2];
	u32 lok_0x7c[2];	
};

void halrf_iqk_init(struct rf_info *rf);
void halrf_iqk(struct rf_info *rf, enum phl_phy_idx phy_idx, bool force);
u32 halrf_get_iqk_ver(struct rf_info *rf);
void halrf_iqk_toneleakage(void *rf_void, u8 path);
void halrf_nbiqk_enable(void *rf_void, bool iqk_nbiqk_en);
void halrf_iqk_tx_bypass(void *rf_void, u8 path);
void halrf_iqk_rx_bypass(void *rf_void, u8 path);
void halrf_iqk_lok_bypass(void *rf_void, u8 path);
void halrf_iqk_xym_enable(void *rf_void, bool iqk_xym_en);
void halrf_iqk_fft_enable(void *rf_void, bool iqk_fft_en);
void halrf_iqk_cfir_enable(void *rf_void, bool iqk_cfir_en);
void halrf_iqk_sram_enable(void *rf_void, bool iqk_sram_en);
void halrf_iqk_reload(void *rf_void, u8 path);
void halrf_iqk_dbcc(void *rf_void, u8 path);
u8 halrf_iqk_get_mcc_ch0(void *rf_void);
u8 halrf_iqk_get_mcc_ch1(void *rf_void);
void halrf_enable_fw_iqk(void *rf_void, bool is_fw_iqk);
u8 halrf_iqk_get_rxevm(void *rf_void);
u32 halrf_iqk_get_rximr(void *rf_void, u8 path, u32 idx);
bool halrf_check_fwiqk_done(struct rf_info *rf);




#endif
