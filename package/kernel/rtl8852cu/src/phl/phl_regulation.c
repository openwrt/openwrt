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
#include "phl_country.h"
#include "phl_regulation_6g.h"

extern const struct regulatory_domain_mapping rdmap[MAX_RD_MAP_NUM];
extern const struct chdef_2ghz chdef2g[MAX_CHDEF_2GHZ];
extern const struct chdef_5ghz chdef5g[MAX_CHDEF_5GHZ];
extern const struct country_domain_mapping cdmap[MAX_COUNTRY_NUM];

#define _set_country(_a_, _id_, _c_) \
	(_a_)[(_id_)] = (_c_)[0]; \
	(_a_)[(_id_) + 1] = (_c_)[1];

/*
 * @ Function description
 *	Convert 2 ghz channels from bit definition and then fill to
 *	struct rtw_regulation_channel *ch array[] and
 *	*ch_cnt will also be calculated.
 *
 * @ parameter
 *	*rg : internal regulatory information
 *	*ch_cnt : final converted 2ghz channel numbers.
 *	*rch : converted channels will be filled here.
 *	ch : 2 ghz bit difinitions
 *	passive : 2 ghz passive bit difinitions
 *
 */
static void _convert_ch2g(struct rtw_regulation *rg, u32 *ch_cnt,
	struct rtw_regulation_channel *rch, u16 ch, u16 passive)
{
	u8 i = 0, property = 0;
	u32 shift = 0, cnt = 0;

	PHL_INFO("[REGU], convert 2 ghz channels\n");

	for (i = 0; i < MAX_CH_NUM_2GHZ; i++) {
		property = 0;
		shift = (1 << i);
		if (ch & shift) {
			rch[*ch_cnt].band = BAND_ON_24G;
			rch[*ch_cnt].channel = (u8)(i + 1);

			if (passive & shift)
				property |= CH_PASSIVE;

			rch[*ch_cnt].property = property;
			(*ch_cnt)++;
			PHL_INFO("[REGU], ch: %d%s\n", (i + 1),
			((property & CH_PASSIVE) ? ", passive" : " " ));
			cnt++;
		}
	}

	PHL_INFO("[REGU], converted channels : %d\n", cnt);
}

static enum rtw_regulation_status _chnlplan_update_2g(
		struct rtw_regulation *rg, const struct freq_plan *f)
{
	const struct chdef_2ghz *chdef = NULL;
	struct rtw_regulation_chplan_group *plan = NULL;
	u16 i = 0, ch = 0, passive = 0;

	if (!f)
		return REGULATION_FAILURE;

	if (f->regulation >= REGULATION_MAX)
		return REGULATION_FAILURE;

	for (i = 0; i < MAX_CHDEF_2GHZ; i++) {
		if (f->ch_idx == chdef2g[i].idx) {
			chdef = &chdef2g[i];
			break;
		}
	}

	if (!chdef)
		return REGULATION_FAILURE;

	rg->ch_idx2g = f->ch_idx;
	rg->regulation_2g = f->regulation;

	plan = &rg->chplan[FREQ_GROUP_2GHZ];
	plan->cnt = 0;
	ch = ((chdef->support_ch[1] << 8) | (chdef->support_ch[0]));
	passive = ((chdef->passive[1] << 8) | (chdef->passive[0]));
	_convert_ch2g(rg, &plan->cnt, plan->ch, ch, passive);

	PHL_INFO("[REGU], 2 GHz, total channel = %d\n", plan->cnt);

	return REGULATION_SUCCESS;
}

static void _get_5ghz_ch_info(const struct chdef_5ghz *chdef,
	u8 group, u16 *ch, u16 *passive, u16 *dfs, u8 *max_num, u8 *ch_start)
{
	switch (group) {
	case FREQ_GROUP_5GHZ_BAND1:
		*ch = chdef->support_ch_b1;
		*passive = chdef->passive_b1;
		*dfs = chdef->dfs_b1;
		*max_num = MAX_CH_NUM_BAND1;
		*ch_start = 36;
		break;
	case FREQ_GROUP_5GHZ_BAND2:
		*ch = chdef->support_ch_b2;
		*passive = chdef->passive_b2;
		*dfs = chdef->dfs_b2;
		*max_num = MAX_CH_NUM_BAND2;
		*ch_start = 52;
		break;
	case FREQ_GROUP_5GHZ_BAND3:
		*ch = ((chdef->support_ch_b3[1] << 8) |
			(chdef->support_ch_b3[0]));
		*passive = ((chdef->passive_b3[1] << 8) |
			(chdef->passive_b3[0]));
		*dfs = ((chdef->dfs_b3[1] << 8) |
			(chdef->dfs_b3[0])) ;
		*max_num = MAX_CH_NUM_BAND3;
		*ch_start = 100;
		break;
	case FREQ_GROUP_5GHZ_BAND4:
		*ch = chdef->support_ch_b4;
		*passive = chdef->passive_b4;
		*dfs = chdef->dfs_b4;
		*max_num = MAX_CH_NUM_BAND4;
		*ch_start = 149;
		break;
	default:
		*ch = 0;
		*passive = 0;
		*dfs = 0;
		*max_num = 0;
		*ch_start = 0;
		break;
	}
}

/*
 * @ Function description
 *	Convert 5 ghz channels from bit definition and then fill to
 *	struct rtw_regulation_channel *ch array[] and
 *	*ch_cnt will also be calculated.
 *
 * @ parameter
 *	band_5g : 1~4 (5g band-1 ~ 5g band-4)
 *	*rg : internal regulatory information
 *	*ch_cnt : final converted 2ghz channel numbers.
 *	*rch : converted channels will be filled here.
 *	ch : 5 ghz bnad channel bit difinitions
 *	passive : 5 ghz band passive bit difinitions
 *	dfs : 5 ghz band dfs bit difinitions
 *	max_num : maximum channel numbers of the 5 ghz band.
 *	ch_start : start channel index of the 5 ghz band.
 */
static void _convert_ch5g(u8 band_5g, struct rtw_regulation *rg,
			u32 *ch_cnt, struct rtw_regulation_channel *rch,
			u16 ch, u16 passive, u16 dfs, u8 max_num, u8 ch_start)
{
	u16 i = 0;
	u32 shift = 0;
	u8 property = 0;
	u32 cnt = 0;

	PHL_INFO("[REGU], convert 5ghz band-%d channels, from %d, ch=0x%x, passive = 0x%x, dfs=0x%x \n",
			band_5g, ch_start, ch, passive, dfs);

	for (i = 0; i < max_num; i++) {
		shift = (1 << i);
		if (ch & shift) {
			property = 0;
			rch[*ch_cnt].band = BAND_ON_5G;
			rch[*ch_cnt].channel = (u8)(ch_start + (i * 4));

			if (passive & shift)
				property |= CH_PASSIVE;
			if (dfs & shift)
				property |= CH_DFS;

			rch[*ch_cnt].property = property;
			PHL_INFO("[REGU], ch: %d%s%s \n",
				rch[*ch_cnt].channel,
				((property & CH_PASSIVE) ? ", passive" : ""),
				((property & CH_DFS) ? ", dfs" : ""));
			(*ch_cnt)++;
			cnt++;
		}
	}

	PHL_INFO("[REGU], converted channels : %d\n", cnt);
}

static enum rtw_regulation_status _chnlplan_update_5g(
		struct rtw_regulation *rg, const struct freq_plan *f)
{
	const struct chdef_5ghz *chdef = NULL;
	struct rtw_regulation_chplan_group *plan = NULL;
	u8 group = FREQ_GROUP_5GHZ_BAND1;
	u8 max_num = 0, ch_start = 0;
	u16 i = 0, ch = 0, passive = 0, dfs = 0;
	u32 total = 0;

	if (!f)
		return REGULATION_FAILURE;

	if (f->regulation >= REGULATION_MAX)
		return REGULATION_FAILURE;

	for (i = 0; i < MAX_CHDEF_5GHZ; i++) {
		if (f->ch_idx == chdef5g[i].idx) {
			chdef = &chdef5g[i];
			break;
		}
	}

	if (!chdef)
		return REGULATION_FAILURE;

	rg->ch_idx5g = f->ch_idx;
	rg->regulation_5g = f->regulation;

	for (i = 0; i < 4; i++) {
		group = (u8)(i + FREQ_GROUP_5GHZ_BAND1);
		plan = &rg->chplan[group];
		plan->cnt = 0;
		_get_5ghz_ch_info(chdef, group,
			&ch, &passive, &dfs, &max_num, &ch_start);
		_convert_ch5g((u8)(i + 1), rg, &plan->cnt, plan->ch,
			ch, passive, dfs, max_num, ch_start);
		total += plan->cnt;
	}

	PHL_INFO("[REGU], 5 GHz, total channel = %d\n", total);

	return REGULATION_SUCCESS;
}

static enum rtw_regulation_status _regulatory_domain_update(
		struct rtw_regulation *rg, u8 did, enum regulation_rsn reason)
{
	enum rtw_regulation_status status = REGULATION_SUCCESS;
	const struct freq_plan *plan_2g = NULL;
	const struct freq_plan *plan_5g = NULL;

	plan_2g = &rdmap[did].freq_2g;
	plan_5g = &rdmap[did].freq_5g;

	rg->domain.code = rdmap[did].domain_code;
	rg->domain.reason = reason;

	status = _chnlplan_update_2g(rg, plan_2g);
		if (status != REGULATION_SUCCESS)
			return status;
	status = _chnlplan_update_5g(rg, plan_5g);
		if (status != REGULATION_SUCCESS)
			return status;

	return status;
}

static bool _available_ch(struct rtw_regulation *rg, u8 ch, u8 dfs)
{
	/*
	 * note, only 2/5 ghz channels can be checked by this
	 */
	if (!ch) {
		/* 0 is invalid ch index */
		return false;
	}

	if (!(rg->capability & CAPABILITY_DFS)) {
		if (dfs) {
			/* not support dfs, but it's a dfs ch */
			return false;
		}
	}

	if (!(rg->capability & CAPABILITY_59GHZ)) {
		if (CH_59GHZ(ch)) {
			/* not support 5.9 GHz ch, but it's a 5.9 ghz ch */
			return false;
		}
	}

	return true;
}

static void _get_group_chplan(struct rtw_regulation *rg,
			struct rtw_regulation_chplan_group *group,
			struct rtw_regulation_chplan *plan)
{
	u8 i = 0, ch = 0, dfs = 0;

	for (i = 0; i < group->cnt; i++) {
		dfs = ((group->ch[i].property & CH_DFS) ? 1 : 0);
		ch = group->ch[i].channel;

		if (_available_ch(rg, ch, dfs)) {
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

static u8 _domain_index(u8 domain)
{
	u8 i = 0;

	for (i = 0; i < MAX_RD_MAP_NUM; i++) {
		if (domain == rdmap[i].domain_code) {
			return i;
		}
	}

	return MAX_RD_MAP_NUM;
}

static u16 _country_index(char *cntry)
{
	u16 i = 0;

	for (i = 0; i < MAX_COUNTRY_NUM; i++) {
		if (cdmap[i].char2[0] == cntry[0] &&
			cdmap[i].char2[1] == cntry[1]) {
			return i;
		}
	}

	return MAX_COUNTRY_NUM;
}

static enum rtw_regulation_status _get_chnlplan(struct rtw_regulation *rg,
				enum rtw_regulation_query type,
				struct rtw_regulation_chplan *plan)
{
	struct rtw_regulation_chplan_group *group = NULL;

	if (rg->domain.code == INVALID_DOMAIN_CODE)
		return REGULATION_INVALID_DOMAIN;

	plan->cnt = 0;

	/* 2ghz */
	if (rg->capability & CAPABILITY_2GHZ) {
		if (type & REGULQ_CHPLAN_2GHZ) {
			group = &rg->chplan[FREQ_GROUP_2GHZ];
			_get_group_chplan(rg, group, plan);
		}
	}

	/* 5ghz */
	if (rg->capability & CAPABILITY_5GHZ) {
		/* band1 */
		if (type & REGULQ_CHPLAN_5GHZ_BAND1) {
			group = &rg->chplan[FREQ_GROUP_5GHZ_BAND1];
			_get_group_chplan(rg, group, plan);
		}
		/* band2 */
		if (type & REGULQ_CHPLAN_5GHZ_BAND2) {
			group = &rg->chplan[FREQ_GROUP_5GHZ_BAND2];
			_get_group_chplan(rg, group, plan);
		}
		/* band3 */
		if (type & REGULQ_CHPLAN_5GHZ_BAND3) {
			group = &rg->chplan[FREQ_GROUP_5GHZ_BAND3];
			_get_group_chplan(rg, group, plan);
		}
		/* band4 */
		if (type & REGULQ_CHPLAN_5GHZ_BAND4) {
			group = &rg->chplan[FREQ_GROUP_5GHZ_BAND4];
			_get_group_chplan(rg, group, plan);
		}
	}

	regu_get_chnlplan_6g(rg, type, plan);

	return REGULATION_SUCCESS;
}

static bool _valid_property(u8 property, u8 reject)
{
	u8 i = 0;

	/* accept all property */
	if (!reject)
		return true;

	/* check if ch property rejected */
	for (i = 0; i < 8; i++) {
		if ((BIT(i) & property) & reject)
			return false;
	}

	return true;
}

static void _filter_chnlplan(void *d,
			struct rtw_regulation_chplan *plan,
			struct rtw_chlist *filter)
{
	struct rtw_regulation_chplan inplan = {0};
	u16 i = 0, j = 0, k = 0;

	if (!d || !plan || !filter)
		return;

	if (plan->cnt < filter->cnt)
		return;

	_os_mem_cpy(d, &inplan, plan, sizeof(struct rtw_regulation_chplan));

	/*
	 * generate output chplan
	 * ex: filter : {1, 6}, inplan : {1, 6, 6, 11}, ouput => {1, 6, 6}
	 */
	plan->cnt = 0;
	for (i = 0; i < filter->cnt; i++) {
		for (j = 0; j < inplan.cnt; j++) {
			if ((filter->ch[i].band == inplan.ch[j].band) &&
				(filter->ch[i].ch == inplan.ch[j].channel)) {
				plan->ch[k].band = inplan.ch[j].band;
				plan->ch[k].channel = inplan.ch[j].channel;
				plan->ch[k].property = inplan.ch[j].property;
				k++;
				plan->cnt++;
			}
		}
	}
}

static bool _regulation_valid(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;
	bool valid = false;

	if (!phl)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	d = phl_to_drvpriv(phl_info);
	_os_spinlock(d, &rg->lock, _bh, NULL);
	valid = rg->valid;
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return valid;
}

static bool _query_channel(struct rtw_regulation *rg,
			enum band_type band, u16 channel,
			struct rtw_regulation_channel *ch)
{
	struct rtw_regulation_chplan_group *plan = NULL;
	u8 i = 0, j = 0;

	if ((BAND_2GHZ(band) && !(rg->capability & CAPABILITY_2GHZ)) ||
		(BAND_5GHZ(band) && !(rg->capability & CAPABILITY_5GHZ)) ||
		(BAND_6GHZ(band) && !(rg->capability & CAPABILITY_6GHZ)))
		return false;

	for (i = FREQ_GROUP_2GHZ; i < FREQ_GROUP_MAX; i++) {
		plan = &rg->chplan[i];
		for (j = 0; j < plan->cnt; j++) {
			if ((band == plan->ch[j].band) &&
				(channel == plan->ch[j].channel)) {

				if (i == FREQ_GROUP_5GHZ_BAND4) {
					if ((!(rg->capability & CAPABILITY_59GHZ)) &&
						CH_59GHZ(channel))
						return false;
				}

				ch->band = plan->ch[j].band;
				ch->channel = plan->ch[j].channel;
				ch->property = plan->ch[j].property;
				return true;
			}
		}
	}

	return false;
}

static void _display_chplan(struct rtw_regulation_chplan *plan)
{
	u16 i = 0;

	for (i = 0; i < plan->cnt; i++) {
		PHL_INFO("[REGU], %d, %shz: ch %d%s%s%s\n", (i + 1),
			((plan->ch[i].band == BAND_ON_24G) ? "2g" :
			((plan->ch[i].band == BAND_ON_5G) ? "5g" :
			((plan->ch[i].band == BAND_ON_6G) ? "6g" : ""))),
			(plan->ch[i].channel),
			((plan->ch[i].property & CH_PASSIVE) ?
						", passive" : ""),
			((plan->ch[i].property & CH_DFS) ? ", dfs" : ""),
			((plan->ch[i].property & CH_PSC) ? ", psc" : ""));
	}
}

static void _phl_regulation_send_msg(struct phl_info_t *phl_info, u8 evt_id)
{
	struct phl_msg msg = {0};
	msg.inbuf = NULL;
	msg.inlen = 0;
	msg.band_idx = HW_BAND_0;
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_REGU);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, evt_id);

	if (RTW_PHL_STATUS_SUCCESS != phl_msg_hub_send(phl_info, NULL, &msg))
		PHL_ERR("[REGULATION] sending message failed (evt_id: %u) \n", evt_id);
}

static void _get_5ghz_udef_ch_info(struct rtw_user_def_chplan *udef,
	u8 group, u16 *ch, u16 *passive, u16 *dfs, u8 *max_num, u8 *ch_start)
{
	switch (group) {
	case FREQ_GROUP_5GHZ_BAND1:
		*ch = (u16)udef->ch5g & 0xf;
		*passive = (u16)udef->passive5g & 0xf;
		*dfs = (u16)udef->dfs5g & 0xf;
		*max_num = MAX_CH_NUM_BAND1;
		*ch_start = 36;
		break;
	case FREQ_GROUP_5GHZ_BAND2:
		*ch = (u16)((udef->ch5g & 0xf0) >> 4);
		*passive = (u16)((udef->passive5g & 0xf0) >> 4);
		*dfs = (u16)((udef->dfs5g & 0xf0) >> 4);
		*max_num = MAX_CH_NUM_BAND2;
		*ch_start = 52;
		break;
	case FREQ_GROUP_5GHZ_BAND3:
		*ch = (u16)((udef->ch5g & 0xfff00) >> 8);
		*passive = (u16)((udef->passive5g & 0xfff00) >> 8);
		*dfs = (u16)((udef->dfs5g & 0xfff00) >> 8);
		*max_num = MAX_CH_NUM_BAND3;
		*ch_start = 100;
		break;
	case FREQ_GROUP_5GHZ_BAND4:
		*ch = (u16)((udef->ch5g & 0xff00000) >> 20);
		*passive = (u16)((udef->passive5g & 0xff00000) >> 20);
		*dfs = (u16)((udef->dfs5g & 0xff00000) >> 20);
		*max_num = MAX_CH_NUM_BAND4;
		*ch_start = 149;
		break;
	default:
		*ch = 0;
		*passive = 0;
		*dfs = 0;
		*max_num = 0;
		*ch_start = 0;
		break;
	}
}

static void _get_6ghz_udef_ch_info(struct rtw_user_def_chplan *udef,
	u8 group, u32 *ch, u32 *passive, u8 *max_num, u8 *ch_start)
{
	switch (group) {
	case FREQ_GROUP_6GHZ_UNII5:
		*ch = (u32)udef->ch6g & 0xffffff;
		*passive = (u32)udef->passive6g & 0xffffff;
		*max_num = MAX_CH_NUM_UNII5;
		*ch_start = 1;
		break;
	case FREQ_GROUP_6GHZ_UNII6:
		*ch = (u32)((udef->ch6g & 0x3f000000) >> 24);
		*passive = (u32)((udef->passive6g & 0x3f000000) >> 24);
		*max_num = MAX_CH_NUM_UNII6;
		*ch_start = 97;
		break;
	case FREQ_GROUP_6GHZ_UNII7:
		*ch = (u32)((udef->ch6g & 0xffffc0000000) >> 30);
		*passive = (u32)((udef->passive6g & 0xffffc0000000) >> 30);
		*max_num = MAX_CH_NUM_UNII7;
		*ch_start = 121;
		break;
	case FREQ_GROUP_6GHZ_UNII8:
		*ch = (u32)((udef->ch6g & 0x7ff000000000000) >> 48);
		*passive = (u32)((udef->passive6g & 0x7ff000000000000) >> 48);
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

/*
 * @ Function description
 *	Reset regulatory info for non-specific country
 */
static void _reset_for_non_specific_country(struct rtw_regulation *rg)
{
	/* reset country */
	rg->country[0] = 0;
	rg->country[1] = 0;

	/* reset TPO */
	rg->tpo = TPO_NA;

	/* default support all */
	rg->support_mode |= (SUPPORT_11B | SUPPORT_11G | SUPPORT_11N |
				SUPPORT_11A | SUPPORT_11AC | SUPPORT_11AX);
}

/*
 * @ Function description
 *	Set user defined channel plans
 *
 * @ parameter
 *	struct rtw_user_def_chplan *udef : user defined channels, bit definition
 *
 * @ return :
 *	true : if set successfully
 *	false : failed to set
 *
 */
bool rtw_phl_set_user_def_chplan(void *phl, struct rtw_user_def_chplan *udef)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regulation_chplan_group *plan = NULL;
	u8 max_num = 0, ch_start = 0;
	u16 ch = 0, passive = 0, dfs = 0;
	u32 ch_6g = 0, passive_6g = 0;
	u8 group = FREQ_GROUP_5GHZ_BAND1;
	void *d = NULL;
	u32 i = 0;

	if (!phl || !udef)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	if (rg->domain.code != RSVD_DOMAIN) {
		PHL_INFO("[REGU], Only reserved domain can set udef channel plan \n");
		return false;
	}

	PHL_INFO("[REGU], set udef channel plan, ch2g:0x%x, ch5g:0x%x\n",
			udef->ch2g, udef->ch5g);

	d = phl_to_drvpriv(phl_info);
	_os_spinlock(d, &rg->lock, _bh, NULL);

	rg->regulation_2g = (u8)udef->regulatory_idx;
	rg->regulation_5g = (u8)udef->regulatory_idx;
	rg->regulation_6g = (u8)udef->regulatory_idx;
	rg->tpo = udef->tpo;

	/* 2 ghz */
	plan = &rg->chplan[FREQ_GROUP_2GHZ];
	plan->cnt = 0;
	ch = udef->ch2g;
	passive = udef->passive2g;
	_convert_ch2g(rg, &plan->cnt, plan->ch, ch, passive);

	PHL_INFO("[REGU], 2 GHz, total channel = %d\n", plan->cnt);

	/* 5 ghz */
	for (i = 0; i < 4; i++) {
		group = (u8)(i + FREQ_GROUP_5GHZ_BAND1);
		plan = &rg->chplan[group];
		plan->cnt = 0;
		_get_5ghz_udef_ch_info(udef, group,
			&ch, &passive, &dfs, &max_num, &ch_start);
		_convert_ch5g((u8)(i + 1), rg, &plan->cnt, plan->ch,
			ch, passive, dfs, max_num, ch_start);
	}

	/* 6 ghz */
	for (i = 0; i < 4; i++) {
		group = (u8)(i + FREQ_GROUP_6GHZ_UNII5);
		plan = &rg->chplan[group];
		plan->cnt = 0;
		_get_6ghz_udef_ch_info(udef, group,
			&ch_6g, &passive_6g, &max_num, &ch_start);
		PHL_INFO("[REGU], ch_6g = %u\n", ch_6g);
		regu_convert_ch6g((u8)(i + 5), rg, &plan->cnt, plan->ch,
			ch_6g, passive_6g, max_num, ch_start);
	}

	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return true;
}


/*
 * @ Function description
 *	Check if domain is valid or not
 *
 * @ parameter
 *	domain : domain code to query
 *
 * @ return :
 *	true : if domain code exists in data base
 *	false : invalid domain code
 *
 */
bool rtw_phl_valid_regulation_domain(u8 domain)
{
	if (domain == RSVD_DOMAIN)
		return true;

	if (_domain_index(domain) >= MAX_RD_MAP_NUM)
		return false;

	return true;
}

bool rtw_phl_valid_regulation_domain_6ghz(u8 domain)
{
	return regu_valid_domain_6g(domain);
}

/*
 * @ Function description
 *	Set regulatory domain code
 *
 * @ parameter
 *	phl : struct phl_info_t *
 *	domain : domain code
 *	reason : why
 *
 * @ return :
 *	true : set domain successfully
 *	false : set fail
 *
 */
bool rtw_phl_regulation_set_domain(void *phl, u8 domain,
				       	enum regulation_rsn reason)
{
	enum rtw_regulation_status status = REGULATION_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;
	u8 did = MAX_RD_MAP_NUM;

	PHL_INFO("[REGU], set domain code = 0x%x, reason = 0x%x\n",
			domain, reason);

	if (!phl_info)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	if (!rtw_phl_valid_regulation_domain(domain))
		return false;

	did = _domain_index(domain);

	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);

	if (domain == RSVD_DOMAIN) {
		rg->domain.code = RSVD_DOMAIN;
		rg->domain.reason = reason;
		status = REGULATION_SUCCESS;
	} else
		status = _regulatory_domain_update(rg, did, reason);

	if (status == REGULATION_SUCCESS) {
		_reset_for_non_specific_country(rg);
		rg->valid = true;
		regu_set_domain_6g(phl, 0x7f, reason);
	} else {
		rg->valid = false;
	}
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	PHL_INFO("[REGU], domain code update status = 0x%x\n", status);

	if (status == REGULATION_SUCCESS) {
		_phl_regulation_send_msg(phl_info, MSG_EVT_REGU_SET_DOMAIN);
		return true;
	} else {
		return false;
	}
}

/*
 * [Desc]: Set original 2/5 ghz domain code
 *
 * [NOTE]: rg->lock need to be acquired before this function is called,
 */
static bool _set_domain_2g_5g(void *phl, u8 domain,
				enum regulation_rsn reason)
{
	enum rtw_regulation_status status = REGULATION_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	u8 did = MAX_RD_MAP_NUM;

	PHL_INFO("[REGU], set 2/5 ghz domain code = 0x%x, reason = 0x%x\n",
			domain, reason);

	if (!phl_info)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	if (!rtw_phl_valid_regulation_domain(domain))
		return false;

	did = _domain_index(domain);

	if (domain == RSVD_DOMAIN) {
		rg->domain.code = RSVD_DOMAIN;
		rg->domain.reason = reason;
		status = REGULATION_SUCCESS;
	} else
		status = _regulatory_domain_update(rg, did, reason);

	PHL_INFO("[REGU], 2/5 ghz domain code update status = 0x%x\n", status);

	if (status == REGULATION_SUCCESS)
		rg->valid = true;
	else
		rg->valid = false;

	return rg->valid;
}

/*
 * @ Function description
 *	Set regulatory domain code for tri-bands 2, 5 and 6 GHz
 *
 * @ phl : struct phl_info_t *
 *	domain : struct rtw_regulatory_domain
 *	reason : why
 *
 * @ return :
 *	true : set domain successfully
 *	false : set fail
 *
 */
bool rtw_phl_regulation_set_domain_ex(void *phl,
					struct rtw_regulatory_domain *domain,
				       	enum regulation_rsn reason)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;
	u8 dm_2g_5g = 0, dm_6g = 0;

	PHL_INFO("[REGU], set domain code = 0x%x/ 0x%x, reason = 0x%x\n",
			domain->domain, domain->domain_6g, reason);

	if (!phl_info || !domain)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	dm_2g_5g = domain->domain;
	dm_6g = domain->domain_6g;

	_os_spinlock(d, &rg->lock, _bh, NULL);

	if (!_set_domain_2g_5g(phl, dm_2g_5g, reason) ||
		!regu_set_domain_6g(phl, dm_6g, reason)) {
		_os_spinunlock(d, &rg->lock, _bh, NULL);
		return false;
	}
	_reset_for_non_specific_country(rg);
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	_phl_regulation_send_msg(phl_info, MSG_EVT_REGU_SET_DOMAIN);
	return true;
}

/*
 * @ Function description
 *	Set regulation by 2bytes country code
 *
 * @ parameter
 *	phl : struct phl_info_t *
 *	country : 2 bytes char
 *	reason : why
 *
 * @ return :
 *	true : set country/domain successfully
 *	false : set fail
 *
 */
bool rtw_phl_regulation_set_country(void *phl, char *country,
					enum regulation_rsn reason)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regulatory_domain rdm = {0};
	void *d = NULL;
	u32 i = 0;

	PHL_INFO("[REGU], set country code = \"%c%c\", reason = 0x%x\n",
			country[0], country[1], reason);

	if (!phl_info)
		return false;

	d = phl_to_drvpriv(phl_info);
	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	if (rg->domain.code == RSVD_DOMAIN)
		return false;

	for (i = 0; i < MAX_COUNTRY_NUM; i++) {
		if (cdmap[i].char2[0] == country[0] &&
			cdmap[i].char2[1] == country[1] ) {
			rdm.domain = cdmap[i].domain_code;
			rdm.domain_6g = cdmap[i].domain_code_6g;
			if (!rtw_phl_regulation_set_domain_ex(
						phl, &rdm, reason))
				return false;
			_os_spinlock(d, &rg->lock, _bh, NULL);
			rg->country[0] = country[0];
			rg->country[1] = country[1];
			rg->tpo = cdmap[i].tpo;
			rg->support_mode = 0;
			if(cdmap[i].support & BIT(0))
				rg->support_mode |= (SUPPORT_11B | SUPPORT_11G
							| SUPPORT_11N);
			if(cdmap[i].support & BIT(1))
				rg->support_mode |= (SUPPORT_11A);
			if(cdmap[i].support & BIT(2))
				rg->support_mode |= (SUPPORT_11AC);
			if(cdmap[i].support & BIT(3))
				rg->support_mode |= (SUPPORT_11AX);
			_os_spinunlock(d, &rg->lock, _bh, NULL);
			return true;
		}
	}

	PHL_INFO("[REGU], country mismatch !!\n");
	return false;
}

/*
 * @ Function description
 *	Query current regulation channel plan
 *
 * @ parameter
 *	phl : struct phl_info_t *
 *	capability : enum rtw_regulation_capability
 *
 * @ return :
 *	true : set capability successfully
 *	false : set capability fail
 *
 */
bool rtw_phl_regulation_set_capability(void *phl,
		enum rtw_regulation_capability capability)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;

	PHL_INFO("[REGU], set capability = 0x%x \n", capability);

	if (!phl_info)
		return false;

	rg = &phl_info->regulation;
	if (!rg->init)
		return false;

	d = phl_to_drvpriv(phl_info);
	_os_spinlock(d, &rg->lock, _bh, NULL);
	rg->capability = capability;
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	PHL_INFO("[REGU], set capability = 0x%x successfully !!\n",
			rg->capability);
	return true;
}

/*
 * @ Function description
 *	Query current regulation channel plan
 *
 * @ parameter
 *	phl : struct phl_info_t *
 *	type : enum rtw_regulation_query, different query type
 * 	filter : struct rtw_chlist *, used to filter regulation channels
 *	plan : struct rtw_regulation_chplan *, query result will be filled here
 *		- result will be the intersection of regulation channel plan and
 *		   the filter channels.
 *
 * @ return :
 *	true : regulation query successfully, caller can check result
 *		by input parameter *plan.
 *	false : regulation query fail
 *
 */
bool rtw_phl_regulation_query_chplan(
			void *phl, enum rtw_regulation_query type,
			struct rtw_chlist *filter,
			struct rtw_regulation_chplan *plan)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	enum rtw_regulation_status status = REGULATION_FAILURE;
	void *d = NULL;

	if (!phl || !plan)
		return false;

	if (!_regulation_valid(phl))
		return false;

	rg = &phl_info->regulation;
	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);
	status = _get_chnlplan(rg, type, plan);
	if (filter)
		_filter_chnlplan(d, plan, filter);
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	if (status == REGULATION_SUCCESS) {
		/* _display_chplan(plan); */
		return true;
	}
	else
		return false;
}

/*
 * @ Function description
 *	Query specific regulation channel plan by domain code
 *
 * @ parameter
 * 	domain : domain code
 *	plan : struct rtw_regulation_chplan *, query result will be filled here
 *
 * @ return :
 *	true : regulation query successfully, caller can check result
 *		by input parameter *plan.
 *	false : regulation query fail
 *
 */
bool rtw_phl_query_specific_chplan(void *phl, u8 domain,
			struct rtw_regulation_chplan *plan)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	const struct chdef_2ghz *chdef2 = NULL;
	const struct chdef_5ghz *chdef5 = NULL;
	struct rtw_regulation *rg = NULL;
	u8 did = MAX_RD_MAP_NUM;
	u8 idx2g = INVALID_CHDEF;
	u8 idx5g = INVALID_CHDEF;
	u16 i = 0, ch = 0, passive = 0, dfs = 0;
	u8 group = FREQ_GROUP_5GHZ_BAND1;
	u8 max_num = 0, ch_start = 0;

	if (!plan)
		return false;
	plan->cnt = 0;

	PHL_INFO("[REGU], query specific channel plan for domain : 0x%x!!\n",
			domain);

	if (!rtw_phl_valid_regulation_domain(domain))
		return false;

	/* find channel definition for 2 ghz & 5 ghz */
	did = _domain_index(domain);
	idx2g = rdmap[did].freq_2g.ch_idx;
	for (i = 0; i < MAX_CHDEF_2GHZ; i++) {
		if (idx2g == chdef2g[i].idx) {
			chdef2 = &chdef2g[i];
		}
	}
	idx5g = rdmap[did].freq_5g.ch_idx;
	for (i = 0; i < MAX_CHDEF_5GHZ; i++) {
		if (idx5g == chdef5g[i].idx) {
			chdef5 = &chdef5g[i];
		}
	}

	/* when regulatory domain & capability is set, check regulatory capability setting first */
	if (_regulation_valid(phl)) {
		rg = &phl_info->regulation;
		if (!(rg->capability & CAPABILITY_2GHZ))
			chdef2 = NULL;
		if (!(rg->capability & CAPABILITY_5GHZ))
			chdef5 = NULL;
	}

	/* 2ghz */
	if (chdef2) {
		ch = ((chdef2->support_ch[1] << 8) |
			(chdef2->support_ch[0]));
		passive = ((chdef2->passive[1] << 8) |
			(chdef2->passive[0]));
		_convert_ch2g(rg, &plan->cnt, plan->ch, ch, passive);
	}

	/* 5ghz */
	if (chdef5) {
		for (i = 0; i < 4; i++) {
			group = (u8)(i + FREQ_GROUP_5GHZ_BAND1);
			_get_5ghz_ch_info(chdef5, group, &ch, &passive, &dfs,
						&max_num, &ch_start);
			_convert_ch5g((u8)(i + 1), rg, &plan->cnt, plan->ch,
				ch, passive, dfs, max_num, ch_start);
		}
	}

	PHL_INFO("[REGU], query specific channel plan for domain : 0x%x, total channels : %d !!\n",
			domain, plan->cnt);
	_display_chplan(plan);

	return true;
}

/*
 * @ Function description
 *	Query basic regulation info
 *
 * @ parameter
 * 	phl : struct phl_info_t *
 *	info : struct rtw_regulation_info *, query result will be filled here
 *
 * @ return :
 *	true : regulation query successfully, caller can check result
 *		by input parameter *info.
 *	false : regulation query fail
 *
 */
bool rtw_phl_query_regulation_info(void *phl, struct rtw_regulation_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;

	if (!phl || !info)
		return false;

	if (!_regulation_valid(phl))
		return false;

	rg = &phl_info->regulation;
	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);

	info->domain_code = (u8)rg->domain.code;
	info->domain_code_6g = (u8)rg->domain_6g.code;
	info->domain_reason = rg->domain.reason;
	info->domain_reason_6g = rg->domain_6g.reason;
	info->country[0] = rg->country[0];
	info->country[1] = rg->country[1];
	info->tpo = rg->tpo;
	info->support_mode = rg->support_mode;
	info->regulation_2g = rg->regulation_2g;
	info->regulation_5g = rg->regulation_5g;
	info->regulation_6g = rg->regulation_6g;
	info->chplan_ver = REGULATION_CHPLAN_VERSION;
	info->country_ver = REGULATION_COUNTRY_VERSION;
	info->capability = rg->capability;

	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return true;
}

enum rtw_regulation_freq_group phl_get_regu_freq_group(enum band_type band, u8 ch)
{
	if (BAND_2GHZ(band))
		return FREQ_GROUP_2GHZ;
	else if (BAND_5GHZ(band)) {
		if (CH_5GHZ_BAND1(ch))
			return FREQ_GROUP_5GHZ_BAND1;
		else if (CH_5GHZ_BAND2(ch))
			return FREQ_GROUP_5GHZ_BAND2;
		else if (CH_5GHZ_BAND3(ch))
			return FREQ_GROUP_5GHZ_BAND3;
		else if (CH_5GHZ_BAND4(ch))
			return FREQ_GROUP_5GHZ_BAND4;
	} else if (BAND_6GHZ(band)) {
		if (CH_6GHZ_UNII5(ch))
			return FREQ_GROUP_6GHZ_UNII5;
		else if (CH_6GHZ_UNII6(ch))
			return FREQ_GROUP_6GHZ_UNII6;
		else if (CH_6GHZ_UNII7(ch))
			return FREQ_GROUP_6GHZ_UNII7;
		else if (CH_6GHZ_UNII8(ch))
			return FREQ_GROUP_6GHZ_UNII8;
	}
	return FREQ_GROUP_MAX;
}

static bool _query_regu_ch2g(u8 did, u8 ch, enum ch_property *prop)
{
	u8 idx2g = rdmap[did].freq_2g.ch_idx;
	const struct chdef_2ghz *chdef2 = NULL;
	u8 i;

	for (i = 0; i < MAX_CHDEF_2GHZ; i++) {
		if (idx2g == chdef2g[i].idx) {
			chdef2 = &chdef2g[i];
			break;
		}
	}

	if (chdef2) {
		u16 ch_bmp = ((chdef2->support_ch[1] << 8) |
			(chdef2->support_ch[0]));

		if (ch_bmp) {
			u16 passive_bmp = ((chdef2->passive[1] << 8) |
				(chdef2->passive[0]));
			u32 shift;

			for (i = 0; i < MAX_CH_NUM_2GHZ; i++) {
				shift = (1 << i);
				if ((ch_bmp & shift) && ch == i + 1) {
					if (prop)
						*prop = (passive_bmp & shift) ? CH_PASSIVE : 0;
					return true;
				}
			}
		}
	}

	return false;
}

static bool _query_regu_ch5g(u8 did, enum rtw_regulation_freq_group freq_gid,
	u8 ch, enum ch_property *prop)
{
	u8 idx5g = idx5g = rdmap[did].freq_5g.ch_idx;
	const struct chdef_5ghz *chdef5 = NULL;
	u16 i;

	for (i = 0; i < MAX_CHDEF_5GHZ; i++) {
		if (idx5g == chdef5g[i].idx) {
			chdef5 = &chdef5g[i];
			break;
		}
	}

	if (chdef5) {
		u16 ch_bmp, passive_bmp, dfs_bmp;
		u8 max_num, ch_start;

		_get_5ghz_ch_info(chdef5, freq_gid, &ch_bmp, &passive_bmp, &dfs_bmp,
			&max_num, &ch_start);

		if (ch_bmp) {
			u32 shift;

			for (i = 0; i < max_num; i++) {
				shift = (1 << i);
				if ((ch_bmp & shift) && ch == ch_start + i * 4) {
					if (prop) {
						*prop = ((passive_bmp & shift) ? CH_PASSIVE : 0)
							| ((dfs_bmp & shift) ? CH_DFS : 0);
					}
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
 *	regulation channel and property
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
bool rtw_phl_query_domain_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop)
{
	enum rtw_regulation_freq_group freq_gid;
	u8 did;

	if (band != BAND_ON_24G && band != BAND_ON_5G)
		return false;

	freq_gid = phl_get_regu_freq_group(band, ch);
	if (freq_gid < FREQ_GROUP_2GHZ || freq_gid > FREQ_GROUP_5GHZ_BAND4)
		return false;

	did = _domain_index(domain);
	if (did >= MAX_RD_MAP_NUM)
		return false;

	if (band == BAND_ON_24G)
		return _query_regu_ch2g(did, ch, prop);
	else if (band == BAND_ON_5G)
		return _query_regu_ch5g(did, freq_gid, ch, prop);
	return false;
}

bool rtw_phl_query_domain_6g_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop)
{
	return regu_query_domain_6g_channel(domain, band, ch, prop);
}

/*
 * @ Function description
 *	Use the coutry code to query the corresponding
 *	domain code and properties
 *
 * @ parameter
 *	country : 2 bytes char
 *	country_chplan : pointer to structre of chplan's info
 *
 * @ return :
 *	true : successfully search the entry form cdmap
 *	false : country chplan query fail
 */
bool rtw_phl_query_country_chplan(char *country,
	struct rtw_regulation_country_chplan* country_chplan)
{
	u32 i = 0;

	for (i = 0; i < MAX_COUNTRY_NUM; i++) {
		if (cdmap[i].char2[0] == country[0] &&
			cdmap[i].char2[1] == country[1] ) {
			country_chplan->domain_code = cdmap[i].domain_code;
			country_chplan->domain_code_6g =
					cdmap[i].domain_code_6g;
			if(cdmap[i].support & BIT(0))
				country_chplan->support_mode |=
				(SUPPORT_11B | SUPPORT_11G | SUPPORT_11N);
			if(cdmap[i].support & BIT(1))
				country_chplan->support_mode |= (SUPPORT_11A);
			if(cdmap[i].support & BIT(2))
				country_chplan->support_mode |= (SUPPORT_11AC);
			if(cdmap[i].support & BIT(3))
				country_chplan->support_mode |= (SUPPORT_11AX);
			country_chplan->tpo = cdmap[i].tpo;
			return true;
		}
	}
	return false;
}

bool rtw_phl_query_cntry_exist(char *cntry)
{
	bool exist = false;

	if (_country_index(cntry) < MAX_COUNTRY_NUM)
		exist = true;

	return exist;
}

bool rtw_phl_query_group_list(void *phl, char *query_list, u32 max_item_num, u8 group_id)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regu_policy policy = phl_info->regulation.policy;
	u32 i = 0, item_num = 0;

	PHL_INFO("[REGU], rtw_phl_query_group_list, group id = %d \n", group_id);

	if (!query_list)
		return false;

	switch (group_id) {
	case DEFAULT_SUPPORT_6G:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].domain_code_6g != 0x00) {
				if (item_num < max_item_num) {
					_set_country(query_list, 2 * item_num,
						cdmap[i].char2);
					item_num++;
				} else {
					PHL_INFO("[REGU], buffer not enough \n");
					return false;
				}
			}
		}
		break;
	case CURRENT_SUPPORT_6G:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].domain_code_6g != 0x00 &&
				!(policy.cp_6g_bp[i] & CP_6G_BAND_BLOCKED)) {
				if (item_num < max_item_num) {
					_set_country(query_list, 2 * item_num,
						cdmap[i].char2);
					item_num++;
				} else {
					PHL_INFO("[REGU], buffer not enough \n");
					return false;
				}
			}
		}
		break;
	case EU_GROUP:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].country_property == CNTRY_EU) {
				if (item_num < max_item_num) {
					_set_country(query_list, 2 * item_num,
						cdmap[i].char2);
					item_num++;
				} else {
					PHL_INFO("[REGU], buffer not enough \n");
					return false;
				}
			}
		}
		break;
	case FCC_GROUP:
	{
		u8 domain = 0;
		u8 did = MAX_RD_MAP_NUM;
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			domain = cdmap[i].domain_code;
			did = _domain_index(domain);
			if (rdmap[did].freq_2g.regulation == REGULATION_FCC) {
				if (item_num < max_item_num) {
					_set_country(query_list, 2 * item_num,
						cdmap[i].char2);
					item_num++;
				} else {
					PHL_INFO("[REGU], buffer not enough \n");
					return false;
				}
			}
		}
	}
		break;
	}
	return true;
}

u32 rtw_phl_query_group_length(void *phl, u8 group_id)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regu_policy policy = phl_info->regulation.policy;
	u32 i = 0, item_num = 0;

	PHL_INFO("[REGU], rtw_phl_query_group_length, group id = %d \n", group_id);

	switch (group_id) {
	case DEFAULT_SUPPORT_6G:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].domain_code_6g != 0x00)
				item_num++;
		}
		break;
	case CURRENT_SUPPORT_6G:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].domain_code_6g != 0x00 &&
				!(policy.cp_6g_bp[i] & CP_6G_BAND_BLOCKED))
				item_num++;
		}
		break;
	case EU_GROUP:
		for (i = 0; i < MAX_COUNTRY_NUM; i++) {
			if (cdmap[i].country_property == CNTRY_EU)
				item_num++;
		}
		break;
	case FCC_GROUP:
		{
			u8 domain = 0;
			u8 did = MAX_RD_MAP_NUM;
			for (i = 0; i < MAX_COUNTRY_NUM; i++) {
				domain = cdmap[i].domain_code;
				did = _domain_index(domain);
				if (rdmap[did].freq_2g.regulation == REGULATION_FCC)
					item_num++;
			}
		}
		break;
	}
	return item_num;
}

static bool _set_cntry_prop_6g_bp(u8 mode, u8 *prop)
{
	bool ret = true;

	if (mode == ALLOW_MODE) {
		/* allow mode default set blocked bit */
		if (!(*prop & CP_6G_BAND_BLOCKED)) /* found already clear */
			ret = false;
		else
			*prop &= ~CP_6G_BAND_BLOCKED; /* clear blocked bit */
	} else {
		/* block mode default clear blocked bit */
		if (*prop & CP_6G_BAND_BLOCKED) /* found already set */
			ret = false;
		else
			*prop |= CP_6G_BAND_BLOCKED; /* set blocked bit */
	}

	return ret;
}

static bool _update_cntry_prop_6g_bp(
		struct rtw_regu_policy *policy,
		u8 mode,
		char *country)
{
	u16 idx = MAX_COUNTRY_NUM;
	bool ret = false;

	idx = _country_index(country);
	if (idx < MAX_COUNTRY_NUM) {
		/* found matching idx, update country property */
		if (_set_cntry_prop_6g_bp(mode, &policy->cp_6g_bp[idx])) {
			PHL_INFO("%s, matched: (%c%c).\n",
				__func__, country[0], country[1]);
			ret = true;
		} else {
			/* already set */
			PHL_INFO("%s, [SKIP] duplicated matched: (%c%c).\n",
				__func__, country[0], country[1]);
		}
	} else {
		/* country unrecognized (no match) */
		PHL_INFO("%s, [SKIP] unrecognized: (%c%c), hex(%02x,%02x).\n",
				__func__, country[0], country[1],
				(u8)country[0], (u8)country[1]);
	}

	return ret;
}

static void _reset_regu_policy_6g_bp(u8 mode,
	struct rtw_regu_policy *policy)
{
	u16 i = 0;

	for (i = 0; i < MAX_COUNTRY_NUM; i++) {
		policy->cp_6g_bp[i] |= CP_6G_BAND_SOFTAP_BLOCKED;
		if (mode == ALLOW_MODE) /* allow mode, default all blocked */
			policy->cp_6g_bp[i] |= CP_6G_BAND_BLOCKED;
		else /* block mode, default all allowed */
			policy->cp_6g_bp[i] &= ~CP_6G_BAND_BLOCKED;
	}
	policy->valid_6g_bp = false;
}

bool rtw_phl_regu_policy_set_6g_bp(void *phl,
	u8 mode,
	u8 *cntry_list, /* List of ISO 3166-2 country codes */
	u32 len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regu_policy *policy = NULL;
	u32 remain_len = 0, cntry_offset = 0;
	u16 cntry_cnt = 0, actual_cnt = 0;
	bool ret = false;
	void *d = NULL;

	if (mode != BLOCK_MODE && mode != ALLOW_MODE) {
		PHL_ERR("%s, mode(%d) invalid.\n",
			__func__, mode);
		goto exit;
	}

	if (!phl)
		goto exit;

	rg = &phl_info->regulation;
	if (!rg->init)
		goto exit;

	d = phl_to_drvpriv(phl_info);
	_os_spinlock(d, &rg->lock, _bh, NULL);

	policy = &phl_info->regulation.policy;

	/* reset 6g band prop */
	_reset_regu_policy_6g_bp(mode, policy);

	/* parse country list */
	remain_len = len;
	while (remain_len >= 2) {
		cntry_offset = 2*cntry_cnt;
		if (_update_cntry_prop_6g_bp(policy,
				mode, (char*)&cntry_list[cntry_offset]))
			actual_cnt++;
		remain_len -= 2;
		cntry_cnt++;
	}
	/* set valid */
	policy->valid_6g_bp = true;

	_os_spinunlock(d, &rg->lock, _bh, NULL);

	PHL_INFO("%s, Done: mode(%d), len(%d), actual_cnt(%d)/cnt(%d).\n",
			__func__, mode, len, actual_cnt, cntry_cnt);
	ret = true;
exit:
	return ret;
}

bool rtw_phl_regu_policy_query_info(void *phl,
		struct rtw_regu_policy_info *info,
		char *country)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regu_policy *policy = NULL;
	u16 idx = MAX_COUNTRY_NUM;
	bool ret = false;
	void *d = NULL;

	if (!phl || !info)
		goto exit;

	rg = &phl_info->regulation;
	if (!rg->init)
		goto exit;

	d = phl_to_drvpriv(phl_info);
	_os_spinlock(d, &rg->lock, _bh, NULL);

	policy = &phl_info->regulation.policy;

	idx = _country_index(country);
	if (idx < MAX_COUNTRY_NUM) {
		info->valid_6g_bp = policy->valid_6g_bp;
		info->cp_6g_bp = policy->cp_6g_bp[idx];
		info->valid_5g_bp = policy->valid_5g_bp;
		info->cp_5g_bp = policy->cp_5g_bp[idx];
		ret = true;
	}

	_os_spinunlock(d, &rg->lock, _bh, NULL);

exit:
	return ret;
}

static void _update_cntry_prop_5g_bp(
		struct rtw_regu_policy *policy,
		char *country,
		u8 prop)
{
	u16 idx = MAX_COUNTRY_NUM;

	idx = _country_index(country);
	if (idx < MAX_COUNTRY_NUM) {
		/* found matching idx, update country property */
		policy->cp_5g_bp[idx] = prop;
		PHL_INFO("%s, prop of (%c%c) = 0x%02X.\n",
				__func__, country[0], country[1], prop);
	} else {
		/* country unrecognized (no match) */
		PHL_INFO("%s, [WARN] skip unrecognized: (%c%c).\n",
				__func__, country[0], country[1]);
	}
}

static void _reset_regu_policy_5g_bp(struct rtw_regu_policy *policy)
{
	u16 i = 0;

	for (i = 0; i < MAX_COUNTRY_NUM; i++)
		policy->cp_5g_bp[i] = 0;
	policy->valid_5g_bp = false;
}

static void _regu_policy_5g_bp_init(struct rtw_regu_policy *policy)
{
	_reset_regu_policy_5g_bp(policy);
	_update_cntry_prop_5g_bp(policy, "CN",
		CP_5G_BAND1_SOFTAP_BLOCKED|CP_5G_BAND2_SOFTAP_BLOCKED|
		CP_5G_BAND3_SOFTAP_BLOCKED);
	_update_cntry_prop_5g_bp(policy, "IL",
		(CP_5G_BAND1_SOFTAP_BLOCKED|CP_5G_BAND2_SOFTAP_BLOCKED|
		CP_5G_BAND3_SOFTAP_BLOCKED|CP_5G_BAND4_SOFTAP_BLOCKED));
	policy->valid_5g_bp = true;
}

void phl_regu_policy_init(void *drv_priv, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;

	if (!phl || !drv_priv)
		return;

	rg = &phl_info->regulation;
	if (!rg->init)
		return;

	_os_spinlock(drv_priv, &rg->lock, _bh, NULL);
	_regu_policy_5g_bp_init(&rg->policy);
	_os_spinunlock(drv_priv, &rg->lock, _bh, NULL);
}

bool rtw_phl_regulation_valid(void *phl)
{
	return _regulation_valid(phl);
}

/*
 * @ Function description
 *	Used to check if channel is in regulation channel list
 *
 * @ parameter
 * 	phl : struct phl_info_t *
 *	channel : channel to be checked
 *	reject : enum ch_property, ex: (CH_PASSIVE | CH_DFS)
 *
 * @ return :
 *	true : channel is in regulation list and not rejected
 *	false : query regulation failed or channel is not in regulation
 *		channel list
 */
bool rtw_phl_regulation_valid_channel(void *phl, enum band_type band,
						u16 channel, u8 reject)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regulation_channel ch = {0};
	bool valid = false;
	void *d = NULL;
	u8 rej_property = reject;

	if (!_regulation_valid(phl))
		return false;

	rg = &phl_info->regulation;
	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);
	if (_query_channel(rg, band, channel, &ch)) {
		if (!(rg->capability & CAPABILITY_DFS))
			rej_property |= CH_DFS;
		if (_valid_property(ch.property, rej_property))
			valid = true;
	}
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return valid;
}

/*
 * @ Function description
 *	Used to check if channel is a regulation DFS channel
 *
 * @ parameter
 * 	phl : struct phl_info_t *
 *	channel : channel to be checked
 *	dfs : result will be filled here
 *
 * @ return :
 *	true : regulation query successfully, caller can check result
 *		by input parameter *dfs.
 *	false : regulation fail
 *
 */
bool rtw_phl_regulation_dfs_channel(void *phl, enum band_type band,
						u16 channel, bool *dfs)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	struct rtw_regulation_channel ch = {0};
	void *d = NULL;
	bool query = false;

	if (!_regulation_valid(phl) || !dfs)
		return false;

	rg = &phl_info->regulation;
	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);
	if (_query_channel(rg, band, channel, &ch)) {
		query = true;
		if (ch.property & CH_DFS)
			*dfs = true;
		else
			*dfs = false;
	}
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return query;
}

/*
 * @ Function description
 *	Query regulation channel
 *
 * @ parameter
 * 	phl : struct phl_info_t *
 *	channel : channel for query
 *	ch : query result will be filled here
 *
 * @ return :
 *	true : regulation query successfully, caller can check result
 *		by input parameter *ch.
 *	false : regulation query fail
 *
 */
bool rtw_phl_regulation_query_ch(void *phl, enum band_type band, u8 channel,
					struct rtw_regulation_channel *ch)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;
	void *d = NULL;
	bool query = false;

	if (!_regulation_valid(phl) || !ch)
		return false;

	rg = &phl_info->regulation;
	d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &rg->lock, _bh, NULL);
	if (_query_channel(rg, band, channel, ch))
		query = true;
	_os_spinunlock(d, &rg->lock, _bh, NULL);

	return query;
}

u8 rtw_phl_get_domain_regulation_2g(u8 domain)
{
	u8 did = MAX_RD_MAP_NUM;

	if (!rtw_phl_valid_regulation_domain(domain))
		return REGULATION_MAX;

	did = _domain_index(domain);
	if (did >= MAX_RD_MAP_NUM)
		return REGULATION_MAX;

	return rdmap[did].freq_2g.regulation;
}

u8 rtw_phl_get_domain_regulation_5g(u8 domain)
{
	u8 did = MAX_RD_MAP_NUM;

	if (!rtw_phl_valid_regulation_domain(domain))
		return REGULATION_MAX;

	did = _domain_index(domain);
	if (did >= MAX_RD_MAP_NUM)
		return REGULATION_MAX;

	return rdmap[did].freq_5g.regulation;
}

u8 rtw_phl_get_domain_regulation_6g(u8 domain)
{
	return regu_get_domain_regulation_6g(domain);
}

u16 rtw_phl_get_regu_chplan_ver(void)
{
	return REGULATION_CHPLAN_VERSION;
}

u16 rtw_phl_get_regu_country_ver(void)
{
	return REGULATION_COUNTRY_VERSION;
}

