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
#ifndef _PHL_REGULATION_H_
#define _PHL_REGULATION_H_

#define MAX_CH_NUM_GROUP 24

enum rtw_regulation_freq_group {
	FREQ_GROUP_2GHZ = 0x0,
	FREQ_GROUP_5GHZ_BAND1,
	FREQ_GROUP_5GHZ_BAND2,
	FREQ_GROUP_5GHZ_BAND3,
	FREQ_GROUP_5GHZ_BAND4,
	FREQ_GROUP_6GHZ_UNII5,
	FREQ_GROUP_6GHZ_UNII6,
	FREQ_GROUP_6GHZ_UNII7,
	FREQ_GROUP_6GHZ_UNII8,
	FREQ_GROUP_MAX
};

enum rtw_regulation_status {
	REGULATION_SUCCESS = 0x0,
	REGULATION_FAILURE,
	REGULATION_DOMAIN_MISMATCH,
	REGULATION_INVALID_2GHZ_RD,
	REGULATION_INVALID_5GHZ_RD,
	REGULATION_INVALID_DOMAIN
};

struct rtw_regulation_chplan_group {
	u32 cnt;
	struct rtw_regulation_channel ch[MAX_CH_NUM_GROUP];
};

#define INVALID_DOMAIN_CODE 0xffff
#define INVALID_CHDEF 0xff

struct rtw_domain {
	u16 code;
	u8 reason;
};

struct rtw_regu_policy {
	u8 valid_6g_bp; /* 1: valid, 0: invalid */
	u8 cp_6g_bp[MAX_COUNTRY_NUM]; /* the country property for 6g band policy */
	u8 valid_5g_bp; /* 1: valid, 0: invalid */
	u8 cp_5g_bp[MAX_COUNTRY_NUM]; /* the country property for 5g band policy */
};

struct rtw_regulation {
	_os_lock lock;
	u8 init; /* regulation sw initialization */

	/* working regulation */
	bool valid; /* true if domain code successfully set */
	u16 capability; /* rtw_regulation_capability */
	struct rtw_domain domain;
	char country[2];
	u8 tpo; /* tx power overwrite */
	u8 support_mode;

	u8 ch_idx2g; /* 2ghz chdef index */
	u8 regulation_2g;
	u8 ch_idx5g; /* 5ghz chdef index */
	u8 regulation_5g;

	struct rtw_regulation_chplan_group chplan[FREQ_GROUP_MAX];

	struct rtw_regu_policy policy; /* regulatory policy */

	/* 6 ghz */
	struct rtw_regulation_chplan_group psc_6g; /* 6 ghz psc */
	bool valid_6g; /* true if domain code successfully set */
	struct rtw_domain domain_6g;
	u8 ch_idx6g; /* 6ghz chdef index */
	u8 regulation_6g;
};

void phl_regu_policy_init(void *drv_priv, void *phl);

bool rtw_phl_regulation_query_ch(void *phl, enum band_type band, u8 channel,
					struct rtw_regulation_channel *ch);

enum rtw_regulation_freq_group phl_get_regu_freq_group(enum band_type band, u8 ch);

#endif /* _PHL_REGULATION_H_ */
