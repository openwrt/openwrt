/******************************************************************************
 *
 * Copyright(c) 2009-2010 - 2017 Realtek Corporation.
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

#include <drv_types.h>

#ifdef CONFIG_IOCTL_CFG80211
void rtw_chset_hook_os_channels(struct rtw_chset *chset, void *os_ref)
{
	struct wiphy *wiphy = os_ref;
	struct ieee80211_channel *ch;
	unsigned int i;
	u32 freq;

	for (i = 0; i < chset->chs_len; i++) {
		freq = rtw_bch2freq(chset->chs[i].band, chset->chs[i].ChannelNum);
		ch = ieee80211_get_channel(wiphy, freq);
		if (!ch)
			RTW_WARN(FUNC_WIPHY_FMT" can't get %s ch%u\n", FUNC_WIPHY_ARG(wiphy), band_str(chset->chs[i].band), chset->chs[i].ChannelNum);
		chset->chs[i].os_chan = ch;
	}
}

#if CONFIG_RTW_CFG80211_CAC_EVENT
static void rtw_regd_set_du_chdef(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	struct rtw_chset *chset = dvobj_to_chset(wiphy_to_dvobj(wiphy));
	struct ieee80211_channel *ch = NULL;
	int i;

	for (i = 0; i < chset->chs_len; i++) {
		if (chset->chs[i].os_chan) {
			ch = chset->chs[i].os_chan;
			break;
		}
	}

	if (ch) {
		wiphy_data->du_chdef.chan = ch;
		wiphy_data->du_chdef.width = NL80211_CHAN_WIDTH_20;
		wiphy_data->du_chdef.center_freq1 = ch->center_freq;
	}
}

static int rtw_du_wdev_ndev_open(struct net_device *ndev)
{
	return -EPERM;
}

static const struct net_device_ops rtw_du_wdev_ndev_ops = {
	.ndo_open = rtw_du_wdev_ndev_open,
};

static struct wireless_dev *rtw_regd_alloc_du_wdev(struct wiphy *wiphy)
{
	struct rtw_netdev_priv_indicator *npi;
	struct net_device *ndev = NULL;
	struct wireless_dev *wdev = NULL;
	int ret = 0;

	ndev = alloc_etherdev(sizeof(struct rtw_netdev_priv_indicator));
	if (!ndev) {
		RTW_WARN(FUNC_WIPHY_FMT" allocate ndev fail\n", FUNC_WIPHY_ARG(wiphy));
		ret = -ENOMEM;
		goto out;
	}

	snprintf(ndev->name, IFNAMSIZ, "%s-du", wiphy_name(wiphy));

	ndev->priv_destructor = rtw_ndev_destructor;
	ndev->netdev_ops = &rtw_du_wdev_ndev_ops;

	npi = netdev_priv(ndev);
	npi->priv = wiphy_to_adapter(wiphy);
	npi->sizeof_priv = sizeof(_adapter);

	wdev = rtw_zmalloc(sizeof(struct wireless_dev));
	if (!wdev) {
		RTW_WARN(FUNC_WIPHY_FMT" allocate wdev fail\n", FUNC_WIPHY_ARG(wiphy));
		ret = -ENOMEM;
		goto out;
	}

	wdev->wiphy = wiphy;
	wdev->netdev = ndev;
	wdev->iftype = NL80211_IFTYPE_AP;
	ndev->ieee80211_ptr = wdev;

	ret = register_netdevice(ndev);

out:
	if (ret && wdev) {
		rtw_mfree(wdev, sizeof(struct wireless_dev));
		wdev = NULL;
	}

	if (ret && ndev)
		free_netdev(ndev);

	return wdev;
}

void rtw_regd_free_du_wdev(struct wiphy *wiphy)
{
	struct dvobj_priv *dvobj = wiphy_to_dvobj(wiphy);
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	u8 rtnl_lock_needed;

	if (!wiphy_data->du_wdev)
		goto exit;

	rtnl_lock_needed = rtw_rtnl_lock_needed(dvobj);
	if (rtnl_lock_needed)
		rtnl_lock();

	unregister_netdevice(wiphy_data->du_wdev->netdev);

	if (rtnl_lock_needed)
		rtnl_unlock();

	wiphy_data->du_wdev = NULL;

exit:
	return;
}

static void rtw_regd_schedule_dfs_chan_update(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);

	if (!wiphy_data->du_wdev) {
		wiphy_data->du_wdev = rtw_regd_alloc_du_wdev(wiphy);
		if (!wiphy_data->du_wdev)
			return;
		rtw_regd_set_du_chdef(wiphy);
	}

	cfg80211_ch_switch_notify(wiphy_data->du_wdev->netdev, &wiphy_data->du_chdef);
}

static void rtw_regd_override_dfs_state(struct wiphy *wiphy, struct get_chplan_resp *chplan, bool non_ocp_only)
{
	RT_CHANNEL_INFO *chinfo;
	struct ieee80211_channel *chan;
	int i;
	u8 beaconing_hwband_bmp = 0;
	bool update = false;

	for (i = HW_BAND_0; i < HW_BAND_MAX; i++)
		if (HWBAND_AP_NUM(wiphy_to_dvobj(wiphy), i) || HWBAND_MESH_NUM(wiphy_to_dvobj(wiphy), i))
			beaconing_hwband_bmp |= BIT(i);

	for (i = 0; i < chplan->chs_len; i++) {
		if (chplan->chs[i].flags & RTW_CHF_DIS)
			continue;
		chinfo = &chplan->chs[i];
		chan = chinfo->os_chan;
		if (!chan)
			continue;

		if (CH_IS_NON_OCP(chinfo)
			&& (chan->dfs_state != NL80211_DFS_UNAVAILABLE
				|| chan->dfs_state_entered != chinfo->non_ocp_end_time - rtw_ms_to_systime(IEEE80211_DFS_MIN_NOP_TIME_MS))
		) {
			chan->dfs_state = NL80211_DFS_UNAVAILABLE;
			chan->dfs_state_entered = chinfo->non_ocp_end_time
				- rtw_ms_to_systime(IEEE80211_DFS_MIN_NOP_TIME_MS);
			update = true;
		}
		if (non_ocp_only)
			continue;

		if (chinfo->cac_done && beaconing_hwband_bmp
			&& chan->dfs_state != NL80211_DFS_AVAILABLE
		) {
			chan->dfs_state = NL80211_DFS_AVAILABLE;
			update = true;
		}
	}

	if (update) {
		/* trigger cfg80211_sched_dfs_chan_update() */
		rtw_regd_schedule_dfs_chan_update(wiphy);
	}
}
#endif /* CONFIG_RTW_CFG80211_CAC_EVENT */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
bool rtw_regd_is_wiphy_self_managed(struct wiphy *wiphy)
{
	return rtw_rfctl_is_disable_sw_channel_plan(wiphy_to_dvobj(wiphy))
		|| !REGSTY_REGD_SRC_FROM_OS(dvobj_to_regsty(wiphy_to_dvobj(wiphy)));
}

static struct ieee80211_reg_rule rtw_get_ieee80211_reg_rule(struct get_chplan_resp *chplan, enum band_type band, int last_end_freq, int start_freq, int end_freq, int next_start_freq, u32 flags)
{
	struct ieee80211_reg_rule rule = REG_RULE(
		start_freq - 10, end_freq + 10, 20, 6, 20,
		((flags & RTW_CHF_NO_IR) ? NL80211_RRF_NO_IR : 0)
		| ((flags & RTW_CHF_DFS) ? NL80211_RRF_DFS : 0)
	);

	int regd_max_bw = 160;
	int frange_max_bw = 160;

	if (!(chplan->proto_en & CHPLAN_PROTO_EN_AC) || band == BAND_ON_24G)
		regd_max_bw = 40;
	/* TODO: !RFCTL_REG_EN_11HT(rfctl) limit to 20MHz  */

	while ((end_freq - start_freq + 20) < frange_max_bw) {
		frange_max_bw /= 2;
		if (frange_max_bw == 20)
			break;
	}
	rule.freq_range.max_bandwidth_khz = MHZ_TO_KHZ(rtw_min(regd_max_bw, frange_max_bw));

	if (regd_max_bw > frange_max_bw
		&& (rtw_freq_consecutive(last_end_freq, start_freq)
			|| rtw_freq_consecutive(end_freq, next_start_freq)
		)
	)
		rule.flags |= NL80211_RRF_AUTO_BW;

	if (regd_max_bw < 40)
		rule.flags |= NL80211_RRF_NO_HT40;
	if (regd_max_bw < 80)
		rule.flags |= NL80211_RRF_NO_80MHZ;
	if (regd_max_bw < 160)
		rule.flags |= NL80211_RRF_NO_160MHZ;

	return rule;
}

static int rtw_build_wiphy_regd(struct wiphy *wiphy, struct get_chplan_resp *chplan, struct ieee80211_regdomain **regd)
{
	int i;
	RT_CHANNEL_INFO *chinfo;
	enum band_type start_band, band;
	int last_end_freq, start_freq, end_freq, freq;
	u32 start_flags, flags;
	struct ieee80211_regdomain *r = NULL;
	int rule_num = 0;
	bool build = 0;

	if (regd)
		*regd = NULL;

loop:
	start_band = BAND_MAX;
	last_end_freq = 0;
	for (i = 0; i < chplan->chs_len; i++) {
		if (chplan->chs[i].flags & RTW_CHF_DIS)
			continue;
		chinfo = &chplan->chs[i];
		freq = rtw_bch2freq(chinfo->band, chinfo->ChannelNum);
		if (!freq) {
			RTW_WARN(FUNC_WIPHY_FMT" rtw_bch2freq(%s, %u) fail\n"
				, FUNC_WIPHY_ARG(wiphy), band_str(chinfo->band), chinfo->ChannelNum);
			continue;
		}
		band = chinfo->band;

		flags = chinfo->flags & RTW_CHF_NO_IR;
		#if CONFIG_RTW_CFG80211_CAC_EVENT
		if (wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD)) {
			if (chinfo->flags & RTW_CHF_DFS)
				flags |= RTW_CHF_DFS;
		}
		#endif

		if (start_band == BAND_MAX) {
			start_band = band;
			start_freq = end_freq = freq;
			start_flags = flags;
			continue;
		}

		if (start_band == band
			&& start_flags == flags
			&& rtw_freq_consecutive(end_freq, freq)
		) {
			end_freq = freq;
			continue;
		}

		/* create rule */
		if (build) {
			RTW_DBG("add rule_%02d(%s, %d, %d, 0x%x)\n"
				, r->n_reg_rules, band_str(start_band), start_freq, end_freq, start_flags);
			r->reg_rules[r->n_reg_rules++] = rtw_get_ieee80211_reg_rule(chplan, start_band
				, last_end_freq, start_freq, end_freq, freq, start_flags);
		} else
			rule_num++;

		/* start a new rule */
		start_band = band;
		last_end_freq = end_freq;
		start_freq = end_freq = freq;
		start_flags = flags;
	}

	if (start_band != BAND_MAX) {
		/* create rule */
		if (build) {
			RTW_DBG("add rule_%02d(%s, %d, %d, 0x%x)\n"
				, r->n_reg_rules, band_str(start_band), start_freq, end_freq, start_flags);
			r->reg_rules[r->n_reg_rules++] = rtw_get_ieee80211_reg_rule(chplan, start_band
				, last_end_freq, start_freq, end_freq, 0, start_flags);
		} else
			rule_num++;
	}

	if (!build) {
		/* switch to build phase */
		build = 1;
		if (!regd)
			goto exit;

		r = rtw_zmalloc(sizeof(**regd) + sizeof(struct ieee80211_reg_rule) * rule_num);
		if (!r) {
			rule_num = -1;
			goto exit;
		}

		_rtw_memcpy(r->alpha2, chplan->alpha2, 2);
		r->alpha2[2] = 0;
		r->dfs_region = NL80211_DFS_UNSET;
		goto loop;
	}

	*regd = r;

exit:
	return rule_num;
}

static void rtw_regd_disable_no_20mhz_chs(struct wiphy *wiphy)
{
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	unsigned int i, j;

	for (i = 0; i < NUM_NL80211_BANDS; i++) {
		sband = wiphy->bands[i];
		if (!sband)
			continue;
		for (j = 0; j < sband->n_channels; j++) {
			ch = &sband->channels[j];
			if (!ch)
				continue;
			if (ch->flags & IEEE80211_CHAN_NO_20MHZ) {
				RTW_INFO(FUNC_WIPHY_FMT" disable band:%d ch:%u w/o 20MHz\n", FUNC_WIPHY_ARG(wiphy), ch->band, ch->hw_value);
				ch->flags = IEEE80211_CHAN_DISABLED;
			}
		}
	}
}

void rtw_update_wiphy_regd(struct wiphy *wiphy, struct get_chplan_resp *chplan, bool rtnl_lock_needed)
{
	struct ieee80211_regdomain *regd;
	int ret;

	ret = rtw_build_wiphy_regd(wiphy, chplan, &regd);
	if (ret == -1) {
		RTW_WARN(FUNC_WIPHY_FMT" rtw_build_wiphy_regd() fail\n", FUNC_WIPHY_ARG(wiphy));
		return;
	}

	if (ret == 0) {
		RTW_WARN(FUNC_WIPHY_FMT" rtw_build_wiphy_regd() builds empty regd, bypass regd setting\n", FUNC_WIPHY_ARG(wiphy));
		goto free_regd;
	}

	if (rtnl_lock_needed)
		rtnl_lock();

	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 12, 0))
	ret = regulatory_set_wiphy_regd_sync(wiphy, regd);
	#else
	ret = regulatory_set_wiphy_regd_sync_rtnl(wiphy, regd);
	#endif

	rtw_regd_disable_no_20mhz_chs(wiphy);

	#if CONFIG_RTW_CFG80211_CAC_EVENT
	rtw_regd_override_dfs_state(wiphy, chplan, false);
	#endif

	if (rtnl_lock_needed)
		rtnl_unlock();

	if (ret != 0)
		RTW_INFO(FUNC_WIPHY_FMT" regulatory_set_wiphy_regd_sync_rtnl return %d\n", FUNC_WIPHY_ARG(wiphy), ret);

free_regd:
	rtw_mfree(regd, sizeof(*regd) + sizeof(struct ieee80211_reg_rule) * regd->n_reg_rules);
}
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)) */

static void rtw_regd_overide_flags(struct wiphy *wiphy, struct get_chplan_resp *chplan, bool rtnl_lock_needed)
{
	RT_CHANNEL_INFO *chs = chplan->chs;
	u8 chs_len = chplan->chs_len;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	unsigned int i, j;

	if (rtnl_lock_needed)
		rtnl_lock();

	/* all channels disable */
	for (i = 0; i < NUM_NL80211_BANDS; i++) {
		sband = wiphy->bands[i];
		if (!sband)
			continue;
		for (j = 0; j < sband->n_channels; j++) {
			ch = &sband->channels[j];
			if (!ch)
				continue;
			ch->flags = IEEE80211_CHAN_DISABLED;
		}
	}

	/* channels apply by channel plans. */
	for (i = 0; i < chs_len; i++) {
		if (chs[i].flags & RTW_CHF_DIS)
			continue;
		ch = chs[i].os_chan;
		if (!ch)
			continue;

		/* enable */
		ch->flags = 0;

		if (chs[i].flags & RTW_CHF_DFS) {
			/*
			* before integrating with nl80211 flow
			* bypass IEEE80211_CHAN_RADAR when configured with radar detection
			* to prevent from hostapd blocking DFS channels
			*/
			#ifdef CONFIG_DFS_MASTER
			if (chplan->dfs_domain == RTW_DFS_REGD_NONE)
			#endif
				ch->flags |= IEEE80211_CHAN_RADAR;
		}

		if (chs[i].flags & RTW_CHF_NO_IR) {
			#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
			ch->flags |= IEEE80211_CHAN_NO_IBSS | IEEE80211_CHAN_PASSIVE_SCAN;
			#else
			ch->flags |= IEEE80211_CHAN_NO_IR;
			#endif
		}
	}

	if (rtnl_lock_needed)
		rtnl_unlock();
}

#ifdef CONFIG_REGD_SRC_FROM_OS
static void rtw_regd_apply_dfs_flags(struct get_chplan_resp *chplan, bool rtnl_lock_needed)
{
	RT_CHANNEL_INFO *chs = chplan->chs;
	u8 chs_len = chplan->chs_len;
	unsigned int i;
	struct ieee80211_channel *chan;

	if (rtnl_lock_needed)
		rtnl_lock();

	/* channels apply by channel plans. */
	for (i = 0; i < chs_len; i++) {
		if (chs[i].flags & RTW_CHF_DIS)
			continue;
		chan = chs[i].os_chan;
		if (!chan) {
			rtw_warn_on(1);
			continue;
		}
		if (chs[i].flags & RTW_CHF_DFS) {
			/*
			* before integrating with nl80211 flow
			* clear IEEE80211_CHAN_RADAR when configured with radar detection
			* to prevent from hostapd blocking DFS channels
			*/
			#ifdef CONFIG_DFS_MASTER
			if (chplan->dfs_domain != RTW_DFS_REGD_NONE)
				chan->flags &= ~IEEE80211_CHAN_RADAR;
			#endif
		}
	}

	if (rtnl_lock_needed)
		rtnl_unlock();
}

static u8 wiphy_chan_get_rtw_ch_flags(struct ieee80211_channel *chan)
{
	u8 flags;

	if (!chan || chan->flags & IEEE80211_CHAN_DISABLED)
		return RTW_CHF_DIS;

	flags = 0;

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	if (chan->flags & (IEEE80211_CHAN_NO_IBSS | IEEE80211_CHAN_PASSIVE_SCAN))
	#else
	if (chan->flags & IEEE80211_CHAN_NO_IR)
	#endif
		flags |= RTW_CHF_NO_IR;
	if (chan->flags & IEEE80211_CHAN_RADAR)
		flags |= RTW_CHF_DFS;
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	if (chan->flags & IEEE80211_CHAN_NO_HT40PLUS)
		flags |= RTW_CHF_NO_HT40U;
	if (chan->flags & IEEE80211_CHAN_NO_HT40MINUS)
		flags |= RTW_CHF_NO_HT40L;
	#endif
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
	if (chan->flags & IEEE80211_CHAN_NO_80MHZ)
		flags |= RTW_CHF_NO_80MHZ;
	if (chan->flags & IEEE80211_CHAN_NO_160MHZ)
		flags |= RTW_CHF_NO_160MHZ;
	#endif

	return flags;
}

static void rtw_chset_apply_wiphy_chans(struct rtw_chset *chset, u8 d_flags)
{
	RT_CHANNEL_INFO *chinfo;
	u8 i;
	u8 flags;

	for (i = 0; i < chset->chs_len; i++) {
		chinfo = &chset->chs[i];
		if (chinfo->flags & RTW_CHF_DIS)
			continue;
		flags = wiphy_chan_get_rtw_ch_flags(chinfo->os_chan);
		if ((flags & RTW_CHF_DIS) || (flags & d_flags))
			chinfo->flags = RTW_CHF_DIS;
		else
			chinfo->flags |= flags;
	}
}

void rtw_chset_apply_from_os(struct rtw_chset *chset, u8 d_flags)
{
	rtw_chset_apply_wiphy_chans(chset, d_flags);
}

s16 rtw_os_get_total_txpwr_regd_lmt_mbm(_adapter *adapter, enum band_type band, u8 cch, enum channel_width bw)
{
	struct wiphy *wiphy = adapter_to_wiphy(adapter);
	s16 mbm = UNSPECIFIED_MBM;
	u8 *op_chs;
	u8 op_ch_num;
	u8 i;
	u32 freq;
	struct ieee80211_channel *ch;

	if (!rtw_get_op_chs_by_bcch_bw(band, cch, bw, &op_chs, &op_ch_num))
		goto exit;

	for (i = 0; i < op_ch_num; i++) {
		freq = rtw_bch2freq(band, op_chs[i]);
		ch = ieee80211_get_channel(wiphy, freq);
		if (!ch) {
			rtw_warn_on(1);
			continue;
		}
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0))
		mbm = rtw_min(mbm, ch->max_reg_power * MBM_PDBM);
		#else
		/* require max_power == 0 (therefore orig_mpwr set to 0) when wiphy registration */
		mbm = rtw_min(mbm, ch->max_power * MBM_PDBM);
		#endif
	}

exit:
	return mbm;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
static enum rtw_dfs_regd nl80211_dfs_regions_to_rtw_dfs_region(enum nl80211_dfs_regions region)
{
	switch (region) {
	case NL80211_DFS_FCC:
		return RTW_DFS_REGD_FCC;
	case NL80211_DFS_ETSI:
		return RTW_DFS_REGD_ETSI;
	case NL80211_DFS_JP:
		return RTW_DFS_REGD_MKK;
	case NL80211_DFS_UNSET:
	default:
		return RTW_DFS_REGD_NONE;
	}
};
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) */
#endif /* CONFIG_REGD_SRC_FROM_OS */

static enum rtw_regd_inr nl80211_reg_initiator_to_rtw_regd_inr(enum nl80211_reg_initiator initiator)
{
	switch (initiator) {
	case NL80211_REGDOM_SET_BY_DRIVER:
		return RTW_REGD_SET_BY_DRIVER;
	case NL80211_REGDOM_SET_BY_CORE:
		return RTW_REGD_SET_BY_CORE;
	case NL80211_REGDOM_SET_BY_USER:
		return RTW_REGD_SET_BY_USER;
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		return RTW_REGD_SET_BY_COUNTRY_IE;
	}
	rtw_warn_on(1);
	return RTW_REGD_SET_BY_NUM;
};

#ifdef CONFIG_RTW_DEBUG
static const char *nl80211_reg_initiator_str(enum nl80211_reg_initiator initiator)
{
	switch (initiator) {
	case NL80211_REGDOM_SET_BY_DRIVER:
		return "DRIVER";
	case NL80211_REGDOM_SET_BY_CORE:
		return "CORE";
	case NL80211_REGDOM_SET_BY_USER:
		return "USER";
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		return "COUNTRY_IE";
	}
	rtw_warn_on(1);
	return "UNKNOWN";
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
static const char *nl80211_user_reg_hint_type_str(enum nl80211_user_reg_hint_type type)
{
	switch (type) {
	case NL80211_USER_REG_HINT_USER:
		return "USER";
	case NL80211_USER_REG_HINT_CELL_BASE:
		return "CELL_BASE";
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
	case NL80211_USER_REG_HINT_INDOOR:
		return "INDOOR";
	#endif
	}
	rtw_warn_on(1);
	return "UNKNOWN";
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
static const char *nl80211_dfs_regions_str(enum nl80211_dfs_regions region)
{
	switch (region) {
	case NL80211_DFS_UNSET:
		return "UNSET";
	case NL80211_DFS_FCC:
		return "FCC";
	case NL80211_DFS_ETSI:
		return "ETSI";
	case NL80211_DFS_JP:
		return "JP";
	}
	rtw_warn_on(1);
	return "UNKNOWN";
};
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) */

static const char *environment_cap_str(enum environment_cap cap)
{
	switch (cap) {
	case ENVIRON_ANY:
		return "ANY";
	case ENVIRON_INDOOR:
		return "INDOOR";
	case ENVIRON_OUTDOOR:
		return "OUTDOOR";
	}
	rtw_warn_on(1);
	return "UNKNOWN";
}

static void dump_requlatory_request(void *sel, struct regulatory_request *request)
{
	u8 alpha2_len;

	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 16, 0))
	alpha2_len = 3;
	#else
	alpha2_len = 2;
	#endif

	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
	RTW_PRINT_SEL(sel, "initiator:%s, wiphy_idx:%d, type:%s\n"
		, nl80211_reg_initiator_str(request->initiator)
		, request->wiphy_idx
		, nl80211_user_reg_hint_type_str(request->user_reg_hint_type));
	#else
	RTW_PRINT_SEL(sel, "initiator:%s, wiphy_idx:%d\n"
		, nl80211_reg_initiator_str(request->initiator)
		, request->wiphy_idx);
	#endif

	RTW_PRINT_SEL(sel, "alpha2:%.*s\n", alpha2_len, request->alpha2);
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
	RTW_PRINT_SEL(sel, "dfs_region:%s\n", nl80211_dfs_regions_str(request->dfs_region));
	#endif

	RTW_PRINT_SEL(sel, "intersect:%d\n", request->intersect);
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
	RTW_PRINT_SEL(sel, "processed:%d\n", request->processed);
	#endif
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
	RTW_PRINT_SEL(sel, "country_ie_checksum:0x%08x\n", request->country_ie_checksum);
	#endif

	RTW_PRINT_SEL(sel, "country_ie_env:%s\n", environment_cap_str(request->country_ie_env));
}
#endif /* CONFIG_RTW_DEBUG */

static void rtw_reg_notifier(struct wiphy *wiphy, struct regulatory_request *request)
{
	struct dvobj_priv *dvobj = wiphy_to_dvobj(wiphy);
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);
	enum rtw_regd_inr inr;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	rtw_set_rtnl_lock_holder(dvobj, current);
#endif

#ifdef CONFIG_RTW_DEBUG
	if (rtw_drv_log_level >= _DRV_INFO_) {
		RTW_INFO(FUNC_WIPHY_FMT"\n", FUNC_WIPHY_ARG(wiphy));
		dump_requlatory_request(RTW_DBGDUMP, request);
	}
#endif

	inr = nl80211_reg_initiator_to_rtw_regd_inr(request->initiator);

#ifdef CONFIG_REGD_SRC_FROM_OS
	if (REGSTY_REGD_SRC_FROM_OS(regsty)) {
		enum rtw_dfs_regd dfs_region =  RTW_DFS_REGD_NONE;

		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0))
		dfs_region = nl80211_dfs_regions_to_rtw_dfs_region(request->dfs_region);
		#endif

		/* trigger command to sync regulatory form OS */
		rtw_sync_os_regd_cmd(wiphy_to_adapter(wiphy), RTW_CMDF_WAIT_ACK, request->alpha2, dfs_region, inr);
	} else
#endif
	{
		/* use alpha2 as input to select the corresponding channel plan settings defined by Realtek */
		struct get_chplan_resp *chplan;

		switch (request->initiator) {
		case NL80211_REGDOM_SET_BY_USER:
			rtw_set_country(wiphy_to_adapter(wiphy), request->alpha2, inr);
			break;
		case NL80211_REGDOM_SET_BY_DRIVER:
		case NL80211_REGDOM_SET_BY_CORE:
		case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		default:
			#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
			rtw_warn_on(rtw_regd_is_wiphy_self_managed(wiphy));
			#endif
			if (rtw_get_chplan_cmd(wiphy_to_adapter(wiphy), RTW_CMDF_WAIT_ACK, &chplan) == _SUCCESS)
				rtw_regd_change_complete_sync(wiphy, chplan, 0);
			else
				rtw_warn_on(1);
			break;
		}
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	rtw_set_rtnl_lock_holder(dvobj, NULL);
#endif
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
static int rtw_reg_notifier_return(struct wiphy *wiphy, struct regulatory_request *request)
{
	rtw_reg_notifier(wiphy, request);
	return 0;
}
#endif

void rtw_regd_change_complete_sync(struct wiphy *wiphy, struct get_chplan_resp *chplan, bool rtnl_lock_needed)
{
	if (chplan->regd_src == REGD_SRC_RTK_PRIV) {
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
		if (rtw_regd_is_wiphy_self_managed(wiphy))
			rtw_update_wiphy_regd(wiphy, chplan, rtnl_lock_needed);
		else
		#endif
			rtw_regd_overide_flags(wiphy, chplan, rtnl_lock_needed);
	}
#ifdef CONFIG_REGD_SRC_FROM_OS
	else if (chplan->regd_src == REGD_SRC_OS)
		rtw_regd_apply_dfs_flags(chplan, rtnl_lock_needed);
#endif
	else
		rtw_warn_on(1);

	rtw_free_get_chplan_resp(chplan);
}

struct async_regd_change_evt {
	_list list; /* async_regd_change_list */
	struct wiphy *wiphy;
	struct get_chplan_resp *chplan;
};

static void async_regd_change_work_hdl(_workitem *work)
{
	struct rtw_wiphy_data *wiphy_data = container_of(work, struct rtw_wiphy_data, async_regd_change_work);
	struct async_regd_change_evt *evt;
	_list *list, *head = &wiphy_data->async_regd_change_list;

	while (1) {
		_rtw_mutex_lock_interruptible(&wiphy_data->async_regd_change_mutex);
		list = rtw_is_list_empty(head) ? NULL : get_next(head);
		if (list)
			rtw_list_delete(list);
		_rtw_mutex_unlock(&wiphy_data->async_regd_change_mutex);

		if (!list)
			break;

		evt = LIST_CONTAINOR(list, struct async_regd_change_evt, list);
		rtw_regd_change_complete_sync(evt->wiphy, evt->chplan, 1);
		rtw_mfree(evt, sizeof(*evt));
	}
}

int rtw_regd_change_complete_async(struct wiphy *wiphy, struct get_chplan_resp *chplan)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	struct async_regd_change_evt *evt;

	evt = rtw_malloc(sizeof(*evt));
	if (!evt) {
		rtw_free_get_chplan_resp(chplan);
		return _FAIL;
	}

	_rtw_init_listhead(&evt->list);
	evt->wiphy = wiphy;
	evt->chplan = chplan;

	_rtw_mutex_lock_interruptible(&wiphy_data->async_regd_change_mutex);

	rtw_list_insert_tail(&evt->list, &wiphy_data->async_regd_change_list);

	_rtw_mutex_unlock(&wiphy_data->async_regd_change_mutex);

	_set_workitem(&wiphy_data->async_regd_change_work);

	return _SUCCESS;
}

static void rtw_regd_async_regd_change_list_free(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	struct async_regd_change_evt *evt;
	struct get_chplan_resp *chplan;
	_list *cur, *head;

	_rtw_mutex_lock_interruptible(&wiphy_data->async_regd_change_mutex);

	head = &wiphy_data->async_regd_change_list;
	cur = get_next(head);

	while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
		evt = LIST_CONTAINOR(cur, struct async_regd_change_evt, list);
		chplan = evt->chplan;
		cur = get_next(cur);
		rtw_list_delete(&evt->list);
		rtw_free_get_chplan_resp(chplan);
		rtw_mfree(evt, sizeof(*evt));
	}

	_rtw_mutex_unlock(&wiphy_data->async_regd_change_mutex);
}

#if CONFIG_RTW_CFG80211_CAC_EVENT
static bool rtw_cfg80211_get_radar_event_chdef(struct wiphy *wiphy, struct cfg80211_chan_def *chdef
	, u8 cch, enum channel_width bw, const char *caller)
{
	u8 *op_chs;
	u8 op_ch_num;
	u8 state;

	/* only channel coverage matters, use first (lowest) op channel, and ht here */

	if (!rtw_get_op_chs_by_bcch_bw(BAND_ON_5G, cch, bw, &op_chs, &op_ch_num)) {
		RTW_WARN("%s invalid cch:%u bw:%s on %s\n", caller, cch, ch_width_str(bw), band_str(BAND_ON_5G));
		return false;
	}

	state = rtw_bchbw_to_cfg80211_chan_def(wiphy, chdef, BAND_ON_5G
		, op_chs[0], bw, bw == CHANNEL_WIDTH_20 ? CHAN_OFFSET_NO_EXT : CHAN_OFFSET_UPPER, 1);

	return state == _SUCCESS ? true : false;

}

void rtw_cfg80211_radar_detected_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 cch, enum channel_width bw)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	struct cfg80211_chan_def chdef;

	if (!rtw_cfg80211_get_radar_event_chdef(wiphy, &chdef, cch, bw, __func__))
		return;

	cfg80211_radar_event(wiphy, &chdef, GFP_KERNEL);
}

struct async_cac_change_evt {
	_list list; /* async_cac_change_list */
	struct net_device *netdev;
	struct cfg80211_chan_def chandef;
	enum nl80211_radar_event event;
};

static void async_cac_change_work_hdl(_workitem *work)
{
	struct rtw_wiphy_data *wiphy_data = container_of(work, struct rtw_wiphy_data, async_cac_change_work);
	struct async_cac_change_evt *evt;
	_list *list, *head = &wiphy_data->async_cac_change_list;

	while (1) {
		_rtw_mutex_lock_interruptible(&wiphy_data->async_cac_change_mutex);
		list = rtw_is_list_empty(head) ? NULL : get_next(head);
		if (list)
			rtw_list_delete(list);
		_rtw_mutex_unlock(&wiphy_data->async_cac_change_mutex);

		if (!list)
			break;

		evt = LIST_CONTAINOR(list, struct async_cac_change_evt, list);

		rtnl_lock();
		cfg80211_cac_event(evt->netdev, &evt->chandef, evt->event, GFP_KERNEL);
		rtnl_unlock();

		rtw_mfree(evt, sizeof(*evt));
	}
}

static int cfg80211_cac_event_async(struct net_device *netdev
	, const struct cfg80211_chan_def *chandef, enum nl80211_radar_event event)
{
	_adapter *adapter = rtw_netdev_priv(netdev);
	struct wiphy *wiphy = adapter_to_wiphy(adapter);
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	struct async_cac_change_evt *evt;

	evt = rtw_malloc(sizeof(*evt));
	if (!evt)
		return _FAIL;

	_rtw_init_listhead(&evt->list);
	evt->netdev = netdev;
	_rtw_memcpy(&evt->chandef, chandef, sizeof(*chandef));
	evt->event = event;

	_rtw_mutex_lock_interruptible(&wiphy_data->async_cac_change_mutex);

	rtw_list_insert_tail(&evt->list, &wiphy_data->async_cac_change_list);

	_rtw_mutex_unlock(&wiphy_data->async_cac_change_mutex);

	_set_workitem(&wiphy_data->async_cac_change_work);

	return _SUCCESS;
}

static void rtw_regd_async_cac_change_list_free(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);
	struct async_cac_change_evt *evt;
	_list *cur, *head;

	_rtw_mutex_lock_interruptible(&wiphy_data->async_cac_change_mutex);

	head = &wiphy_data->async_cac_change_list;
	cur = get_next(head);

	while ((rtw_end_of_queue_search(head, cur)) == _FALSE) {
		evt = LIST_CONTAINOR(cur, struct async_cac_change_evt, list);
		cur = get_next(cur);
		rtw_list_delete(&evt->list);
		rtw_mfree(evt, sizeof(*evt));
	}

	_rtw_mutex_unlock(&wiphy_data->async_cac_change_mutex);
}

static void rtw_cfg80211_cac_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw, enum nl80211_radar_event event
	, const char *caller)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	struct cfg80211_chan_def chdef;
	_adapter *iface;
	int i;
	bool async;

	if (!ifbmp) {
		RTW_WARN(FUNC_HWBAND_FMT" ifbmp is empty", caller, HWBAND_ARG(band_idx));
		return;
	}

	if (!rtw_cfg80211_get_radar_event_chdef(wiphy, &chdef, cch, bw, caller))
		return;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;
		if (!iface->rtw_wdev)
			continue;
		async = !iface->rtw_wdev->chandef.chan;
		RTW_INFO(FUNC_ADPT_FMT" async:%d\n", caller, ADPT_ARG(iface), async);
		if (async)
			cfg80211_cac_event_async(iface->pnetdev, &chdef, event);
		else
			cfg80211_cac_event(iface->pnetdev, &chdef, event, GFP_KERNEL);
	}
}

void rtw_cfg80211_cac_started_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;

	if (ifbmp == 0xFF) {
		ifbmp &= rtw_mi_get_ap_mesh_ifbmp_by_hwband(dvobj, band_idx);
		if (!ifbmp)
			return;
	}

	rtw_cfg80211_cac_event(rfctl, band_idx, ifbmp, cch, bw, NL80211_RADAR_CAC_STARTED, __func__);
}

void rtw_cfg80211_cac_finished_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	_adapter *iface;
	int i;

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;
		/* finish only for wdev with cac_started */
		if (!iface->rtw_wdev || !iface->rtw_wdev->cac_started)
			ifbmp &= ~BIT(iface->iface_id);
	}

	rtw_cfg80211_cac_event(rfctl, band_idx, ifbmp, cch, bw, NL80211_RADAR_CAC_FINISHED, __func__);
}

void rtw_cfg80211_cac_aborted_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	_adapter *iface;
	int i;

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;
		/* abort only for wdev with cac_started */
		if (!iface->rtw_wdev || !iface->rtw_wdev->cac_started)
			ifbmp &= ~BIT(iface->iface_id);
	}

	rtw_cfg80211_cac_event(rfctl, band_idx, ifbmp, cch, bw, NL80211_RADAR_CAC_ABORTED, __func__);
}

static void rtw_cfg80211_cac_force_started_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	rtw_cfg80211_cac_event(rfctl, band_idx, ifbmp, cch, bw, NL80211_RADAR_CAC_STARTED, __func__);
}

static void rtw_cfg80211_cac_force_finished_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	rtw_cfg80211_cac_event(rfctl, band_idx, ifbmp, cch, bw, NL80211_RADAR_CAC_FINISHED, __func__);
}

void rtw_cfg80211_cac_force_finished(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 ifbmp, u8 cch, enum channel_width bw)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	struct rtw_chset *chset = &rfctl->chset;
	_adapter *iface;
	u8 i;
	u8 *op_chs;
	u8 op_ch_num;
	RT_CHANNEL_INFO *chinfo;
	struct ieee80211_channel *chan;
	bool need_start = false;
	u8 finished_ifbmp, started_ifbmp;

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;

	if (ifbmp == 0xFF) {
		ifbmp &= rtw_mi_get_ap_mesh_ifbmp_by_hwband(dvobj, band_idx);
		if (!ifbmp)
			return;
	}

	/*
	* if coverage channels still USABLE and wdev is not cac_started
	* send CAC_STARTED to set cac_started and
	* send CAC_FINISHED to clear cac_started and make coverage channels AVAILABLE
	*/

	if (!rtw_get_op_chs_by_bcch_bw(BAND_ON_5G, cch, bw, &op_chs, &op_ch_num)) {
		RTW_WARN("%s invalid cch:%u bw:%s on %s\n", __func__, cch, ch_width_str(bw), band_str(BAND_ON_5G));
		return;
	}

	for (i = 0; i < op_ch_num; i++) {
		chinfo = rtw_chset_get_chinfo_by_bch(chset, BAND_ON_5G, op_chs[i], true);
		if (!chinfo) {
			RTW_WARN("%s can't get chinfo by ch:%u on %s\n", __func__, op_chs[i], band_str(BAND_ON_5G));
			continue;
		}
		if (!chinfo->os_chan) {
			RTW_WARN("%s ch:%u on %s has no os_chan\n", __func__, op_chs[i], band_str(BAND_ON_5G));
			continue;
		}

		chan = chinfo->os_chan;
		if (chan->dfs_state == NL80211_DFS_UNAVAILABLE) {
			RTW_WARN("%s ch:%u on %s is UNAVAILABLE(NON_OCP)\n", __func__, op_chs[i], band_str(BAND_ON_5G));
			return;
		}
		if (chan->dfs_state == NL80211_DFS_USABLE) {
			need_start = true;
			break;
		}
	}

	started_ifbmp = need_start ? ifbmp : 0;
	finished_ifbmp = ifbmp;
	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;
		if (!iface->rtw_wdev) {
			finished_ifbmp &= ~BIT(iface->iface_id);
			started_ifbmp &= ~BIT(iface->iface_id);
		}
		if (need_start && iface->rtw_wdev->cac_started)
			started_ifbmp &= ~BIT(iface->iface_id);
		else if (!need_start && !iface->rtw_wdev->cac_started)
			finished_ifbmp &= ~BIT(iface->iface_id);
	}

	/* send CAC_STARTED to set cac_started */
	if (started_ifbmp)
		rtw_cfg80211_cac_force_started_event(rfctl, band_idx, started_ifbmp, cch, bw);

	/* send CAC_FINISHED to clear cac_started and make coverage channels AVAILABLE */
	if (finished_ifbmp)
		rtw_cfg80211_cac_force_finished_event(rfctl, band_idx, finished_ifbmp, cch, bw);
}

void rtw_cfg80211_nop_finished_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 band, u8 cch, enum channel_width bw)
{
	/* non occupancy is controlled by net/wireless/mlme.c */
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;
}

void rtw_cfg80211_nop_started_event(struct rf_ctl_t *rfctl, u8 band_idx
	, u8 band, u8 cch, enum channel_width bw, bool called_on_cmd_thd)
{
	/* non occupancy is controlled by net/wireless/mlme.c */
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	struct wiphy *wiphy = dvobj_to_wiphy(dvobj); /* TODO: hwband specific wiphy mapping */
	struct get_chplan_resp *chplan;
	bool rtnl_lock_needed;

	if (!wiphy_ext_feature_isset(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD))
		return;

	if (rtw_get_chplan_cmd(dvobj_get_primary_adapter(dvobj)
		, called_on_cmd_thd ? RTW_CMDF_DIRECTLY : RTW_CMDF_WAIT_ACK, &chplan) == _FAIL)
		return;

	rtnl_lock_needed = rtw_rtnl_lock_needed(dvobj);
	if (rtnl_lock_needed)
		rtnl_lock();

	rtw_regd_override_dfs_state(wiphy, chplan, true);
	rtw_regd_schedule_dfs_chan_update(wiphy);

	if (rtnl_lock_needed)
		rtnl_unlock();

	rtw_free_get_chplan_resp(chplan);
}
#endif /* CONFIG_RTW_CFG80211_CAC_EVENT  */

int rtw_regd_init(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
	wiphy->reg_notifier = rtw_reg_notifier_return;
#else
	wiphy->reg_notifier = rtw_reg_notifier;
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
	wiphy->flags &= ~WIPHY_FLAG_STRICT_REGULATORY;
	wiphy->flags &= ~WIPHY_FLAG_DISABLE_BEACON_HINTS;
#else
	wiphy->regulatory_flags &= ~REGULATORY_STRICT_REG;
	wiphy->regulatory_flags &= ~REGULATORY_DISABLE_BEACON_HINTS;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	wiphy->regulatory_flags |= REGULATORY_IGNORE_STALE_KICKOFF;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	if (rtw_regd_is_wiphy_self_managed(wiphy))
		wiphy->regulatory_flags |= REGULATORY_WIPHY_SELF_MANAGED;
#endif

#if defined(CONFIG_DFS_MASTER) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
	if (rtw_regd_is_wiphy_self_managed(wiphy)
		&& rtw_rfctl_radar_detect_supported(dvobj_to_rfctl(wiphy_to_dvobj(wiphy)))
		&& wiphy->bands[NL80211_BAND_5GHZ])
		wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_DFS_OFFLOAD);
#endif

	_rtw_init_listhead(&wiphy_data->async_regd_change_list);
	_rtw_mutex_init(&wiphy_data->async_regd_change_mutex);
	_init_workitem(&wiphy_data->async_regd_change_work, async_regd_change_work_hdl, NULL);

#if CONFIG_RTW_CFG80211_CAC_EVENT
	_rtw_init_listhead(&wiphy_data->async_cac_change_list);
	_rtw_mutex_init(&wiphy_data->async_cac_change_mutex);
	_init_workitem(&wiphy_data->async_cac_change_work, async_cac_change_work_hdl, NULL);
#endif

	return 0;
}

void rtw_regd_deinit(struct wiphy *wiphy)
{
	struct rtw_wiphy_data *wiphy_data = rtw_wiphy_priv(wiphy);

	_cancel_workitem_sync(&wiphy_data->async_regd_change_work);
	rtw_regd_async_regd_change_list_free(wiphy);
	_rtw_mutex_free(&wiphy_data->async_regd_change_mutex);

#if CONFIG_RTW_CFG80211_CAC_EVENT
	_cancel_workitem_sync(&wiphy_data->async_cac_change_work);
	rtw_regd_async_cac_change_list_free(wiphy);
	_rtw_mutex_free(&wiphy_data->async_cac_change_mutex);
#endif
}
#endif /* CONFIG_IOCTL_CFG80211 */
