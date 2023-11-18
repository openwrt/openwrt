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
#include "phl_headers.h"
#include "phl_chnlplan.h"
#include "phl_chnlplan_6g.h"
#include "phl_country.h"

extern const struct chdef_6ghz chdef6g[MAX_CHDEF_6GHZ];
extern const struct regulatory_domain_mapping_6g rdmap6[MAX_RD_MAP_NUM_6GHZ];

static void _get_6ghz_ch_info(const struct chdef_6ghz *chdef,
	u8 group, u32 *ch, u32 *passive, u8 *max_num, u8 *ch_start)
{
	switch (group) {
	case FREQ_GROUP_6GHZ_UNII5:
		*ch = ((chdef->support_ch_u5[2] << 16) |
			(chdef->support_ch_u5[1] << 8) |
			(chdef->support_ch_u5[0]));
		*passive = ((chdef->passive_u5[2] << 16) |
			(chdef->passive_u5[1] << 8) |
			(chdef->passive_u5[0]));
		*max_num = MAX_CH_NUM_UNII5;
		*ch_start = 1;
		break;
	case FREQ_GROUP_6GHZ_UNII6:
		*ch = chdef->support_ch_u6;
		*passive = chdef->passive_u6;
		*max_num = MAX_CH_NUM_UNII6;
		*ch_start = 97;
		break;
	case FREQ_GROUP_6GHZ_UNII7:
		*ch = ((chdef->support_ch_u7[2] << 16) |
			(chdef->support_ch_u7[1] << 8) |
			(chdef->support_ch_u7[0]));
		*passive = ((chdef->passive_u7[2] << 16) |
			(chdef->passive_u7[1] << 8) |
			(chdef->passive_u7[0]));
		*max_num = MAX_CH_NUM_UNII7;
		*ch_start = 121;
		break;
	case FREQ_GROUP_6GHZ_UNII8:
		*ch = ((chdef->support_ch_u8[1] << 8) |
			(chdef->support_ch_u8[0]));
		*passive = ((chdef->passive_u8[1] << 8) |
			(chdef->passive_u8[0]));
		*max_num = MAX_CH_NUM_UNII8;
		*ch_start = 193;
		break;
	default:
		*ch = 0;
		*passive = 0;
		*max_num = 0;
		*ch_start = 0;
		break;
	}
}

void regu_convert_ch6g(u8 unii_6g, struct rtw_regulation *rg,
			u32 *ch_cnt, struct rtw_regulation_channel *rch,
			u32 ch, u32 passive, u8 max_num, u8 ch_start)
{
	u16 i = 0;
	u32 shift = 0;
	u8 property = 0;
	u32 cnt = 0;

	PHL_INFO("[REGU], convert 6 ghz unii-%d channels, from %d, ch=0x%x, passive = 0x%x \n",
			unii_6g, ch_start, ch, passive);

	for (i = 0; i < max_num; i++) {
		shift = (1 << i);
		if (ch & shift) {
			property = 0;
			rch[*ch_cnt].band = BAND_ON_6G;
			rch[*ch_cnt].channel = (u8)(ch_start + (i * 4));

			if (passive & shift)
				property |= CH_PASSIVE;
			if ((rch[*ch_cnt].channel % 16) == 5)
				property |= CH_PSC;

			rch[*ch_cnt].property = property;
			PHL_INFO("[REGU], ch: %d%s%s \n",
				rch[*ch_cnt].channel,
				((property & CH_PASSIVE) ? ", passive" : ""),
				((property & CH_PSC) ? ", psc" : ""));
			(*ch_cnt)++;
			cnt++;
		}
	}

	PHL_INFO("[REGU], converted channels : %d\n", cnt);
}

static void _update_psc_group(struct rtw_regulation *rg)
{
	u8 group = FREQ_GROUP_6GHZ_UNII5;
	struct rtw_regulation_chplan_group *plan = &rg->psc_6g;
	struct rtw_regulation_chplan_group *src = NULL;
	u32 i = 0, j = 0;

	plan->cnt = 0;
	for (i = 0; i < 4; i++) {
		group = (u8)(i + FREQ_GROUP_6GHZ_UNII5);
		src = &rg->chplan[group];

		for (j = 0; j < src->cnt; j++) {
			if (src->ch[j].property & CH_PSC) {
				plan->ch[plan->cnt].band =
					src->ch[j].band;
				plan->ch[plan->cnt].channel =
					src->ch[j].channel;
				plan->ch[plan->cnt].property =
					src->ch[j].property;
				plan->cnt++;
			}
		}
	}
}

static bool _chnlplan_update_6g(struct rtw_regulation *rg,
			u8 regulation, u8 ch_idx)
{
	const struct chdef_6ghz *chdef = NULL;
	struct rtw_regulation_chplan_group *plan = NULL;
	u8 group = FREQ_GROUP_6GHZ_UNII5;
	u8 max_num = 0, ch_start = 0;
	u16 i = 0;
	u32 ch = 0, passive = 0;
	u32 total = 0;

	if (regulation >= REGULATION_MAX)
		return false;

	for (i = 0; i < MAX_CHDEF_6GHZ; i++) {
		if (ch_idx == chdef6g[i].idx) {
			chdef = &chdef6g[i];
			break;
		}
	}

	if (!chdef)
		return false;

	rg->ch_idx6g = ch_idx;
	rg->regulation_6g = regulation;

	for (i = 0; i < 4; i++) {
		group = (u8)(i + FREQ_GROUP_6GHZ_UNII5);
		plan = &rg->chplan[group];
		plan->cnt = 0;
		_get_6ghz_ch_info(chdef, group,
			&ch, &passive, &max_num, &ch_start);
		regu_convert_ch6g((u8)(i + 5), rg, &plan->cnt, plan->ch,
			ch, passive, max_num, ch_start);
		total += plan->cnt;
	}

	_update_psc_group(rg);

	PHL_INFO("[REGU], 6 GHz, total channel = %d, regulation = %d\n",
					total, rg->regulation_6g);

	return true;
}

static u8 _domain_index_6g(u8 domain)
{
	u8 i = 0;

	for (i = 0; i < MAX_RD_MAP_NUM_6GHZ; i++) {
		if (domain == rdmap6[i].domain_code) {
			return i;
		}
	}

	return MAX_RD_MAP_NUM_6GHZ;
}

static bool _regulatory_domain_update_6g(struct rtw_regulation *rg,
			u8 domain, enum regulation_rsn reason)
{
	u8 regulation = REGULATION_NA;
	u8 ch_idx = 0, did = 0;

	rg->domain_6g.reason = reason;
	if (domain == RSVD_DOMAIN) {
		rg->domain_6g.code = RSVD_DOMAIN;
		return true;
	} else {
		/* Note: only valid domain index can reach here */
		did = _domain_index_6g(domain);
		rg->domain_6g.code = rdmap6[did].domain_code;
		regulation = rdmap6[did].regulation;
		ch_idx = rdmap6[did].ch_idx;
		return _chnlplan_update_6g(rg, regulation, ch_idx);
	}
}

static void _get_group_chplan_6g(struct rtw_regulation *rg,
			struct rtw_regulation_chplan_group *group,
			struct rtw_regulation_chplan *plan)
{
	u32 i = 0;

	for (i = 0; i < group->cnt; i++) {
		if (group->ch[i].channel) {
			plan->ch[plan->cnt].band =
				group->ch[i].band;
			plan->ch[plan->cnt].channel =
				group->ch[i].channel;
			plan->ch[plan->cnt].property =
				group->ch[i].property;
			plan->cnt++;
		}
	}
}

void regu_get_chnlplan_6g(struct rtw_regulation *rg,
				enum rtw_regulation_query type,
				struct rtw_regulation_chplan *plan)
{
	struct rtw_regulation_chplan_group *group = NULL;

	/* 6ghz */
	if (rg->capability & CAPABILITY_6GHZ) {
		/* unii5 */
		if (type & REGULQ_CHPLAN_6GHZ_UNII5) {
			group = &rg->chplan[FREQ_GROUP_6GHZ_UNII5];
			_get_group_chplan_6g(rg, group, plan);
		}
		/* unii6 */
		if (type & REGULQ_CHPLAN_6GHZ_UNII6) {
			group = &rg->chplan[FREQ_GROUP_6GHZ_UNII6];
			_get_group_chplan_6g(rg, group, plan);
		}
		/* unii7 */
		if (type & REGULQ_CHPLAN_6GHZ_UNII7) {
			group = &rg->chplan[FREQ_GROUP_6GHZ_UNII7];
			_get_group_chplan_6g(rg, group, plan);
		}
		/* unii8 */
		if (type & REGULQ_CHPLAN_6GHZ_UNII8) {
			group = &rg->chplan[FREQ_GROUP_6GHZ_UNII8];
			_get_group_chplan_6g(rg, group, plan);
		}
		/* psc */
		if (type & REGULQ_CHPLAN_6GHZ_PSC) {
			group = &rg->psc_6g;
			_get_group_chplan_6g(rg, group, plan);
		}
	}
}

bool regu_valid_domain_6g(u8 domain)
{
	if (domain == RSVD_DOMAIN)
		return true;

	if (_domain_index_6g(domain) >= MAX_RD_MAP_NUM_6GHZ)
		return false;

	return true;
}

/*
 * [Desc]: Set 6 ghz domain code
 *
 * [NOTE]: rg->lock need to be acquired before this function is called,
 */
bool regu_set_domain_6g(void *phl, u8 domain,
				       	enum regulation_rsn reason)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;

	PHL_INFO("[REGU], set 6 ghz domain code = 0x%x, reason = 0x%x\n",
			domain, reason);

	if (!phl_info)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	if (!regu_valid_domain_6g(domain))
		return false;

	if (_regulatory_domain_update_6g(rg, domain, reason))
		rg->valid_6g = true;
	else
		rg->valid_6g = false;

	PHL_INFO("[REGU], 6 ghz domain code valid = %d\n", rg->valid_6g);

	return rg->valid_6g;
}

static bool _query_regu_ch6g(u8 did, enum rtw_regulation_freq_group freq_gid,
	u8 ch, enum ch_property *prop)
{
	u8 idx6g = rdmap6[did].ch_idx;
	const struct chdef_6ghz *chdef6 = NULL;
	u16 i;
	u32 shift;

	for (i = 0; i < MAX_CHDEF_6GHZ; i++) {
		if (idx6g == chdef6g[i].idx) {
			chdef6 = &chdef6g[i];
			break;
		}
	}

	if (chdef6) {
		u32 ch_bmp, passive_bmp;
		u8 max_num, ch_start;

		_get_6ghz_ch_info(chdef6, freq_gid, &ch_bmp, &passive_bmp,
			&max_num, &ch_start);

		if (ch_bmp) {
			for (i = 0; i < max_num; i++) {
				shift = (1 << i);
				if ((ch_bmp & shift) && ch == ch_start + i * 4) {
					if (prop)
						*prop = (passive_bmp & shift) ? CH_PASSIVE : 0;
					return true;
				}
			}
		}
	}

	return false;
}


/*
 * @ Function description
 *	Use the domain code, band, ch  to query the corresponding
 *	regulation channel  and property
 *
 * @ parameter
 * 	domain : the specified domain code
 *    band : the specified band
 *    ch : the specified channel
 *	prop : if the regulation channel exist, the resulting property will
 *              be filled
 *
 * @ return :
 *	true : the queried regulation channel exist
 *	false : not exist
 *
 */
bool regu_query_domain_6g_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop)
{
	enum rtw_regulation_freq_group freq_gid;
	u8 did;

	if (band != BAND_ON_6G)
		return false;

	freq_gid = phl_get_regu_freq_group(band, ch);
	if (freq_gid < FREQ_GROUP_6GHZ_UNII5 || freq_gid > FREQ_GROUP_6GHZ_UNII8)
		return false;

	did = _domain_index_6g(domain);
	if (did >= MAX_RD_MAP_NUM_6GHZ)
		return false;

	return _query_regu_ch6g(did, freq_gid, ch, prop);
}

u8 regu_get_domain_regulation_6g(u8 domain)
{
	u8 did = MAX_RD_MAP_NUM_6GHZ;

	if (!regu_valid_domain_6g(domain))
		return REGULATION_MAX;

	did = _domain_index_6g(domain);
	if (did >= MAX_RD_MAP_NUM_6GHZ)
		return REGULATION_MAX;

	return rdmap6[did].regulation;
}


