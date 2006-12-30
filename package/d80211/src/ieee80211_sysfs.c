/*
 * Copyright (c) 2006 Jiri Benc <jbenc@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/if.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <net/d80211.h>
#include "ieee80211_i.h"
#include "ieee80211_rate.h"

#define to_ieee80211_local(class) \
	container_of(class, struct ieee80211_local, class_dev)
#define to_net_dev(class) \
	container_of(class, struct net_device, class_dev)

static inline int rtnl_lock_local(struct ieee80211_local *local)
{
	rtnl_lock();
	if (unlikely(local->reg_state != IEEE80211_DEV_REGISTERED)) {
		rtnl_unlock();
		return -ENODEV;
	}
	return 0;
}

static const char *ieee80211_mode_str_short(int mode)
{
	switch (mode) {
	case MODE_IEEE80211A:
		return "802.11a";
	case MODE_IEEE80211B:
		return "802.11b";
	case MODE_IEEE80211G:
		return "802.11g";
	case MODE_ATHEROS_TURBO:
		return "AtherosTurbo";
	default:
		return "UNKNOWN";
	}
}

static const char *ieee80211_mode_str(int mode)
{
	switch (mode) {
	case MODE_IEEE80211A:
		return "IEEE 802.11a";
	case MODE_IEEE80211B:
		return "IEEE 802.11b";
	case MODE_IEEE80211G:
		return "IEEE 802.11g";
	case MODE_ATHEROS_TURBO:
		return "Atheros Turbo (5 GHz)";
	default:
		return "UNKNOWN";
	}
}

/* attributes in /sys/class/ieee80211/phyX/ */

static ssize_t store_add_iface(struct class_device *dev,
			       const char *buf, size_t len)
{
	struct ieee80211_local *local = to_ieee80211_local(dev);
	struct net_device *new_dev;
	int res;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (len > IFNAMSIZ)
		return -EINVAL;
	res = rtnl_lock_local(local);
	if (res)
		return res;
	res = ieee80211_if_add(local->mdev, buf, 0, &new_dev);
	if (res == 0)
		ieee80211_if_set_type(new_dev, IEEE80211_IF_TYPE_STA);
	rtnl_unlock();
	return res < 0 ? res : len;
}

static ssize_t store_remove_iface(struct class_device *dev,
				  const char *buf, size_t len)
{
	struct ieee80211_local *local = to_ieee80211_local(dev);
	int res;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (len > IFNAMSIZ)
		return -EINVAL;
	res = rtnl_lock_local(local);
	if (res)
		return res;
	res = ieee80211_if_remove(local->mdev, buf, -1);
	rtnl_unlock();
	return res < 0 ? res : len;
}

static ssize_t store_rate_ctrl_alg(struct class_device *dev,
				   const char *buf, size_t len)
{
	struct ieee80211_local *local = to_ieee80211_local(dev);
	int res;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	res = rtnl_lock_local(local);
	if (res)
		return res;
	res = ieee80211_init_rate_ctrl_alg(local, buf);
	rtnl_unlock();
	return res < 0 ? res : len;
}

static ssize_t ieee80211_local_show(struct class_device *dev, char *buf,
			ssize_t (*format)(struct ieee80211_local *, char *))
{
	struct ieee80211_local *local = to_ieee80211_local(dev);
	ssize_t ret = -EINVAL;

	if (local->reg_state == IEEE80211_DEV_REGISTERED)
		ret = (*format)(local, buf);
	return ret;
}

#define IEEE80211_LOCAL_FMT(name, field, format_string)			\
static ssize_t ieee80211_local_fmt_##name(struct ieee80211_local *local,\
					  char *buf)			\
{									\
	return sprintf(buf, format_string, local->field);		\
}

#define __IEEE80211_LOCAL_SHOW(name)					\
static ssize_t ieee80211_local_show_##name(struct class_device *cd,	\
					   char *buf)			\
{									\
	return ieee80211_local_show(cd, buf,				\
				    ieee80211_local_fmt_##name);	\
}

#define IEEE80211_LOCAL_SHOW(name, field, format)			\
		IEEE80211_LOCAL_FMT(name, field, format "\n")		\
		__IEEE80211_LOCAL_SHOW(name)

IEEE80211_LOCAL_SHOW(channel, hw.conf.channel, "%d");
IEEE80211_LOCAL_SHOW(frequency, hw.conf.freq, "%d");
IEEE80211_LOCAL_SHOW(radar_detect, hw.conf.radar_detect, "%d");
IEEE80211_LOCAL_SHOW(antenna_sel, hw.conf.antenna_sel, "%d");
IEEE80211_LOCAL_SHOW(bridge_packets, bridge_packets, "%d");
IEEE80211_LOCAL_SHOW(key_tx_rx_threshold, key_tx_rx_threshold, "%d");
IEEE80211_LOCAL_SHOW(rts_threshold, rts_threshold, "%d");
IEEE80211_LOCAL_SHOW(fragmentation_threshold, fragmentation_threshold, "%d");
IEEE80211_LOCAL_SHOW(short_retry_limit, short_retry_limit, "%d");
IEEE80211_LOCAL_SHOW(long_retry_limit, long_retry_limit, "%d");
IEEE80211_LOCAL_SHOW(total_ps_buffered, total_ps_buffered, "%d");

static ssize_t ieee80211_local_fmt_mode(struct ieee80211_local *local,
					char *buf)
{
	return sprintf(buf, "%s\n", ieee80211_mode_str(local->hw.conf.phymode));
}
__IEEE80211_LOCAL_SHOW(mode);

static ssize_t ieee80211_local_fmt_wep_iv(struct ieee80211_local *local,
					  char *buf)
{
	return sprintf(buf, "%#06x\n", local->wep_iv & 0xffffff);
}
__IEEE80211_LOCAL_SHOW(wep_iv);

static ssize_t ieee80211_local_fmt_tx_power_reduction(struct ieee80211_local
						      *local, char *buf)
{
	short tx_power_reduction = local->hw.conf.tx_power_reduction;

	return sprintf(buf, "%d.%d dBm\n", tx_power_reduction / 10,
		       tx_power_reduction % 10);
}
__IEEE80211_LOCAL_SHOW(tx_power_reduction);

static ssize_t ieee80211_local_fmt_modes(struct ieee80211_local *local,
					 char *buf)
{
	int i;
	struct ieee80211_hw_modes *mode;
	char *p = buf;

	/* FIXME: locking against ieee80211_update_hw? */
	for (i = 0; i < local->hw.num_modes; i++) {
		mode = &local->hw.modes[i];
		p += sprintf(p, "%s\n", ieee80211_mode_str_short(mode->mode));
	}
	return (p - buf);
}
__IEEE80211_LOCAL_SHOW(modes);

static ssize_t ieee80211_local_fmt_rate_ctrl_alg(struct ieee80211_local *local,
						 char *buf)
{
	struct rate_control_ref *ref = local->rate_ctrl;
	if (ref)
		return sprintf(buf, "%s\n", ref->ops->name);
	return 0;
}
__IEEE80211_LOCAL_SHOW(rate_ctrl_alg);

static struct class_device_attribute ieee80211_class_dev_attrs[] = {
	__ATTR(add_iface, S_IWUGO, NULL, store_add_iface),
	__ATTR(remove_iface, S_IWUGO, NULL, store_remove_iface),
	__ATTR(channel, S_IRUGO, ieee80211_local_show_channel, NULL),
	__ATTR(frequency, S_IRUGO, ieee80211_local_show_frequency, NULL),
	__ATTR(radar_detect, S_IRUGO, ieee80211_local_show_radar_detect, NULL),
	__ATTR(antenna_sel, S_IRUGO, ieee80211_local_show_antenna_sel, NULL),
	__ATTR(bridge_packets, S_IRUGO, ieee80211_local_show_bridge_packets, NULL),
	__ATTR(key_tx_rx_threshold, S_IRUGO, ieee80211_local_show_key_tx_rx_threshold, NULL),
	__ATTR(rts_threshold, S_IRUGO, ieee80211_local_show_rts_threshold, NULL),
	__ATTR(fragmentation_threshold, S_IRUGO, ieee80211_local_show_fragmentation_threshold, NULL),
	__ATTR(short_retry_limit, S_IRUGO, ieee80211_local_show_short_retry_limit, NULL),
	__ATTR(long_retry_limit, S_IRUGO, ieee80211_local_show_long_retry_limit, NULL),
	__ATTR(total_ps_buffered, S_IRUGO, ieee80211_local_show_total_ps_buffered, NULL),
	__ATTR(mode, S_IRUGO, ieee80211_local_show_mode, NULL),
	__ATTR(wep_iv, S_IRUGO, ieee80211_local_show_wep_iv, NULL),
	__ATTR(tx_power_reduction, S_IRUGO, ieee80211_local_show_tx_power_reduction, NULL),
	__ATTR(modes, S_IRUGO, ieee80211_local_show_modes, NULL),
	__ATTR(rate_ctrl_alg, S_IRUGO | S_IWUGO, ieee80211_local_show_rate_ctrl_alg, store_rate_ctrl_alg),
	{}
};

/* attributes in /sys/class/ieee80211/phyX/statistics/ */

#define IEEE80211_LOCAL_ATTR(name, field, format)			\
IEEE80211_LOCAL_SHOW(name, field, format)				\
static CLASS_DEVICE_ATTR(name, S_IRUGO, ieee80211_local_show_##name, NULL);

IEEE80211_LOCAL_ATTR(transmitted_fragment_count, dot11TransmittedFragmentCount, "%u");
IEEE80211_LOCAL_ATTR(multicast_transmitted_frame_count, dot11MulticastTransmittedFrameCount, "%u");
IEEE80211_LOCAL_ATTR(failed_count, dot11FailedCount, "%u");
IEEE80211_LOCAL_ATTR(retry_count, dot11RetryCount, "%u");
IEEE80211_LOCAL_ATTR(multiple_retry_count, dot11MultipleRetryCount, "%u");
IEEE80211_LOCAL_ATTR(frame_duplicate_count, dot11FrameDuplicateCount, "%u");
IEEE80211_LOCAL_ATTR(received_fragment_count, dot11ReceivedFragmentCount, "%u");
IEEE80211_LOCAL_ATTR(multicast_received_frame_count, dot11MulticastReceivedFrameCount, "%u");
IEEE80211_LOCAL_ATTR(transmitted_frame_count, dot11TransmittedFrameCount, "%u");
IEEE80211_LOCAL_ATTR(wep_undecryptable_count, dot11WEPUndecryptableCount, "%u");
IEEE80211_LOCAL_ATTR(num_scans, scan.num_scans, "%u");

#ifdef CONFIG_D80211_DEBUG_COUNTERS
IEEE80211_LOCAL_ATTR(tx_handlers_drop, tx_handlers_drop, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_queued, tx_handlers_queued, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_drop_unencrypted, tx_handlers_drop_unencrypted, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_drop_fragment, tx_handlers_drop_fragment, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_drop_wep, tx_handlers_drop_wep, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_drop_not_assoc, tx_handlers_drop_not_assoc, "%u");
IEEE80211_LOCAL_ATTR(tx_handlers_drop_unauth_port, tx_handlers_drop_unauth_port, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_drop, rx_handlers_drop, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_queued, rx_handlers_queued, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_drop_nullfunc, rx_handlers_drop_nullfunc, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_drop_defrag, rx_handlers_drop_defrag, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_drop_short, rx_handlers_drop_short, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_drop_passive_scan, rx_handlers_drop_passive_scan, "%u");
IEEE80211_LOCAL_ATTR(tx_expand_skb_head, tx_expand_skb_head, "%u");
IEEE80211_LOCAL_ATTR(tx_expand_skb_head_cloned, tx_expand_skb_head_cloned, "%u");
IEEE80211_LOCAL_ATTR(rx_expand_skb_head, rx_expand_skb_head, "%u");
IEEE80211_LOCAL_ATTR(rx_expand_skb_head2, rx_expand_skb_head2, "%u");
IEEE80211_LOCAL_ATTR(rx_handlers_fragments, rx_handlers_fragments, "%u");
IEEE80211_LOCAL_ATTR(tx_status_drop, tx_status_drop, "%u");

static ssize_t ieee80211_local_fmt_wme_rx_queue(struct ieee80211_local *local,
						char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u\n", local->wme_rx_queue[i]);
	return (p - buf);
}
__IEEE80211_LOCAL_SHOW(wme_rx_queue);
static CLASS_DEVICE_ATTR(wme_rx_queue, S_IRUGO,
			 ieee80211_local_show_wme_rx_queue, NULL);

static ssize_t ieee80211_local_fmt_wme_tx_queue(struct ieee80211_local *local,
						char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u\n", local->wme_tx_queue[i]);
	return (p - buf);
}
__IEEE80211_LOCAL_SHOW(wme_tx_queue);
static CLASS_DEVICE_ATTR(wme_tx_queue, S_IRUGO,
			 ieee80211_local_show_wme_tx_queue, NULL);
#endif

static ssize_t ieee80211_stats_show(struct class_device *dev, char *buf,
		ssize_t (*format)(struct ieee80211_low_level_stats *, char *))
{
	struct ieee80211_local *local = to_ieee80211_local(dev);
	struct ieee80211_low_level_stats stats;
	ssize_t ret = -EINVAL;

	if (!local->ops->get_stats)
		return -EOPNOTSUPP;
	ret = rtnl_lock_local(local);
	if (ret)
		return ret;
	ret = local->ops->get_stats(local_to_hw(local), &stats);
	rtnl_unlock();
	if (!ret)
		ret = (*format)(&stats, buf);
	return ret;
}

#define IEEE80211_STATS_FMT(name, field, format_string)			\
static ssize_t ieee80211_stats_fmt_##name(struct ieee80211_low_level_stats \
					  *stats, char *buf)		\
{									\
	return sprintf(buf, format_string, stats->field);		\
}

#define __IEEE80211_STATS_SHOW(name)					\
static ssize_t ieee80211_stats_show_##name(struct class_device *cd,	\
					   char *buf)			\
{									\
	return ieee80211_stats_show(cd, buf,				\
				    ieee80211_stats_fmt_##name);	\
}

#define IEEE80211_STATS_ATTR(name, field, format)			\
IEEE80211_STATS_FMT(name, field, format "\n")				\
__IEEE80211_STATS_SHOW(name)						\
static CLASS_DEVICE_ATTR(name, S_IRUGO, ieee80211_stats_show_##name, NULL);

IEEE80211_STATS_ATTR(ack_failure_count, dot11ACKFailureCount, "%u");
IEEE80211_STATS_ATTR(rts_failure_count, dot11RTSFailureCount, "%u");
IEEE80211_STATS_ATTR(fcs_error_count, dot11FCSErrorCount, "%u");
IEEE80211_STATS_ATTR(rts_success_count, dot11RTSSuccessCount, "%u");

static struct attribute *ieee80211_stats_attrs[] = {
	&class_device_attr_transmitted_fragment_count.attr,
	&class_device_attr_multicast_transmitted_frame_count.attr,
	&class_device_attr_failed_count.attr,
	&class_device_attr_retry_count.attr,
	&class_device_attr_multiple_retry_count.attr,
	&class_device_attr_frame_duplicate_count.attr,
	&class_device_attr_received_fragment_count.attr,
	&class_device_attr_multicast_received_frame_count.attr,
	&class_device_attr_transmitted_frame_count.attr,
	&class_device_attr_wep_undecryptable_count.attr,
	&class_device_attr_ack_failure_count.attr,
	&class_device_attr_rts_failure_count.attr,
	&class_device_attr_fcs_error_count.attr,
	&class_device_attr_rts_success_count.attr,
	&class_device_attr_num_scans.attr,
#ifdef CONFIG_D80211_DEBUG_COUNTERS
	&class_device_attr_tx_handlers_drop.attr,
	&class_device_attr_tx_handlers_queued.attr,
	&class_device_attr_tx_handlers_drop_unencrypted.attr,
	&class_device_attr_tx_handlers_drop_fragment.attr,
	&class_device_attr_tx_handlers_drop_wep.attr,
	&class_device_attr_tx_handlers_drop_not_assoc.attr,
	&class_device_attr_tx_handlers_drop_unauth_port.attr,
	&class_device_attr_rx_handlers_drop.attr,
	&class_device_attr_rx_handlers_queued.attr,
	&class_device_attr_rx_handlers_drop_nullfunc.attr,
	&class_device_attr_rx_handlers_drop_defrag.attr,
	&class_device_attr_rx_handlers_drop_short.attr,
	&class_device_attr_rx_handlers_drop_passive_scan.attr,
	&class_device_attr_tx_expand_skb_head.attr,
	&class_device_attr_tx_expand_skb_head_cloned.attr,
	&class_device_attr_rx_expand_skb_head.attr,
	&class_device_attr_rx_expand_skb_head2.attr,
	&class_device_attr_rx_handlers_fragments.attr,
	&class_device_attr_tx_status_drop.attr,
	&class_device_attr_wme_rx_queue.attr,
	&class_device_attr_wme_tx_queue.attr,
#endif
	NULL,
};

static struct attribute_group ieee80211_stats_group = {
	.name = "statistics",
	.attrs = ieee80211_stats_attrs,
};

/* attributes in /sys/class/net/X/ */

static ssize_t ieee80211_if_show(struct class_device *cd, char *buf,
			ssize_t (*format)(const struct ieee80211_sub_if_data *,
					  char *))
{
	struct net_device *dev = to_net_dev(cd);
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	ssize_t ret = -EINVAL;

	read_lock(&dev_base_lock);
	if (dev->reg_state == NETREG_REGISTERED) {
		ret = (*format)(sdata, buf);
	}
	read_unlock(&dev_base_lock);
	return ret;
}

#define IEEE80211_IF_FMT(name, field, format_string)			\
static ssize_t ieee80211_if_fmt_##name(const struct			\
			ieee80211_sub_if_data *sdata, char *buf)	\
{									\
	return sprintf(buf, format_string, sdata->field);		\
}
#define IEEE80211_IF_FMT_DEC(name, field)				\
		IEEE80211_IF_FMT(name, field, "%d\n")
#define IEEE80211_IF_FMT_HEX(name, field)				\
		IEEE80211_IF_FMT(name, field, "%#x\n")
#define IEEE80211_IF_FMT_SIZE(name, field)				\
		IEEE80211_IF_FMT(name, field, "%zd\n")

#define IEEE80211_IF_FMT_ATOMIC(name, field)				\
static ssize_t ieee80211_if_fmt_##name(const struct			\
			ieee80211_sub_if_data *sdata, char *buf)	\
{									\
	return sprintf(buf, "%d\n", atomic_read(&sdata->field));	\
}

#define IEEE80211_IF_FMT_MAC(name, field)				\
static ssize_t ieee80211_if_fmt_##name(const struct			\
			ieee80211_sub_if_data *sdata, char *buf)	\
{									\
	return sprintf(buf, MAC_FMT "\n", MAC_ARG(sdata->field));	\
}

#define __IEEE80211_IF_SHOW(name)					\
static ssize_t ieee80211_if_show_##name(struct class_device *cd,	\
					char *buf)			\
{									\
	return ieee80211_if_show(cd, buf, ieee80211_if_fmt_##name);	\
}									\
static CLASS_DEVICE_ATTR(name, S_IRUGO, ieee80211_if_show_##name, NULL);

#define IEEE80211_IF_SHOW(name, field, format)				\
		IEEE80211_IF_FMT_##format(name, field)			\
		__IEEE80211_IF_SHOW(name)

/* common attributes */
IEEE80211_IF_SHOW(channel_use, channel_use, DEC);
IEEE80211_IF_SHOW(drop_unencrypted, drop_unencrypted, DEC);
IEEE80211_IF_SHOW(eapol, eapol, DEC);
IEEE80211_IF_SHOW(ieee8021_x, ieee802_1x, DEC);

/* STA/IBSS attributes */
IEEE80211_IF_SHOW(state, u.sta.state, DEC);
IEEE80211_IF_SHOW(bssid, u.sta.bssid, MAC);
IEEE80211_IF_SHOW(prev_bssid, u.sta.prev_bssid, MAC);
IEEE80211_IF_SHOW(ssid_len, u.sta.ssid_len, SIZE);
IEEE80211_IF_SHOW(aid, u.sta.aid, DEC);
IEEE80211_IF_SHOW(ap_capab, u.sta.ap_capab, HEX);
IEEE80211_IF_SHOW(capab, u.sta.capab, HEX);
IEEE80211_IF_SHOW(extra_ie_len, u.sta.extra_ie_len, SIZE);
IEEE80211_IF_SHOW(auth_tries, u.sta.auth_tries, DEC);
IEEE80211_IF_SHOW(assoc_tries, u.sta.assoc_tries, DEC);
IEEE80211_IF_SHOW(auth_algs, u.sta.auth_algs, HEX);
IEEE80211_IF_SHOW(auth_alg, u.sta.auth_alg, DEC);
IEEE80211_IF_SHOW(auth_transaction, u.sta.auth_transaction, DEC);

static ssize_t ieee80211_if_fmt_flags(const struct
			ieee80211_sub_if_data *sdata, char *buf)
{
	return sprintf(buf, "%s%s%s%s%s%s%s\n",
		       sdata->u.sta.ssid_set ? "SSID\n" : "",
		       sdata->u.sta.bssid_set ? "BSSID\n" : "",
		       sdata->u.sta.prev_bssid_set ? "prev BSSID\n" : "",
		       sdata->u.sta.authenticated ? "AUTH\n" : "",
		       sdata->u.sta.associated ? "ASSOC\n" : "",
		       sdata->u.sta.probereq_poll ? "PROBEREQ POLL\n" : "",
		       sdata->u.sta.use_protection ? "CTS prot\n" : "");
}
__IEEE80211_IF_SHOW(flags);

/* AP attributes */
IEEE80211_IF_SHOW(num_sta_ps, u.ap.num_sta_ps, ATOMIC);
IEEE80211_IF_SHOW(dtim_period, u.ap.dtim_period, DEC);
IEEE80211_IF_SHOW(dtim_count, u.ap.dtim_count, DEC);
IEEE80211_IF_SHOW(num_beacons, u.ap.num_beacons, DEC);
IEEE80211_IF_SHOW(force_unicast_rateidx, u.ap.force_unicast_rateidx, DEC);
IEEE80211_IF_SHOW(max_ratectrl_rateidx, u.ap.max_ratectrl_rateidx, DEC);

static ssize_t ieee80211_if_fmt_num_buffered_multicast(const struct
			ieee80211_sub_if_data *sdata, char *buf)
{
	return sprintf(buf, "%u\n", skb_queue_len(&sdata->u.ap.ps_bc_buf));
}
__IEEE80211_IF_SHOW(num_buffered_multicast);

static ssize_t ieee80211_if_fmt_beacon_head_len(const struct
			ieee80211_sub_if_data *sdata, char *buf)
{
	if (sdata->u.ap.beacon_head)
		return sprintf(buf, "%d\n", sdata->u.ap.beacon_head_len);
	return sprintf(buf, "\n");
}
__IEEE80211_IF_SHOW(beacon_head_len);

static ssize_t ieee80211_if_fmt_beacon_tail_len(const struct
			ieee80211_sub_if_data *sdata, char *buf)
{
	if (sdata->u.ap.beacon_tail)
		return sprintf(buf, "%d\n", sdata->u.ap.beacon_tail_len);
	return sprintf(buf, "\n");
}
__IEEE80211_IF_SHOW(beacon_tail_len);

/* WDS attributes */
IEEE80211_IF_SHOW(peer, u.wds.remote_addr, MAC);

/* VLAN attributes */
IEEE80211_IF_SHOW(vlan_id, u.vlan.id, DEC);

/* MONITOR attributes */
static ssize_t ieee80211_if_fmt_mode(const struct
			ieee80211_sub_if_data *sdata, char *buf)
{
	struct ieee80211_local *local = sdata->local;

	return sprintf(buf, "%s\n",
		       ((local->hw.flags & IEEE80211_HW_MONITOR_DURING_OPER) ||
			local->open_count == local->monitors) ?
		       "hard" : "soft");
}
__IEEE80211_IF_SHOW(mode);

static struct attribute *ieee80211_sta_attrs[] = {
	&class_device_attr_channel_use.attr,
	&class_device_attr_drop_unencrypted.attr,
	&class_device_attr_eapol.attr,
	&class_device_attr_ieee8021_x.attr,
	&class_device_attr_state.attr,
	&class_device_attr_bssid.attr,
	&class_device_attr_prev_bssid.attr,
	&class_device_attr_ssid_len.attr,
	&class_device_attr_aid.attr,
	&class_device_attr_ap_capab.attr,
	&class_device_attr_capab.attr,
	&class_device_attr_extra_ie_len.attr,
	&class_device_attr_auth_tries.attr,
	&class_device_attr_assoc_tries.attr,
	&class_device_attr_auth_algs.attr,
	&class_device_attr_auth_alg.attr,
	&class_device_attr_auth_transaction.attr,
	&class_device_attr_flags.attr,
	NULL
};

static struct attribute *ieee80211_ap_attrs[] = {
	&class_device_attr_channel_use.attr,
	&class_device_attr_drop_unencrypted.attr,
	&class_device_attr_eapol.attr,
	&class_device_attr_ieee8021_x.attr,
	&class_device_attr_num_sta_ps.attr,
	&class_device_attr_dtim_period.attr,
	&class_device_attr_dtim_count.attr,
	&class_device_attr_num_beacons.attr,
	&class_device_attr_force_unicast_rateidx.attr,
	&class_device_attr_max_ratectrl_rateidx.attr,
	&class_device_attr_num_buffered_multicast.attr,
	&class_device_attr_beacon_head_len.attr,
	&class_device_attr_beacon_tail_len.attr,
	NULL
};

static struct attribute *ieee80211_wds_attrs[] = {
	&class_device_attr_channel_use.attr,
	&class_device_attr_drop_unencrypted.attr,
	&class_device_attr_eapol.attr,
	&class_device_attr_ieee8021_x.attr,
	&class_device_attr_peer.attr,
	NULL
};

static struct attribute *ieee80211_vlan_attrs[] = {
	&class_device_attr_channel_use.attr,
	&class_device_attr_drop_unencrypted.attr,
	&class_device_attr_eapol.attr,
	&class_device_attr_ieee8021_x.attr,
	&class_device_attr_vlan_id.attr,
	NULL
};

static struct attribute *ieee80211_monitor_attrs[] = {
	&class_device_attr_mode.attr,
	NULL
};

static struct attribute_group ieee80211_sta_group = {
	.name = "sta",
	.attrs = ieee80211_sta_attrs,
};

static struct attribute_group ieee80211_ap_group = {
	.name = "ap",
	.attrs = ieee80211_ap_attrs,
};

static struct attribute_group ieee80211_wds_group = {
	.name = "wds",
	.attrs = ieee80211_wds_attrs,
};

static struct attribute_group ieee80211_vlan_group = {
	.name = "vlan",
	.attrs = ieee80211_vlan_attrs,
};

static struct attribute_group ieee80211_monitor_group = {
	.name = "monitor",
	.attrs = ieee80211_monitor_attrs,
};

/* /sys/class/ieee80211/phyX functions */

static void ieee80211_class_dev_release(struct class_device *dev)
{
	ieee80211_release_hw(to_ieee80211_local(dev));
}

#ifdef CONFIG_HOTPLUG
static int ieee80211_uevent(struct class_device *cd, char **envp,
			    int num_envp, char *buf, int size)
{
	struct ieee80211_local *local = to_ieee80211_local(cd);

	if (num_envp < 2)
		return -ENOMEM;
	envp[0] = buf;
	if (snprintf(buf, size, "IEEE80211_DEV=phy%d",
		     local->hw.index) + 1 >= size)
		return -ENOMEM;
	envp[1] = NULL;
	return 0;
}
#endif

static struct class ieee80211_class = {
	.name = "ieee80211",
	.class_dev_attrs = ieee80211_class_dev_attrs,
	.release = ieee80211_class_dev_release,
#ifdef CONFIG_HOTPLUG
	.uevent = ieee80211_uevent,
#endif
};

void ieee80211_dev_sysfs_init(struct ieee80211_local *local)
{
	local->class_dev.class = &ieee80211_class;
	local->class_dev.class_data = local;
	class_device_initialize(&local->class_dev);
}

void ieee80211_dev_sysfs_put(struct ieee80211_local *local)
{
	class_device_put(&local->class_dev);
}

int ieee80211_dev_sysfs_add(struct ieee80211_local *local)
{
	int res;

	snprintf(local->class_dev.class_id, BUS_ID_SIZE,
		 "phy%d", local->hw.index);
	res = class_device_add(&local->class_dev);
	if (res)
		return res;
	res = sysfs_create_group(&local->class_dev.kobj,
				 &ieee80211_stats_group);
	if (res)
		class_device_del(&local->class_dev);
	return res;
}

void ieee80211_dev_sysfs_del(struct ieee80211_local *local)
{
	sysfs_remove_group(&local->class_dev.kobj, &ieee80211_stats_group);
	class_device_del(&local->class_dev);
}

/* /sys/class/net/X functions */

static void __ieee80211_remove_if_group(struct kobject *kobj,
					struct ieee80211_sub_if_data *sdata)
{
	if (sdata->sysfs_group) {
		sysfs_remove_group(kobj, sdata->sysfs_group);
		sdata->sysfs_group = NULL;
	}
}

static inline void ieee80211_remove_if_group(struct kobject *kobj,
					     struct net_device *dev)
{
	__ieee80211_remove_if_group(kobj, IEEE80211_DEV_TO_SUB_IF(dev));
}

static int ieee80211_add_if_group(struct kobject *kobj,
				  struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	int res = 0;

	__ieee80211_remove_if_group(kobj, sdata);
	switch (sdata->type) {
	case IEEE80211_IF_TYPE_STA:
		sdata->sysfs_group = &ieee80211_sta_group;
		break;
	case IEEE80211_IF_TYPE_AP:
		sdata->sysfs_group = &ieee80211_ap_group;
		break;
	case IEEE80211_IF_TYPE_WDS:
		sdata->sysfs_group = &ieee80211_wds_group;
		break;
	case IEEE80211_IF_TYPE_VLAN:
		sdata->sysfs_group = &ieee80211_vlan_group;
		break;
	case IEEE80211_IF_TYPE_MNTR:
		sdata->sysfs_group = &ieee80211_monitor_group;
		break;
	default:
		goto out;
	}
	res = sysfs_create_group(kobj, sdata->sysfs_group);
	if (res)
		sdata->sysfs_group = NULL;
out:
	return res;
}

int ieee80211_sysfs_change_if_type(struct net_device *dev)
{
	return ieee80211_add_if_group(&dev->class_dev.kobj, dev);
}

int ieee80211_sysfs_add_netdevice(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int res;

	res = sysfs_create_link(&dev->class_dev.kobj, &local->class_dev.kobj,
				"hw");
	if (res)
		goto err_out;
	res = ieee80211_add_if_group(&dev->class_dev.kobj, dev);
	if (res)
		goto err_link;
	res = ieee80211_key_kset_sysfs_register(IEEE80211_DEV_TO_SUB_IF(dev));
	return res;

err_link:
	sysfs_remove_link(&dev->class_dev.kobj, "hw");
err_out:
	return res;
}

void ieee80211_sysfs_remove_netdevice(struct net_device *dev)
{
	ieee80211_key_kset_sysfs_unregister(IEEE80211_DEV_TO_SUB_IF(dev));
	ieee80211_remove_if_group(&dev->class_dev.kobj, dev);
	sysfs_remove_link(&dev->class_dev.kobj, "hw");
}

/* general module functions */

int ieee80211_sysfs_init(void)
{
	return class_register(&ieee80211_class);
}

void ieee80211_sysfs_deinit(void)
{
	class_unregister(&ieee80211_class);
}
