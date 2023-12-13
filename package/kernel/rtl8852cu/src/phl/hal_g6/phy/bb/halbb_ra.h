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
#ifndef _HALBB_RA_H_
#define _HALBB_RA_H_

/*@--------------------------[extern] ---------------------------------------*/
extern const u16 bb_phy_rate_table[LEGACY_RATE_NUM + HE_RATE_NUM_4SS];
/*@--------------------------[Define] ---------------------------------------*/
#define VHT_2_HE32_RATE(X) ((((X) << 3) + (X) + 4) >> 3) /*= Round(X * 1.125)*/
#define HE32_2_HE16_RATE(X) ((((X) << 3) + (X) + 4) >> 3) /*= Round(X * 1.125)*/
#define HE32_2_HE08_RATE(X) ((((X) << 4) + ((X) << 1) + (X) + 8) >> 4) /*= Round(X * 1.1875)*/

#define RAMASK_B	0x000000000000000f
#define RAMASK_AG	0x0000000000000ff0
#define RAMASK_BG	0x0000000000000ff5
#define RAMASK_HT_2G	0x00000ffffffff015
#define RAMASK_HT_5G	0x00000ffffffff010
#define RAMASK_VHT_2G	0x000ffffffffff015
#define RAMASK_VHT_5G	0x000ffffffffff010
#define RAMASK_HE_2G	0x0ffffffffffff015
#define RAMASK_HE_5G	0x0ffffffffffff010
#define RAMASK_EHT_2G_L	0xfffffffffffff015
#define RAMASK_EHT_5G_L	0xfffffffffffff010
#define RAMASK_EHT_H	0xfff

#define RAMASK_1SS_HT	0x00000000000fffff
#define RAMASK_2SS_HT	0x00000000ff0fffff
#define RAMASK_3SS_HT	0x00000ff0ff0fffff
#define RAMASK_4SS_HT	0x00ff0ff0ff0fffff

#define RAMASK_1SS_VHT	0x00000000003fffff
#define RAMASK_2SS_VHT	0x00000003ff3fffff
#define RAMASK_3SS_VHT	0x00003ff3ff3fffff
#define RAMASK_4SS_VHT	0x03ff3ff3ff3fffff

#define RAMASK_1SS_HE	0x0000000000ffffff
#define RAMASK_2SS_HE	0x0000000fffffffff
#define RAMASK_3SS_HE	0x0000ffffffffffff
#define RAMASK_4SS_HE	0x0fffffffffffffff

#define RAMASK_1SS_EHT	0x000000000fffffff
#define RAMASK_2SS_EHT	0x00000fffffffffff
#define RAMASK_3SS_EHT	0x0fffffffffffffff
#define RAMASK_4SS_EHT_L 0xffffffffffffffff
#define RAMASK_4SS_EHT_H 0xfff

#define MAX_ABG_RATE_NUM 12
#define MAX_NSS_VHT 4
#define MAX_NSS_HT 4
#define MAX_NSS_HE 4
#define MAX_CAP_EHT 4

#define STA_NUM_RSSI_CMD PHL_MAX_STA_NUM

#define MASKRATE_AX	0x01ff
#define MASKGILTF_AX	0x0e00

/* WiFi Support Mode */
#define CCK_SUPPORT 	BIT(0)
#define OFDM_SUPPORT	BIT(1)
#define HT_SUPPORT		BIT(2)
#define VHT_SUPPORT_TX	BIT(3)
#define HE_SUPPORT		BIT(4)
#define EHT_SUPPORT		BIT(5)

#define	RA_FLOOR_TABLE_SIZE	7
#define	RA_FLOOR_UP_GAP		3

#define MAX_RATE_HE 24
#define MAX_RATE_VHT 40  // 8198F is 4SS
#define MAX_RATE_HT 32

/*@--------------------------[Enum]------------------------------------------*/

/* rate_idx table used in FW*/
enum rate_table {
    RATE_CCK_1M        = 0x00,
    RATE_CCK_2M        = 0x01,
    RATE_CCK_5M        = 0x02,
    RATE_CCK_11M       = 0x03,
    RATE_OFDM_6M       = 0x04,
    RATE_OFDM_9M       = 0x05,
    RATE_OFDM_12M      = 0x06,
    RATE_OFDM_18M      = 0x07,
    RATE_OFDM_24M      = 0x08,
    RATE_OFDM_36M      = 0x09,
    RATE_OFDM_48M      = 0x0A,
    RATE_OFDM_54M      = 0x0B,
    RATE_HT_MCS0       = 0x0C,
    RATE_HT_MCS1       = 0x0D,
    RATE_HT_MCS2       = 0x0E,
    RATE_HT_MCS3       = 0x0F,
    RATE_HT_MCS4       = 0x10,
    RATE_HT_MCS5       = 0x11,
    RATE_HT_MCS6       = 0x12,
    RATE_HT_MCS7       = 0x13,
    RATE_HT_MCS8       = 0x14,
    RATE_HT_MCS9       = 0x15,
    RATE_HT_MCS10      = 0x16,
    RATE_HT_MCS11      = 0x17,
    RATE_HT_MCS12      = 0x18,
    RATE_HT_MCS13      = 0x19,
    RATE_HT_MCS14      = 0x1A,
    RATE_HT_MCS15      = 0x1B,
    RATE_HT_MCS16      = 0x1C,
    RATE_HT_MCS17      = 0x1D,
    RATE_HT_MCS18      = 0x1E,
    RATE_HT_MCS19      = 0x1F,
    RATE_HT_MCS20      = 0x20,
    RATE_HT_MCS21      = 0x21,
    RATE_HT_MCS22      = 0x22,
    RATE_HT_MCS23      = 0x23,
    RATE_HT_MCS24      = 0x24,
    RATE_HT_MCS25      = 0x25,
    RATE_HT_MCS26      = 0x26,
    RATE_HT_MCS27      = 0x27,
    RATE_HT_MCS28      = 0x28,
    RATE_HT_MCS29      = 0x29,
    RATE_HT_MCS30      = 0x2A,
    RATE_HT_MCS31      = 0x2B,
    RATE_VHT1SS_MCS0   = 0x2C,
    RATE_VHT1SS_MCS1   = 0x2D,
    RATE_VHT1SS_MCS2   = 0x2E,
    RATE_VHT1SS_MCS3   = 0x2F,
    RATE_VHT1SS_MCS4   = 0x30,
    RATE_VHT1SS_MCS5   = 0x31,
    RATE_VHT1SS_MCS6   = 0x32,
    RATE_VHT1SS_MCS7   = 0x33,
    RATE_VHT1SS_MCS8   = 0x34,
    RATE_VHT1SS_MCS9   = 0x35,
    RATE_VHT2SS_MCS0   = 0x36,
    RATE_VHT2SS_MCS1   = 0x37,
    RATE_VHT2SS_MCS2   = 0x38,
    RATE_VHT2SS_MCS3   = 0x39,
    RATE_VHT2SS_MCS4   = 0x3A,
    RATE_VHT2SS_MCS5   = 0x3B,
    RATE_VHT2SS_MCS6   = 0x3C,
    RATE_VHT2SS_MCS7   = 0x3D,
    RATE_VHT2SS_MCS8   = 0x3E,
    RATE_VHT2SS_MCS9   = 0x3F,
    RATE_VHT3SS_MCS0   = 0x40,
    RATE_VHT3SS_MCS1   = 0x41,
    RATE_VHT3SS_MCS2   = 0x42,
    RATE_VHT3SS_MCS3   = 0x43,
    RATE_VHT3SS_MCS4   = 0x44,
    RATE_VHT3SS_MCS5   = 0x45,
    RATE_VHT3SS_MCS6   = 0x46,
    RATE_VHT3SS_MCS7   = 0x47,
    RATE_VHT3SS_MCS8   = 0x48,
    RATE_VHT3SS_MCS9   = 0x49,
    RATE_VHT4SS_MCS0   = 0x4A,
    RATE_VHT4SS_MCS1   = 0x4B,
    RATE_VHT4SS_MCS2   = 0x4C,
    RATE_VHT4SS_MCS3   = 0x4D,
    RATE_VHT4SS_MCS4   = 0x4E,
    RATE_VHT4SS_MCS5   = 0x4F,
    RATE_VHT4SS_MCS6   = 0x50,
    RATE_VHT4SS_MCS7   = 0x51,
    RATE_VHT4SS_MCS8   = 0x52,
    RATE_VHT4SS_MCS9   = 0x53,
    // HE
    RATE_HE1SS_MCS0    = 0x54,
    RATE_HE1SS_MCS1    = 0x55,
    RATE_HE1SS_MCS2    = 0x56,
    RATE_HE1SS_MCS3    = 0x57,
    RATE_HE1SS_MCS4    = 0x58,
    RATE_HE1SS_MCS5    = 0x59,
    RATE_HE1SS_MCS6    = 0x5A,
    RATE_HE1SS_MCS7    = 0x5B,
    RATE_HE1SS_MCS8    = 0x5C,
    RATE_HE1SS_MCS9    = 0x5D,
    RATE_HE1SS_MCS10   = 0x5E,
    RATE_HE1SS_MCS11   = 0x5F,
    RATE_HE2SS_MCS0    = 0x60,
    RATE_HE2SS_MCS1    = 0x61,
    RATE_HE2SS_MCS2    = 0x62,
    RATE_HE2SS_MCS3    = 0x63,
    RATE_HE2SS_MCS4    = 0x64,
    RATE_HE2SS_MCS5    = 0x65,
    RATE_HE2SS_MCS6    = 0x66,
    RATE_HE2SS_MCS7    = 0x67,
    RATE_HE2SS_MCS8    = 0x68,
    RATE_HE2SS_MCS9    = 0x69,
    RATE_HE2SS_MCS10   = 0x6A,
    RATE_HE2SS_MCS11   = 0x6B,
};

enum spatial_stream_num {
	RA_1SS_MODE	= 0,
	RA_2SS_MODE	= 1,
	RA_3SS_MODE	= 2,
	RA_4SS_MODE	= 3
};

enum wifi_mode {
	RA_CCK		= 0,
	RA_non_ht	= 1,
	RA_HT		= 2,
	RA_VHT		= 3,
	RA_HE		= 4,
	RA_EHT		= 5
};

enum mu_cmd_type {
	MU_ADD_ENTRY	= 0,
	MU_DEL_ENTRY	= 1,
	MU_DBG_CTRL	=2,
};

enum  bb_opt_gi_ltf {
	BB_OPT_GILTF_4XHE08 = 0,
	BB_OPT_GILTF_1XHE08 = 1
};


/*@--------------------------[Structure]-------------------------------------*/

struct bb_rate_info {
	u16 rate_idx_all;
	u16 rate_idx;
	enum rtw_gi_ltf gi_ltf;
	enum bb_mode_type mode; /*0:legacy, 1:HT, 2*/
	enum channel_width bw;
	u8 ss;
	u8 idx;
	u8 fw_rate_idx;
};

union bb_h2c_ra_cmn_info {
    u32 val[6];
    struct bb_h2c_ra_cfg_info bb_h2c_ra_info;
    struct bb_h2c_ra_cfg_info_wifi7 bb_h2c_ra_info_wifi7;
};

struct bb_ra_info {
	/* Config move to phl_sta_info*/
	union bb_h2c_ra_cmn_info ra_cfg;
	u8 cal_giltf;
	/* Ctrl */
	u8 drv_ractrl;
	bool fixed_rate_en;
	u8 fixed_rate; /* 7bit rate */
	u8 fixed_rat_md; /* 2bit rate_mode */
	u8 fixed_giltf; /*  3bit giltf */
	u8 fixed_bw; /* 2bit bw */
	u8 rssi; /* should not put here */
	u8 rainfo_cfg1; /* prepare for other control*/
	u8 rainfo_cfg2; /* prepare for other control*/

	u8 rssi_lv;

	/* Report */
	u8 rpt_rate; /* 7bit rate + 2bit rat_md + 3bit giltf + 2bit bw */
	u8 rpt_rat_md;
	u8 rpt_giltf;
	u8 rpt_bw;
	u8 rpt_ratio;

	u8 tmp;
	
};

union bb_h2c_ra_rssi_info {
    u32 val[2];
    struct bb_h2c_rssi_setting bb_h2c_ra_rssi;
    struct bb_h2c_rssi_setting_wifi7 bb_h2c_ra_rssi_wifi7;
};

/*@--------------------------[Prptotype]-------------------------------------*/

struct bb_info;
bool halbb_is_cck_rate(struct bb_info *bb, u16 rate);
bool halbb_is_ofdm_rate(struct bb_info *bb, u16 rate);
bool halbb_is_ht_rate(struct bb_info *bb, u16 rate);
bool halbb_is_vht_rate(struct bb_info *bb, u16 rate);
bool halbb_is_he_rate(struct bb_info *bb, u16 rate);
#ifdef BB_8922A_DVLP_SPF
bool halbb_is_eht_rate_wifi7(struct bb_info *bb, u16 rate);
bool halbb_is_he_rate_wifi7(struct bb_info *bb, u16 rate);
bool halbb_is_vht_rate_wifi7(struct bb_info *bb, u16 rate);
bool halbb_is_ht_rate_wifi7(struct bb_info *bb, u16 rate);
#endif
u8 halbb_legacy_rate_2_spec_rate(struct bb_info *bb, u16 rate);
u8 halbb_rate_2_rate_digit(struct bb_info *bb, u16 rate);
u8 halbb_get_rx_stream_num(struct bb_info *bb, enum rf_type type);
u8 halbb_rate_type_2_num_ss(struct bb_info *bb, enum halbb_rate_type type);
u8 halbb_rate_to_num_ss(struct bb_info *bb, u16 rate);
u16 halbb_gen_rate_idx(struct bb_info *bb, enum bb_mode_type mode, u8 ss, u8 idx);
void halbb_print_rate_2_buff(struct bb_info *bb, u16 rate_idx, enum rtw_gi_ltf gi_ltf, char *buf, u16 buf_size);
enum bb_qam_type halbb_get_qam_order(struct bb_info *bb, u16 rate_idx);
u8 halbb_rate_order_compute(struct bb_info *bb, u16 rate_idx);
void halbb_ra_watchdog(struct bb_info *bb);

void halbb_ra_init(struct bb_info *bb);
void halbb_ra_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
void halbb_rate_idx_parsor(struct bb_info *bb, u16 rate_idx, enum rtw_gi_ltf gi_ltf, struct bb_rate_info *ra_i);
u32 halbb_get_fw_ra_rpt(struct bb_info *bb, u16 len, u8 *c2h);
u32 halbb_get_txsts_rpt(struct bb_info *bb, u16 len, u8 *c2h);
void halbb_get_ra_dbgreg(struct bb_info *bb);
#endif

