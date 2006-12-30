/*
 * Copyright 2003-2005, Devicescape Software, Inc.
 * Copyright (c) 2006 Jiri Benc <jbenc@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kobject.h>
#include <linux/sysfs.h>
#include "ieee80211_i.h"
#include "ieee80211_key.h"
#include "sta_info.h"

static ssize_t sta_sysfs_show(struct kobject *, struct attribute *, char *);
static ssize_t key_sysfs_show(struct kobject *, struct attribute *, char *);

static struct sysfs_ops sta_ktype_ops = {
	.show = sta_sysfs_show,
};

static struct sysfs_ops key_ktype_ops = {
	.show = key_sysfs_show,
};

/* sta attributtes */

#define STA_SHOW(name, field, format_string)				\
static ssize_t show_sta_##name(const struct sta_info *sta, char *buf)	\
{									\
	return sprintf(buf, format_string, sta->field);			\
}
#define STA_SHOW_D(name, field) STA_SHOW(name, field, "%d\n")
#define STA_SHOW_U(name, field) STA_SHOW(name, field, "%u\n")
#define STA_SHOW_LU(name, field) STA_SHOW(name, field, "%lu\n")
#define STA_SHOW_S(name, field) STA_SHOW(name, field, "%s\n")

#define STA_SHOW_RATE(name, field)					\
static ssize_t show_sta_##name(const struct sta_info *sta, char *buf)	\
{									\
	struct ieee80211_local *local = sta->dev->ieee80211_ptr;	\
	return sprintf(buf, "%d\n",					\
		       (sta->field >= 0 &&				\
			sta->field < local->num_curr_rates) ?		\
		       local->curr_rates[sta->field].rate : -1);	\
}

#define __STA_ATTR(name)						\
static struct sta_attribute sta_attr_##name =				\
	__ATTR(name, S_IRUGO, show_sta_##name, NULL)

#define STA_ATTR(name, field, format)					\
		STA_SHOW_##format(name, field)				\
		__STA_ATTR(name)

STA_ATTR(aid, aid, D);
STA_ATTR(key_idx_compression, key_idx_compression, D);
STA_ATTR(dev, dev->name, S);
STA_ATTR(vlan_id, vlan_id, D);
STA_ATTR(rx_packets, rx_packets, LU);
STA_ATTR(tx_packets, tx_packets, LU);
STA_ATTR(rx_bytes, rx_bytes, LU);
STA_ATTR(tx_bytes, tx_bytes, LU);
STA_ATTR(rx_duplicates, num_duplicates, LU);
STA_ATTR(rx_fragments, rx_fragments, LU);
STA_ATTR(rx_dropped, rx_dropped, LU);
STA_ATTR(tx_fragments, tx_fragments, LU);
STA_ATTR(tx_filtered, tx_filtered_count, LU);
STA_ATTR(txrate, txrate, RATE);
STA_ATTR(last_txrate, last_txrate, RATE);
STA_ATTR(tx_retry_failed, tx_retry_failed, LU);
STA_ATTR(tx_retry_count, tx_retry_count, LU);
STA_ATTR(last_rssi, last_rssi, D);
STA_ATTR(last_signal, last_signal, D);
STA_ATTR(last_noise, last_noise, D);
STA_ATTR(channel_use, channel_use, D);
STA_ATTR(wep_weak_iv_count, wep_weak_iv_count, D);

static ssize_t show_sta_flags(const struct sta_info *sta, char *buf)
{
	return sprintf(buf, "%s%s%s%s%s%s%s%s%s",
		       sta->flags & WLAN_STA_AUTH ? "AUTH\n" : "",
		       sta->flags & WLAN_STA_ASSOC ? "ASSOC\n" : "",
		       sta->flags & WLAN_STA_PS ? "PS\n" : "",
		       sta->flags & WLAN_STA_TIM ? "TIM\n" : "",
		       sta->flags & WLAN_STA_PERM ? "PERM\n" : "",
		       sta->flags & WLAN_STA_AUTHORIZED ? "AUTHORIZED\n" : "",
		       sta->flags & WLAN_STA_SHORT_PREAMBLE ?
		       "SHORT PREAMBLE\n" : "",
		       sta->flags & WLAN_STA_WME ? "WME\n" : "",
		       sta->flags & WLAN_STA_WDS ? "WDS\n" : "");
}
__STA_ATTR(flags);

static ssize_t show_sta_num_ps_buf_frames(const struct sta_info *sta, char *buf)
{
	return sprintf(buf, "%u\n", skb_queue_len(&sta->ps_tx_buf));
}
__STA_ATTR(num_ps_buf_frames);

static ssize_t show_sta_last_ack_rssi(const struct sta_info *sta, char *buf)
{
	return sprintf(buf, "%d %d %d\n", sta->last_ack_rssi[0],
		       sta->last_ack_rssi[1], sta->last_ack_rssi[2]);
}
__STA_ATTR(last_ack_rssi);

static ssize_t show_sta_last_ack_ms(const struct sta_info *sta, char *buf)
{
	return sprintf(buf, "%d\n", sta->last_ack ?
		       jiffies_to_msecs(jiffies - sta->last_ack) : -1);
}
__STA_ATTR(last_ack_ms);

static ssize_t show_sta_inactive_ms(const struct sta_info *sta, char *buf)
{
	return sprintf(buf, "%d\n", jiffies_to_msecs(jiffies - sta->last_rx));
}
__STA_ATTR(inactive_ms);

static ssize_t show_sta_last_seq_ctrl(const struct sta_info *sta, char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%x ", sta->last_seq_ctrl[i]);
	p += sprintf(p, "\n");
	return (p - buf);
}
__STA_ATTR(last_seq_ctrl);

#ifdef CONFIG_D80211_DEBUG_COUNTERS
static ssize_t show_sta_wme_rx_queue(const struct sta_info *sta, char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u ", sta->wme_rx_queue[i]);
	p += sprintf(p, "\n");
	return (p - buf);
}
__STA_ATTR(wme_rx_queue);

static ssize_t show_sta_wme_tx_queue(const struct sta_info *sta, char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u ", sta->wme_tx_queue[i]);
	p += sprintf(p, "\n");
	return (p - buf);
}
__STA_ATTR(wme_tx_queue);
#endif

static struct attribute *sta_ktype_attrs[] = {
	&sta_attr_aid.attr,
	&sta_attr_key_idx_compression.attr,
	&sta_attr_dev.attr,
	&sta_attr_vlan_id.attr,
	&sta_attr_rx_packets.attr,
	&sta_attr_tx_packets.attr,
	&sta_attr_rx_bytes.attr,
	&sta_attr_tx_bytes.attr,
	&sta_attr_rx_duplicates.attr,
	&sta_attr_rx_fragments.attr,
	&sta_attr_rx_dropped.attr,
	&sta_attr_tx_fragments.attr,
	&sta_attr_tx_filtered.attr,
	&sta_attr_txrate.attr,
	&sta_attr_last_txrate.attr,
	&sta_attr_tx_retry_failed.attr,
	&sta_attr_tx_retry_count.attr,
	&sta_attr_last_rssi.attr,
	&sta_attr_last_signal.attr,
	&sta_attr_last_noise.attr,
	&sta_attr_channel_use.attr,
	&sta_attr_wep_weak_iv_count.attr,

	&sta_attr_flags.attr,
	&sta_attr_num_ps_buf_frames.attr,
	&sta_attr_last_ack_rssi.attr,
	&sta_attr_last_ack_ms.attr,
	&sta_attr_inactive_ms.attr,
	&sta_attr_last_seq_ctrl.attr,
#ifdef CONFIG_D80211_DEBUG_COUNTERS
	&sta_attr_wme_rx_queue.attr,
	&sta_attr_wme_tx_queue.attr,
#endif
	NULL
};

/* keys attributtes */

struct key_attribute {
	struct attribute attr;
	ssize_t (*show)(const struct ieee80211_key *, char *buf);
	ssize_t (*store)(struct ieee80211_key *, const char *buf,
			 size_t count);
};

#define KEY_SHOW(name, field, format_string)				\
static ssize_t show_key_##name(const struct ieee80211_key *key, char *buf)\
{									\
	return sprintf(buf, format_string, key->field);			\
}
#define KEY_SHOW_D(name, field) KEY_SHOW(name, field, "%d\n")

#define __KEY_ATTR(name)						\
static struct key_attribute key_attr_##name =				\
	__ATTR(name, S_IRUSR, show_key_##name, NULL)

#define KEY_ATTR(name, field, format)					\
		KEY_SHOW_##format(name, field)				\
		__KEY_ATTR(name)

KEY_ATTR(length, keylen, D);
KEY_ATTR(sw_encrypt, force_sw_encrypt, D);
KEY_ATTR(index, keyidx, D);
KEY_ATTR(hw_index, hw_key_idx, D);
KEY_ATTR(tx_rx_count, tx_rx_count, D);

static ssize_t show_key_algorithm(const struct ieee80211_key *key, char *buf)
{
	char *alg;

	switch (key->alg) {
	case ALG_WEP:
		alg = "WEP";
		break;
	case ALG_TKIP:
		alg = "TKIP";
		break;
	case ALG_CCMP:
		alg = "CCMP";
		break;
	default:
		return 0;
	}
	return sprintf(buf, "%s\n", alg);
}
__KEY_ATTR(algorithm);

static ssize_t show_key_tx_spec(const struct ieee80211_key *key, char *buf)
{
	const u8 *tpn;

	switch (key->alg) {
	case ALG_WEP:
		return sprintf(buf, "\n");
	case ALG_TKIP:
		return sprintf(buf, "%08x %04x\n", key->u.tkip.iv32,
			       key->u.tkip.iv16);
	case ALG_CCMP:
		tpn = key->u.ccmp.tx_pn;
		return sprintf(buf, "%02x%02x%02x%02x%02x%02x\n", tpn[0],
			       tpn[1], tpn[2], tpn[3], tpn[4], tpn[5]);
	default:
		return 0;
	}
}
__KEY_ATTR(tx_spec);

static ssize_t show_key_rx_spec(const struct ieee80211_key *key, char *buf)
{
	int i;
	const u8 *rpn;
	char *p = buf;

	switch (key->alg) {
	case ALG_WEP:
		return sprintf(buf, "\n");
	case ALG_TKIP:
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
			p += sprintf(p, "%08x %04x\n",
				     key->u.tkip.iv32_rx[i],
				     key->u.tkip.iv16_rx[i]);
		return (p - buf);
	case ALG_CCMP:
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++) {
			rpn = key->u.ccmp.rx_pn[i];
			p += sprintf(p, "%02x%02x%02x%02x%02x%02x\n", rpn[0],
				     rpn[1], rpn[2], rpn[3], rpn[4], rpn[5]);
		}
		return (p - buf);
	default:
		return 0;
	}
}
__KEY_ATTR(rx_spec);

static ssize_t show_key_replays(const struct ieee80211_key *key, char *buf)
{
	if (key->alg != ALG_CCMP)
		return 0;
	return sprintf(buf, "%u\n", key->u.ccmp.replays);
}
__KEY_ATTR(replays);

static ssize_t show_key_key(const struct ieee80211_key *key, char *buf)
{
	int i;
	char *p = buf;

	for (i = 0; i < key->keylen; i++)
		p += sprintf(p, "%02x", key->key[i]);
	p += sprintf(p, "\n");
	return (p - buf);
}
__KEY_ATTR(key);

static struct attribute *key_ktype_attrs[] = {
	&key_attr_length.attr,
	&key_attr_sw_encrypt.attr,
	&key_attr_index.attr,
	&key_attr_hw_index.attr,
	&key_attr_tx_rx_count.attr,
	&key_attr_algorithm.attr,
	&key_attr_tx_spec.attr,
	&key_attr_rx_spec.attr,
	&key_attr_replays.attr,
	&key_attr_key.attr,
	NULL
};

/* structures and functions */

static struct kobj_type sta_ktype = {
	.release = sta_info_release,
	.sysfs_ops = &sta_ktype_ops,
	.default_attrs = sta_ktype_attrs,
};

static struct kobj_type key_ktype = {
	.release = ieee80211_key_release,
	.sysfs_ops = &key_ktype_ops,
	.default_attrs = key_ktype_attrs,
};

static ssize_t sta_sysfs_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	struct sta_attribute *sta_attr;
	struct sta_info *sta;

	sta_attr = container_of(attr, struct sta_attribute, attr);
	sta = container_of(kobj, struct sta_info, kobj);
	return sta_attr->show(sta, buf);
}

static ssize_t key_sysfs_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	struct key_attribute *key_attr;
	struct ieee80211_key *key;

	key_attr = container_of(attr, struct key_attribute, attr);
	key = container_of(kobj, struct ieee80211_key, kobj);
	return key_attr->show(key, buf);
}

int ieee80211_sta_kset_sysfs_register(struct ieee80211_local *local)
{
	int res;

	res = kobject_set_name(&local->sta_kset.kobj, "sta");
	if (res)
		return res;
	local->sta_kset.kobj.parent = &local->class_dev.kobj;
	local->sta_kset.ktype = &sta_ktype;
	return kset_register(&local->sta_kset);
}

void ieee80211_sta_kset_sysfs_unregister(struct ieee80211_local *local)
{
	kset_unregister(&local->sta_kset);
}

int ieee80211_key_kset_sysfs_register(struct ieee80211_sub_if_data *sdata)
{
	int res;

	res = kobject_set_name(&sdata->key_kset.kobj, "keys");
	if (res)
		return res;
	sdata->key_kset.kobj.parent = &sdata->dev->class_dev.kobj;
	sdata->key_kset.ktype = &key_ktype;
	return kset_register(&sdata->key_kset);
}

void ieee80211_key_kset_sysfs_unregister(struct ieee80211_sub_if_data *sdata)
{
	kset_unregister(&sdata->key_kset);
}

int ieee80211_sta_sysfs_add(struct sta_info *sta)
{
	return kobject_add(&sta->kobj);
}

void ieee80211_sta_sysfs_remove(struct sta_info *sta)
{
	kobject_del(&sta->kobj);
}

void ieee80211_key_sysfs_set_kset(struct ieee80211_key *key, struct kset *kset)
{
	key->kobj.kset = kset;
	if (!kset)
		key->kobj.ktype = &key_ktype;
}

int ieee80211_key_sysfs_add(struct ieee80211_key *key)
{
	return kobject_add(&key->kobj);
}

void ieee80211_key_sysfs_remove(struct ieee80211_key *key)
{
	if (key)
		kobject_del(&key->kobj);
}

int ieee80211_key_sysfs_add_default(struct ieee80211_sub_if_data *sdata)
{
	return sysfs_create_link(&sdata->key_kset.kobj,
				 &sdata->default_key->kobj, "default");
}

void ieee80211_key_sysfs_remove_default(struct ieee80211_sub_if_data *sdata)
{
	sysfs_remove_link(&sdata->key_kset.kobj, "default");
}
