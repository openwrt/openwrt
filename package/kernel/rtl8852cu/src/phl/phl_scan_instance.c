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
#include "phl_scan_instance.h"
#include "phl_scan.h"
#include "phl_regulation.h"

enum {
	ACTIVE_PERIOD_MIN = 40,
	ACTIVE_PERIOD_DEFAULT = 60,
	ACTIVE_PERIOD_MAX = 80
};

enum {
	PASSIVE_PERIOD_MIN = 50,
	PASSIVE_PERIOD_DEFAULT = 80,
	PASSIVE_PERIOD_MAX = 110
};

struct rtw_pickup_channel {
	enum band_type band;
	u8 channel;
	u8 property;
	u8 picked;
};

struct rtw_pickup_chplan_group {
	u32 cnt;
	struct rtw_pickup_channel ch[MAX_CH_NUM_GROUP];
};

static void _set_inst_ch(enum period_strategy strategy,
			struct instance_channel *dest,
			enum band_type band,
			u8 channel, u8 property)
{
	u8 max_t[2] = {PASSIVE_PERIOD_MAX, ACTIVE_PERIOD_MAX};
	u8 min_t[2] = {PASSIVE_PERIOD_MIN, ACTIVE_PERIOD_MIN};
	u8 def_t[2] = {PASSIVE_PERIOD_DEFAULT, ACTIVE_PERIOD_DEFAULT};

	dest->band = band;
	dest->channel = channel;
	dest->property = property;
	dest->mode = NORMAL_SCAN_MODE;
	dest->bw = CHANNEL_WIDTH_20;
	dest->offset = CHAN_OFFSET_NO_EXT;

	/* active or passive */
	if ((dest->property & CH_PASSIVE) ||
		(dest->property & CH_DFS))
		dest->active = 0;
	else
		dest->active = 1;

	if (strategy & PERIOD_ALL_MAX)
		dest->period = max_t[dest->active];
	else if (strategy & PERIOD_ALL_MIN)
		dest->period = min_t[dest->active];
	else {
		if ((strategy & PERIOD_MIN_DFS) &&
			(dest->property & CH_DFS))
			dest->period = min_t[dest->active];
		else
			dest->period = def_t[dest->active];
	}

	PHL_INFO("[REGU], pick channel %d, active=%d, period=%d\n",
			dest->channel, dest->active, dest->period);
}

static void _pick_active_channels(struct rtw_pickup_chplan_group *group,
					struct instance_strategy *strategy,
					struct instance *inst)
{
	struct instance_channel *dest = NULL;
	struct rtw_pickup_channel *src = NULL;
	u8 i = 0;

	dest = &inst->ch[inst->cnt];

	for (i = 0; i < group->cnt; i++) {
		src = &group->ch[i];

		if (src->picked)
			continue;

		if (!(src->property & CH_PASSIVE) &&
			!(src->property & CH_DFS) ) {
			_set_inst_ch(strategy->period, dest,
					src->band,
					src->channel,
					src->property);
			inst->cnt++;
			dest++;
			src->picked = 1;
		}
	}
}

static void _pick_the_rest_channels(struct rtw_pickup_chplan_group *group,
					struct instance_strategy *strategy,
					struct instance *inst)
{
	struct instance_channel *dest = NULL;
	struct rtw_pickup_channel *src = NULL;
	u8 i = 0;

	dest = &inst->ch[inst->cnt];

	for (i = 0; i < group->cnt; i++) {
		src = &group->ch[i];

		if (!src->picked) {
			_set_inst_ch(strategy->period, dest,
					src->band,
					src->channel,
					src->property);
			inst->cnt++;
			dest++;
			src->picked = 1;
		}
	}
}

static void _pick_5ghz_channels(struct rtw_pickup_chplan_group *group,
					struct instance_strategy *strategy,
					struct instance *inst)
{
	u8 order = strategy->order;
	u8 i = 0;

	if (order & ORDER_ACTIVE_PRIOR) {
		for (i = FREQ_GROUP_5GHZ_BAND1;
			i <= FREQ_GROUP_5GHZ_BAND4; i++) {
			_pick_active_channels(&group[i], strategy, inst);
		}
	}

	for (i = FREQ_GROUP_5GHZ_BAND1;
		i <= FREQ_GROUP_5GHZ_BAND4; i++) {
		_pick_the_rest_channels(&group[i], strategy, inst);
	}
}

static void _pick_2ghz_channels(struct rtw_pickup_chplan_group *group,
					struct instance_strategy *strategy,
					struct instance *inst)
{
	u8 order = strategy->order;

	if (order & ORDER_ACTIVE_PRIOR)
		_pick_active_channels(&group[FREQ_GROUP_2GHZ],
						strategy, inst);

	_pick_the_rest_channels(&group[FREQ_GROUP_2GHZ],
					strategy, inst);
}

static void _pick_6ghz_channels(struct rtw_pickup_chplan_group *group,
					struct instance_strategy *strategy,
					struct instance *inst)
{
	u32 i = 0;

	for (i = FREQ_GROUP_6GHZ_UNII5;
		i <= FREQ_GROUP_6GHZ_UNII8; i++) {
		_pick_the_rest_channels(&group[i], strategy, inst);
	}
}

static void _generate_instance(
		struct rtw_pickup_chplan_group *group,
		struct instance_strategy *strategy,
		struct instance *inst)
{
	u8 order = strategy->order;

	inst->cnt = 0;

	if (order & ORDER_5GHZ_PRIOR) {
		_pick_5ghz_channels(group, strategy, inst);
		_pick_2ghz_channels(group, strategy, inst);
	} else {
		_pick_2ghz_channels(group, strategy, inst);
		_pick_5ghz_channels(group, strategy, inst);
	}

	_pick_6ghz_channels(group, strategy, inst);
}

static void _select_channels_by_group(struct instance_strategy *strategy,
					struct rtw_regulation_chplan *plan,
					struct rtw_pickup_chplan_group *group)
{
	u8 skip = strategy->skip;
	u8 chnl = 0, property = 0, gpidx = 0, keep = 0;
	enum band_type band = BAND_ON_24G;
	u8 i = 0;

	for (i = 0; i < plan->cnt; i++) {
		band = plan->ch[i].band;
		chnl = plan->ch[i].channel;
		property = plan->ch[i].property;
		keep = 0;

		/* skip passive channels */
		if ((skip & SKIP_PASSIVE) && (property & CH_PASSIVE))
			continue;

		/* skip DFS channels */
		if ((skip & SKIP_DFS) && (property & CH_DFS))
			continue;

		if ((BAND_2GHZ(band)) && !(skip & SKIP_2GHZ)) {
			gpidx = FREQ_GROUP_2GHZ;
			keep = 1;
		} else if ((BAND_5GHZ(band)) && !(skip & SKIP_5GHZ)) {
			if (CH_5GHZ_BAND1(chnl))
				gpidx = FREQ_GROUP_5GHZ_BAND1;
			else if (CH_5GHZ_BAND2(chnl))
				gpidx = FREQ_GROUP_5GHZ_BAND2;
			else if (CH_5GHZ_BAND3(chnl))
				gpidx = FREQ_GROUP_5GHZ_BAND3;
			else if (CH_5GHZ_BAND4(chnl))
				gpidx = FREQ_GROUP_5GHZ_BAND4;
			else
				continue;
			keep = 1;
		} else if ((BAND_6GHZ(band)) && !(skip & SKIP_6GHZ)) {
			if (CH_6GHZ_UNII5(chnl))
				gpidx = FREQ_GROUP_6GHZ_UNII5;
			else if (CH_6GHZ_UNII6(chnl))
				gpidx = FREQ_GROUP_6GHZ_UNII6;
			else if (CH_6GHZ_UNII7(chnl))
				gpidx = FREQ_GROUP_6GHZ_UNII7;
			else if (CH_6GHZ_UNII8(chnl))
				gpidx = FREQ_GROUP_6GHZ_UNII8;
			else
				continue;
			keep = 1;
		}

		if (keep) {
			group[gpidx].ch[group[gpidx].cnt].band = band;
			group[gpidx].ch[group[gpidx].cnt].channel = chnl;
			group[gpidx].ch[group[gpidx].cnt].property = property;
			group[gpidx].ch[group[gpidx].cnt].picked = 0;
			group[gpidx].cnt++;
			PHL_INFO("[REGU], keep group-%d channel %d, cnt=%d\n",
					gpidx, chnl, group[gpidx].cnt);
		}
	}
}

bool rtw_phl_generate_scan_instance(struct instance_strategy *strategy,
					struct rtw_regulation_chplan *chplan,
					struct instance *inst)
{
	struct rtw_pickup_chplan_group group[FREQ_GROUP_MAX] = {0};
	u8 i = 0;

	if (!strategy || !inst || !chplan)
		return false;

	PHL_INFO("[REGU], Generate Scan Instance, strategy [skip=0x%x, order=0x%x, period=0x%x] \n",
			strategy->skip, strategy->order, strategy->period);
	PHL_INFO("[REGU], Channel Plan Source : \n");

	for (i = 0; i < chplan->cnt; i++) {
		PHL_INFO("[REGU], %d. band=%d, ch=%d, dfs=%d, passive=%d (property=0x%x)\n",
			i + 1, chplan->ch[i].band, chplan->ch[i].channel,
			((chplan->ch[i].property & CH_DFS) ? 1 : 0),
			((chplan->ch[i].property & CH_PASSIVE) ? 1 : 0),
			chplan->ch[i].property);
	}

	/* step1 : remove "skip channels" and select channels into groups */
	_select_channels_by_group(strategy, chplan, group);


	/* step2 : generate instance by strategy */
	_generate_instance(group, strategy, inst);

	PHL_INFO("[REGU], Output Scan Instance : \n");
	for (i = 0; i < inst->cnt; i++) {
		PHL_INFO("[REGU], %d. band=%d, ch=%d, active=%d, period=%d \n",
				i + 1, inst->ch[i].band,
				inst->ch[i].channel,
				inst->ch[i].active,
				inst->ch[i].period);
	}

	return true;
}

bool rtw_phl_scan_instance_insert_ch(void *phl, struct instance *inst,
					enum band_type band, u8 channel,
						u8 strategy_period)
{
	struct rtw_regulation_channel ch = {0};
	struct instance_channel *dest = NULL;

	if (!phl || !inst)
		return false;

	if (inst->cnt >= MAX_SCAN_INSTANCE)
		return false;

	if (rtw_phl_regulation_query_ch(phl, band, channel, &ch)) {
		dest = &inst->ch[inst->cnt];
		_set_inst_ch(strategy_period, dest,
				ch.band, ch.channel, ch.property);
		inst->cnt++;
		return true;
	}

	return false;
}



