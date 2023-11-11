/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_REGULATION_DEF_H_
#define _PHL_REGULATION_DEF_H_

#define RSVD_DOMAIN 0x1a

#define MAX_CH_NUM_2GHZ 14

#define MAX_CH_NUM_BAND1 4 /* 36, 40, 44, 48 */
#define MAX_CH_NUM_BAND2 4 /* 52, 56, 60, 64 */
#define MAX_CH_NUM_BAND3 12 /* 100, 104, 108, 112,
				116, 120, 124, 128,
				132, 136, 140, 144 */
#define MAX_CH_NUM_BAND4 8 /* 149, 153, 157, 161, 165, 169, 173, 177 */
#define MAX_CH_NUM_5GHZ (MAX_CH_NUM_BAND1 + MAX_CH_NUM_BAND2 +\
				MAX_CH_NUM_BAND3 + MAX_CH_NUM_BAND4)

#define MAX_CH_NUM_UNII5 24 /* 1 ~ 93 */
#define MAX_CH_NUM_UNII6 6 /* 97 ~ 117 */
#define MAX_CH_NUM_UNII7 18 /* 121 ~ 189 */
#define MAX_CH_NUM_UNII8 11 /* 193 ~ 233 */
#define MAX_CH_NUM_6GHZ (MAX_CH_NUM_UNII5 + MAX_CH_NUM_UNII6 +\
				MAX_CH_NUM_UNII7 + MAX_CH_NUM_UNII8)


#define BAND_2GHZ(_band_) ((_band_ == BAND_ON_24G) ? true : false)
#define BAND_5GHZ(_band_) ((_band_ == BAND_ON_5G) ? true : false)
#define BAND_6GHZ(_band_) ((_band_ == BAND_ON_6G) ? true : false)

#define CH_5GHZ_BAND1(_ch_) (((_ch_ >= 36) && (_ch_ <= 48)) ? true : false)
#define CH_5GHZ_BAND2(_ch_) (((_ch_ >= 52) && (_ch_ <= 64)) ? true : false)
#define CH_5GHZ_BAND3(_ch_) (((_ch_ >= 100) && (_ch_ <= 144)) ? true : false)
#define CH_5GHZ_BAND4(_ch_) (((_ch_ >= 149) && (_ch_ <= 177)) ? true : false)
#define CH_59GHZ(_ch_) (((_ch_ > 165) && (_ch_ <= 177)) ? true : false)


/* UNII-5 support channel list, ch1 ~ ch93, total : 24 */
/* UNII-6 support channel list, ch97 ~ ch117, total : 6 */
/* UNII-7 support channel list, ch121 ~ ch189, total : 18 */
/* UNII-8 support channel list, ch193 ~ ch233, total : 11 */
#define CH_6GHZ_UNII5(_ch_) (((_ch_ >= 1) && (_ch_ <= 93)) ? true : false)
#define CH_6GHZ_UNII6(_ch_) (((_ch_ >= 97) && (_ch_ <= 117)) ? true : false)
#define CH_6GHZ_UNII7(_ch_) (((_ch_ >= 121) && (_ch_ <= 189)) ? true : false)
#define CH_6GHZ_UNII8(_ch_) (((_ch_ >= 193) && (_ch_ <= 233)) ? true : false)

#define CH_IN_SAME_5G_BAND(ch_a, ch_b) \
	((CH_5GHZ_BAND1(ch_a) &&   CH_5GHZ_BAND1(ch_b)) \
	|| (CH_5GHZ_BAND2(ch_a) &&   CH_5GHZ_BAND2(ch_b)) \
	|| (CH_5GHZ_BAND3(ch_a) &&   CH_5GHZ_BAND3(ch_b)) \
	|| (CH_5GHZ_BAND4(ch_a) &&   CH_5GHZ_BAND4(ch_b)))

#define CH_IN_SAME_6G_BAND(ch_a, ch_b) 0

#define SUPPORT_11A BIT(0)
#define SUPPORT_11B BIT(1)
#define SUPPORT_11G BIT(2)
#define SUPPORT_11N BIT(3)
#define SUPPORT_11AC BIT(4)
#define SUPPORT_11AX BIT(5)

enum regulation_rsn {
	REGU_RSN_DEFAULT = 0x0,
	REGU_RSN_SMBIOS,
	REGU_RSN_EFUSE,
	REGU_RSN_11D,
	REGU_RSN_REGISTRY,
	REGU_RSN_LOCATION,
	REGU_RSN_MANUAL,
	REGU_RSN_MAX
};

enum rtw_regulation_capability {
	CAPABILITY_2GHZ = BIT(0),
	CAPABILITY_5GHZ = BIT(1),
	CAPABILITY_DFS = BIT(2),
	CAPABILITY_6GHZ = BIT(3),
	CAPABILITY_59GHZ = BIT(4),
};

enum rtw_regulation_query {
	REGULQ_CHPLAN_2GHZ = BIT(0),
	REGULQ_CHPLAN_5GHZ_BAND1 = BIT(1),
	REGULQ_CHPLAN_5GHZ_BAND2 = BIT(2),
	REGULQ_CHPLAN_5GHZ_BAND3 = BIT(3),
	REGULQ_CHPLAN_5GHZ_BAND4 = BIT(4),
	REGULQ_CHPLAN_6GHZ_UNII5 = BIT(5),
	REGULQ_CHPLAN_6GHZ_UNII6 = BIT(6),
	REGULQ_CHPLAN_6GHZ_UNII7 = BIT(7),
	REGULQ_CHPLAN_6GHZ_UNII8 = BIT(8),
	REGULQ_CHPLAN_6GHZ_PSC = BIT(9),
	REGULQ_CHPLAN_6GHZ_ALL = (REGULQ_CHPLAN_6GHZ_UNII5 |
		REGULQ_CHPLAN_6GHZ_UNII6 | REGULQ_CHPLAN_6GHZ_UNII7 |
		REGULQ_CHPLAN_6GHZ_UNII8),
	REGULQ_CHPLAN_5GHZ_ALL = (REGULQ_CHPLAN_5GHZ_BAND1 |
		REGULQ_CHPLAN_5GHZ_BAND2 | REGULQ_CHPLAN_5GHZ_BAND3 |
		REGULQ_CHPLAN_5GHZ_BAND4),
	REGULQ_CHPLAN_2GHZ_5GHZ = (REGULQ_CHPLAN_2GHZ |
		REGULQ_CHPLAN_5GHZ_BAND1 | REGULQ_CHPLAN_5GHZ_BAND2 |
		REGULQ_CHPLAN_5GHZ_BAND3 | REGULQ_CHPLAN_5GHZ_BAND4),
	REGULQ_CHPLAN_FULL = (REGULQ_CHPLAN_2GHZ | REGULQ_CHPLAN_5GHZ_ALL |
					REGULQ_CHPLAN_6GHZ_ALL),
	REGULQ_CHPLAN_DEF = (REGULQ_CHPLAN_2GHZ | REGULQ_CHPLAN_5GHZ_ALL |
					REGULQ_CHPLAN_6GHZ_PSC),
};

enum country_property {
	CNTRY_EU = BIT(0), /* European Union */
};

enum ch_property {
	CH_PASSIVE = BIT(0), /* regulatory passive channel */
	CH_DFS = BIT(1), /* 5 ghz DFS channel */
	CH_PSC = BIT(2) /* 6 ghz preferred scanning channel */
};

enum rtw_regulation_group_id {
	DEFAULT_SUPPORT_6G = 0x0,
	CURRENT_SUPPORT_6G,
	EU_GROUP,
	FCC_GROUP
};

struct rtw_regulation_channel {
	enum band_type band;
	u8 channel;
	u8 property;
};

struct rtw_regulation_chplan {
	u32 cnt;
	struct rtw_regulation_channel ch[MAX_CH_NUM_2GHZ +
					MAX_CH_NUM_5GHZ +
					MAX_CH_NUM_6GHZ];
};

struct rtw_ch {
	enum band_type band;
	u8 ch;
};

struct rtw_chlist {
	u32 cnt;
	struct rtw_ch ch[MAX_CH_NUM_2GHZ +
			MAX_CH_NUM_5GHZ +
			MAX_CH_NUM_6GHZ];
};

struct rtw_regulatory_domain {
	u8 domain; /* original 2/5 ghz domain */
	u8 domain_6g; /* 6 ghz domain */
};

enum cntry_property_6g_bp {
	CP_6G_BAND_BLOCKED = BIT(0), /* country is blocked, cannot use 6 ghz band */
	CP_6G_BAND_SOFTAP_BLOCKED = BIT(1), /* country cannot use softap on 6 ghz band */
};

enum cntry_property_5g_bp {
	CP_5G_BAND1_SOFTAP_BLOCKED = BIT(0), /* country cannot use softap on 5 ghz band 1 */
	CP_5G_BAND2_SOFTAP_BLOCKED = BIT(1), /* country cannot use softap on 5 ghz band 2 */
	CP_5G_BAND3_SOFTAP_BLOCKED = BIT(2), /* country cannot use softap on 5 ghz band 3 */
	CP_5G_BAND4_SOFTAP_BLOCKED = BIT(3), /* country cannot use softap on 5 ghz band 4 */
};

enum cp_bp_mode { /* country property - band policy mode */
	BLOCK_MODE = 0, /* selected countries are blocked, otherwise follow RTK */
	ALLOW_MODE = 1, /* selected countries are allowed (to follow RTK), otherwise blocked */
};

struct rtw_regu_policy_info {
	u8 valid_6g_bp;
	u8 cp_6g_bp; /* country property - 6g band policy */
	u8 valid_5g_bp;
	u8 cp_5g_bp; /* country property - 5g band policy */
};

struct rtw_regulation_info {
	u8 domain_code;
	u8 domain_reason;
	u8 domain_code_6g;
	u8 domain_reason_6g;
	char country[2];
	u8 support_mode;
	u8 regulation_2g;
	u8 regulation_5g;
	u8 regulation_6g;
	u8 tpo;
	u8 chplan_ver;
	u8 country_ver;
	u16 capability;
};

struct rtw_regulation_country_chplan {
	u8 domain_code;
	u8 domain_code_6g;
	u8 support_mode;
	/*
	* bit0: accept 11a
	* bit1: accept 11b
	* bit2: accept 11g
	* bit3: accept 11n
	* bit4: accept 11ac
	* bit5: accept 11ax
	*/
	u8 tpo; /* tx power overwrite */
};

struct rtw_user_def_chplan {
	u16 ch2g; /* bit0 ~ bit13 : ch1~ch14 */
	u16 passive2g; /* bit0 ~ bit13 : ch1~ch14, if value = 1, means passive for that channel */

	/* 5g channels.
	 * bit0~7 : ch 36/40/44/48/52/56/60/64
	 * bit8~15 : ch 100/104/108/112/116/120/124/128
	 * bit16~23 : ch 132/136/140/144/149/153/157/161
	 * bit24~27 : ch 165/169/173/177
	 */
	u32 ch5g;
	u32 passive5g;
	u32 dfs5g;
	/* 6g channels.
	 * bit0~7 : ch 1/5/9/13/17/21/25/29
	 * bit8~15 : ch 33/37/41/45/49/53/57/61
	 * bit16~23 : ch 65/69/73/77/81/85/89/93
	 * bit24~31 : ch 97/101/105/109/113/117/121/125
	 * bit32~39 : ch 129/133/137/141/145/149/153/157
	 * bit40~47 : ch 161/165/169/173/177/181/185/189
	 * bit48~55 : ch 193/197/201/205/209/213/217/221
	 * bit56~58 : ch 225/229/233
	 */
	u64 ch6g;
	u64 passive6g;

	u32 regulatory_idx;
	u8 tpo;
};

/*
 * NOTE:
 * 	This api prototype will be removed after hal related API/header is added
 * 	for halrf.
 */
bool rtw_phl_query_regulation_info(void *phl, struct rtw_regulation_info *info);

#endif /* _PHL_REGULATION_DEF_H_ */
