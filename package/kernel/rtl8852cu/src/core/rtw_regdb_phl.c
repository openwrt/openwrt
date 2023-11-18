/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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
#define _RTW_REGDB_PHL_C_

#include <drv_types.h>

static enum rtw_regd phl_regulation_to_regd(enum REGULATION reg)
{
	switch (reg) {
	case REGULATION_WW:
		return RTW_REGD_WW;
	case REGULATION_ETSI:
		return RTW_REGD_ETSI;
	case REGULATION_FCC:
		return RTW_REGD_FCC;
	case REGULATION_MKK:
		return RTW_REGD_MKK;
	case REGULATION_KCC:
		return RTW_REGD_KCC;
	case REGULATION_NCC:
		return RTW_REGD_NCC;
	case REGULATION_ACMA:
		return RTW_REGD_ACMA;
	case REGULATION_NA:
		return RTW_REGD_NA;
	case REGULATION_IC:
		return RTW_REGD_IC;
	case REGULATION_CHILE:
		return RTW_REGD_CHILE;
	case REGULATION_MEX:
		return RTW_REGD_MEX;
	case REGULATION_MAX:
	default:
		RTW_WARN("unknown reg:%d\n", reg);
		rtw_warn_on(1);
		return RTW_REGD_NA;
	}
};

static u8 phl_ch_property_to_flag(enum ch_property prop)
{
	return ((prop & CH_PASSIVE) ? RTW_CHF_NO_IR : 0)
		| ((prop & CH_DFS) ? RTW_CHF_DFS : 0)
		;
}

static u8 phl_regdb_get_default_regd_2g(u8 id)
{
	return phl_regulation_to_regd(rtw_phl_get_domain_regulation_2g(id));
}

#if CONFIG_IEEE80211_BAND_5GHZ
static u8 phl_regdb_get_default_regd_5g(u8 id)
{
	return phl_regulation_to_regd(rtw_phl_get_domain_regulation_5g(id));
}
#endif

static bool phl_regdb_is_domain_code_valid(u8 id)
{
	return rtw_phl_valid_regulation_domain(id);
}

static bool phl_regdb_domain_get_ch(u8 id, u32 ch, u8 *flags)
{
	enum ch_property prop;

	if (rtw_phl_query_domain_channel(id, ch > 14 ? BAND_ON_5G : BAND_ON_24G, ch, &prop)) {
		if (flags)
			*flags = phl_ch_property_to_flag(prop);
		return true;
	}
	return false;
}

#if CONFIG_IEEE80211_BAND_6GHZ
static u8 phl_regdb_get_default_regd_6g(u8 id)
{
	return phl_regulation_to_regd(rtw_phl_get_domain_regulation_6g(id));
}

static bool phl_regdb_is_domain_code_6g_valid(u8 id)
{
	return rtw_phl_valid_regulation_domain_6ghz(id);
}

static bool phl_regdb_domain_6g_get_ch(u8 id, u32 ch, u8 *flags)
{
	enum ch_property prop;

	if (rtw_phl_query_domain_6g_channel(id, BAND_ON_6G, ch, &prop)) {
		if (flags)
			*flags = phl_ch_property_to_flag(prop);
		return true;
	}
	return false;
}
#endif /* CONFIG_IEEE80211_BAND_6GHZ */

static REGULATION_TXPWR_LMT phl_tpo_to_txpwr_lmt(enum TP_OVERWRITE tpo)
{
	switch (tpo) {
	case TPO_CHILE:
		return TXPWR_LMT_CHILE;
	case TPO_UK:
		return TXPWR_LMT_UK;
	case TPO_QATAR:
		return TXPWR_LMT_QATAR;
	case TPO_UKRAINE:
		return TXPWR_LMT_UKRAINE;
	case TPO_CN:
		return TXPWR_LMT_CN;
	case TPO_NA:
		return TXPWR_LMT_DEF;
	default:
		RTW_WARN("unknown tpo:%d\n", tpo);
		rtw_warn_on(1);
		return TXPWR_LMT_DEF;
	}
}

static bool phl_regdb_get_chplan_from_alpha2(const char *alpha2, struct country_chplan *ent)
{
	struct rtw_regulation_country_chplan plan;

	_rtw_memset(&plan, 0, sizeof(plan));

	if (!rtw_phl_query_country_chplan((char *)alpha2, &plan)) /* TODO:  PHL use type of 'const char *' for 'country' */
		return false;

	if (ent) {
		_rtw_memcpy(ent->alpha2, alpha2, 2);
		ent->domain_code = plan.domain_code;
		#if CONFIG_IEEE80211_BAND_6GHZ
		ent->domain_code_6g = plan.domain_code_6g;
		#endif
		ent->edcca_mode_2g_override = RTW_EDCCA_DEF;
		#if CONFIG_IEEE80211_BAND_5GHZ
		ent->edcca_mode_5g_override = RTW_EDCCA_DEF;
		#endif
		#if CONFIG_IEEE80211_BAND_6GHZ
		ent->edcca_mode_6g_override = RTW_EDCCA_DEF;
		#endif
		ent->txpwr_lmt_override = phl_tpo_to_txpwr_lmt(plan.tpo);
		#if defined(CONFIG_80211AX_HE) || defined(CONFIG_80211AC_VHT) || CONFIG_IEEE80211_BAND_5GHZ
		ent->proto_en = 0
			| ((plan.support_mode & SUPPORT_11AC) ? CHPLAN_PROTO_EN_AC : 0)
			| ((plan.support_mode & SUPPORT_11AX) ? CHPLAN_PROTO_EN_AX : 0)
			| ((plan.support_mode & SUPPORT_11A) ? CHPLAN_PROTO_EN_A : 0)
			;
		#endif
	}

	return true;
}

static void phl_regdb_get_ver_str(char *buf, size_t buf_len)
{
	snprintf(buf, buf_len, "%d-%d", rtw_phl_get_regu_chplan_ver(), rtw_phl_get_regu_country_ver());
}

struct rtw_regdb_ops regdb_ops = {
	.get_default_regd_2g = phl_regdb_get_default_regd_2g,
#if CONFIG_IEEE80211_BAND_5GHZ
	.get_default_regd_5g = phl_regdb_get_default_regd_5g,
#endif
	.is_domain_code_valid = phl_regdb_is_domain_code_valid,
	.domain_get_ch = phl_regdb_domain_get_ch,

#if CONFIG_IEEE80211_BAND_6GHZ
	.get_default_regd_6g = phl_regdb_get_default_regd_6g,
	.is_domain_code_6g_valid = phl_regdb_is_domain_code_6g_valid,
	.domain_6g_get_ch = phl_regdb_domain_6g_get_ch,
#endif

	.get_chplan_from_alpha2 = phl_regdb_get_chplan_from_alpha2,

	.get_ver_str = phl_regdb_get_ver_str,
};

