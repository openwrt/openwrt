/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef	__RTW_RF_H_
#define __RTW_RF_H_

#define NumRates	(13)
#define	B_MODE_RATE_NUM	(4)
#define	G_MODE_RATE_NUM	(8)
#define	G_MODE_BASIC_RATE_NUM	(3)
/* slot time for 11g */
#define SHORT_SLOT_TIME					9
#define NON_SHORT_SLOT_TIME				20

#define CENTER_CH_2G_NUM		14
#define CENTER_CH_2G_40M_NUM	9

#define CENTER_CH_5G_20M_NUM	28	/* 20M center channels */
#define CENTER_CH_5G_40M_NUM	14	/* 40M center channels */
#define CENTER_CH_5G_80M_NUM	7	/* 80M center channels */
#define CENTER_CH_5G_160M_NUM	3	/* 160M center channels */
#define CENTER_CH_5G_ALL_NUM	(CENTER_CH_5G_20M_NUM + CENTER_CH_5G_40M_NUM + CENTER_CH_5G_80M_NUM)

#define CENTER_CH_6G_20M_NUM	64	/* 20M center channels */
#define CENTER_CH_6G_40M_NUM	32	/* 40M center channels */
#define CENTER_CH_6G_80M_NUM	16	/* 80M center channels */
#define CENTER_CH_6G_160M_NUM	8	/* 160M center channels */

#define	MAX_CHANNEL_NUM_2G		CENTER_CH_2G_NUM
#define	MAX_CHANNEL_NUM_5G		CENTER_CH_5G_20M_NUM
#define MAX_CHANNEL_NUM_6G		CENTER_CH_6G_20M_NUM
#define MAX_CHANNEL_NUM_2G_5G	(MAX_CHANNEL_NUM_2G + MAX_CHANNEL_NUM_5G)

#define	MAX_CHANNEL_NUM		( \
	MAX_CHANNEL_NUM_2G \
	+ (CONFIG_IEEE80211_BAND_5GHZ ? MAX_CHANNEL_NUM_5G : 0) \
	+ (CONFIG_IEEE80211_BAND_6GHZ ? MAX_CHANNEL_NUM_6G : 0) \
	)

extern u8 center_ch_2g[CENTER_CH_2G_NUM];
extern u8 center_ch_2g_40m[CENTER_CH_2G_40M_NUM];

u8 center_chs_2g_num(u8 bw);
u8 center_chs_2g(u8 bw, u8 id);

#if CONFIG_IEEE80211_BAND_5GHZ
extern u8 center_ch_5g_20m[CENTER_CH_5G_20M_NUM];
extern u8 center_ch_5g_40m[CENTER_CH_5G_40M_NUM];
extern u8 center_ch_5g_80m[CENTER_CH_5G_80M_NUM];
extern u8 center_ch_5g_160m[CENTER_CH_5G_160M_NUM];
extern u8 center_ch_5g_all[CENTER_CH_5G_ALL_NUM];

u8 center_chs_5g_num(u8 bw);
u8 center_chs_5g(u8 bw, u8 id);
#endif

#if CONFIG_IEEE80211_BAND_6GHZ
extern u8 center_ch_6g_20m[CENTER_CH_6G_20M_NUM];
extern u8 center_ch_6g_40m[CENTER_CH_6G_40M_NUM];
extern u8 center_ch_6g_80m[CENTER_CH_6G_80M_NUM];
extern u8 center_ch_6g_160m[CENTER_CH_6G_160M_NUM];

u8 center_chs_6g_num(u8 bw);
u8 center_chs_6g(u8 bw, u8 id);
#endif

extern u8 (*center_chs_num_of_band[BAND_MAX])(u8 bw);
extern u8 (*center_chs_of_band[BAND_MAX])(u8 bw, u8 id);

RTW_FUNC_2G_5G_ONLY u8 rtw_get_scch_by_cch_offset(u8 cch, u8 bw, u8 offset);
RTW_FUNC_2G_5G_ONLY u8 rtw_get_scch_by_cch_opch(u8 cch, u8 bw, u8 opch);

RTW_FUNC_2G_5G_ONLY u8 rtw_get_op_chs_by_cch_bw(u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num);

RTW_FUNC_2G_5G_ONLY u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset);
RTW_FUNC_2G_5G_ONLY u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);

RTW_FUNC_2G_5G_ONLY bool rtw_is_chbw_grouped(u8 ch_a, u8 bw_a, u8 offset_a, u8 ch_b, u8 bw_b, u8 offset_b);
RTW_FUNC_2G_5G_ONLY void rtw_sync_chbw(u8 *req_ch, u8 *req_bw, u8 *req_offset, u8 *g_ch, u8 *g_bw, u8 *g_offset);

u8 rtw_get_scch_by_bcch_offset(enum band_type band, u8 cch, u8 bw, u8 offset);
u8 rtw_get_scch_by_bcch_opch(enum band_type band, u8 cch, u8 bw, u8 opch);

u8 rtw_get_op_chs_by_bcch_bw(enum band_type band, u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num);

u8 rtw_get_offset_by_bchbw(enum band_type band, u8 ch, u8 bw, u8 *r_offset);
u8 rtw_get_center_ch_by_band(enum band_type band, u8 ch, u8 bw, u8 offset);

bool rtw_is_bchbw_grouped(enum band_type band_a, u8 ch_a, u8 bw_a, u8 offset_a
	, enum band_type band_b, u8 ch_b, u8 bw_b, u8 offset_b);
void rtw_sync_bchbw(enum band_type *req_band, u8 *req_ch, u8 *req_bw, u8 *req_offset
	, enum band_type *g_band, u8 *g_ch, u8 *g_bw, u8 *g_offset);

typedef enum _CAPABILITY {
	cESS			= 0x0001,
	cIBSS			= 0x0002,
	cPollable		= 0x0004,
	cPollReq			= 0x0008,
	cPrivacy		= 0x0010,
	cShortPreamble	= 0x0020,
	cPBCC			= 0x0040,
	cChannelAgility	= 0x0080,
	cSpectrumMgnt	= 0x0100,
	cQos			= 0x0200,	/* For HCCA, use with CF-Pollable and CF-PollReq */
	cShortSlotTime	= 0x0400,
	cAPSD			= 0x0800,
	cRM				= 0x1000,	/* RRM (Radio Request Measurement) */
	cDSSS_OFDM	= 0x2000,
	cDelayedBA		= 0x4000,
	cImmediateBA	= 0x8000,
} CAPABILITY, *PCAPABILITY;

enum	_REG_PREAMBLE_MODE {
	PREAMBLE_LONG	= 1,
	PREAMBLE_AUTO	= 2,
	PREAMBLE_SHORT	= 3,
};

#define rf_path_char(path) (((path) >= RF_PATH_MAX) ? 'X' : 'A' + (path))

#ifdef CONFIG_NARROWBAND_SUPPORTING
enum nb_config {
	RTW_NB_CONFIG_NONE		= 0,
	RTW_NB_CONFIG_WIDTH_5	= 5,
	RTW_NB_CONFIG_WIDTH_10	= 6,
};
#endif

extern const char *const _rtw_band_str[];
#define band_str(band) (((band) >= BAND_MAX) ? _rtw_band_str[BAND_MAX] : _rtw_band_str[(band)])

extern const u8 _band_to_band_cap[];
#define band_to_band_cap(band) (((band) >= BAND_MAX) ? _band_to_band_cap[BAND_MAX] : _band_to_band_cap[(band)])


extern const char *const _ch_width_str[];
#define ch_width_str(bw) (((bw) < CHANNEL_WIDTH_MAX) ? _ch_width_str[(bw)] : "CHANNEL_WIDTH_MAX")

extern const u8 _ch_width_to_bw_cap[];
#define ch_width_to_bw_cap(bw) (((bw) < CHANNEL_WIDTH_MAX) ? _ch_width_to_bw_cap[(bw)] : 0)

enum opc_bw {
	OPC_BW20		= 0,
	OPC_BW40PLUS	= 1,
	OPC_BW40MINUS	= 2,
	OPC_BW40		= 3,
	OPC_BW80		= 4,
	OPC_BW160		= 5,
	OPC_BW80P80		= 6,
	OPC_BW_NUM,
};

extern const char *const _opc_bw_str[OPC_BW_NUM];
#define opc_bw_str(bw) (((bw) < OPC_BW_NUM) ? _opc_bw_str[(bw)] : "N/A")

extern const u8 _opc_bw_to_ch_width[OPC_BW_NUM];
#define opc_bw_to_ch_width(bw) (((bw) < OPC_BW_NUM) ? _opc_bw_to_ch_width[(bw)] : CHANNEL_WIDTH_MAX)

/* global op class APIs */
bool is_valid_global_op_class_id(u8 gid);
s16 get_sub_op_class(u8 gid, u8 ch);
void dump_global_op_class(void *sel);
RTW_FUNC_2G_5G_ONLY u8 rtw_get_op_class_by_chbw(u8 ch, u8 bw, u8 offset);
u8 rtw_get_op_class_by_bchbw(enum band_type band, u8 ch, u8 bw, u8 offset);
u8 rtw_get_bw_offset_by_op_class_ch(u8 gid, u8 ch, u8 *bw, u8 *offset);
#ifdef CONFIG_ECSA_PHL
int alink_get_supported_op_class(struct _ADAPTER_LINK *padapter_link, u8 *op_set, int len);
int get_supported_op_class(_adapter *padapter, u8 *op_set, int len);
#endif

struct op_ch_t {
	u8 ch;
	u8 static_non_op:1; /* not in channel list */
	u8 no_ir:1;
	s16 max_txpwr; /* mBm */
};

struct op_class_pref_t {
	u8 class_id;
	enum band_type band;
	enum opc_bw bw;
	u8 ch_num; /* number of chs */
	u8 op_ch_num; /* channel number which is not static non operable */
	u8 ir_ch_num; /* channel number which can init radiation */
	struct op_ch_t chs[];
};

struct rf_ctl_t;
int rtw_rfctl_op_class_pref_init(struct rf_ctl_t *rfctl, u8 band_bmp, u8 bw_bmp[]);
void rtw_rfctl_op_class_pref_deinit(struct rf_ctl_t *rfctl);

#define REG_BEACON_HINT		0
#define REG_TXPWR_CHANGE	1
#define REG_CHANGE			2

void op_class_pref_apply_regulatory(struct rf_ctl_t *rfctl, u8 reason);

struct rf_ctl_t;
void dump_cap_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);
void dump_reg_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);
void dump_cur_spt_op_class_ch(void *sel, struct rf_ctl_t *rfctl, bool detail);

typedef enum _VHT_DATA_SC {
	VHT_DATA_SC_DONOT_CARE = 0,
	VHT_DATA_SC_20_UPPER_OF_80MHZ = 1,
	VHT_DATA_SC_20_LOWER_OF_80MHZ = 2,
	VHT_DATA_SC_20_UPPERST_OF_80MHZ = 3,
	VHT_DATA_SC_20_LOWEST_OF_80MHZ = 4,
	VHT_DATA_SC_20_RECV1 = 5,
	VHT_DATA_SC_20_RECV2 = 6,
	VHT_DATA_SC_20_RECV3 = 7,
	VHT_DATA_SC_20_RECV4 = 8,
	VHT_DATA_SC_40_UPPER_OF_80MHZ = 9,
	VHT_DATA_SC_40_LOWER_OF_80MHZ = 10,
} VHT_DATA_SC, *PVHT_DATA_SC_E;

typedef enum _PROTECTION_MODE {
	PROTECTION_MODE_AUTO = 0,
	PROTECTION_MODE_FORCE_ENABLE = 1,
	PROTECTION_MODE_FORCE_DISABLE = 2,
} PROTECTION_MODE, *PPROTECTION_MODE;

typedef enum _RF_TX_NUM {
	RF_1TX = 0,
	RF_2TX,
	RF_3TX,
	RF_4TX,
	RF_MAX_TX_NUM,
	RF_TX_NUM_NONIMPLEMENT,
} RF_TX_NUM;

#define RF_TYPE_VALID(rf_type) (rf_type < RF_TYPE_MAX)

extern const u8 _rf_type_to_rf_tx_cnt[];
#define rf_type_to_rf_tx_cnt(rf_type) (RF_TYPE_VALID(rf_type) ? _rf_type_to_rf_tx_cnt[rf_type] : 0)

extern const u8 _rf_type_to_rf_rx_cnt[];
#define rf_type_to_rf_rx_cnt(rf_type) (RF_TYPE_VALID(rf_type) ? _rf_type_to_rf_rx_cnt[rf_type] : 0)

extern const char *const _rf_type_to_rfpath_str[];
#define rf_type_to_rfpath_str(rf_type) (RF_TYPE_VALID(rf_type) ? _rf_type_to_rfpath_str[rf_type] : "UNKNOWN")

enum rf_type trx_num_to_rf_type(u8 tx_num, u8 rx_num);
enum rf_type trx_bmp_to_rf_type(u8 tx_bmp, u8 rx_bmp);
bool rf_type_is_a_in_b(enum rf_type a, enum rf_type b);
u8 rtw_restrict_trx_path_bmp_by_rftype(u8 trx_path_bmp, enum rf_type type, u8 *tx_num, u8 *rx_num);

RTW_FUNC_2G_5G_ONLY int rtw_ch2freq(int chan);
int rtw_bch2freq(enum band_type band, int ch);
int rtw_freq2ch(int freq);
enum band_type rtw_freq2band(int freq);
enum channel_width rtw_frange_to_bw(u32 hi, u32 lo);
bool rtw_freq_consecutive(int a, int b);
bool rtw_bcchbw_to_freq_range(enum band_type band, u8 c_ch, u8 bw, u32 *hi, u32 *lo);
bool rtw_bchbw_to_freq_range(enum band_type band, u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo);
RTW_FUNC_2G_5G_ONLY bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo);

struct rf_ctl_t;

void txpwr_idx_get_dbm_str(s8 idx, u8 txgi_max, u8 txgi_pdbm, SIZE_T cwidth, char dbm_str[], u8 dbm_str_len);

#define MBM_PDBM 100
#define UNSPECIFIED_MBM 32767 /* maximum of s16 */

void txpwr_mbm_get_dbm_str(s16 mbm, SIZE_T cwidth, char dbm_str[], u8 dbm_str_len);
s16 mb_of_ntx(u8 ntx);

#if CONFIG_TXPWR_LIMIT
void dump_regd_exc_list(void *sel, struct rf_ctl_t *rfctl);
void dump_txpwr_lmt(void *sel, _adapter *adapter);
#endif

RTW_FUNC_2G_5G_ONLY static inline bool rtw_is_2g_ch(u8 ch) { return ch >= 1 && ch <= 14; }
RTW_FUNC_2G_5G_ONLY static inline bool rtw_is_5g_ch(u8 ch) { return ch >= 36 && ch <= 177; }
RTW_FUNC_2G_5G_ONLY static inline enum band_type rtw_get_band_type(u8 chan)
{
	return (chan > 14) ? BAND_ON_5G : BAND_ON_24G;
}

bool rtw_is_long_cac_range(u32 hi, u32 lo, u8 dfs_region);
RTW_FUNC_2G_5G_ONLY bool rtw_is_long_cac_ch(u8 ch, u8 bw, u8 offset, u8 dfs_region);
bool rtw_is_long_cac_bch(enum band_type band, u8 ch, u8 bw, u8 offset, u8 dfs_region);

#endif /* _RTL8711_RF_H_ */
