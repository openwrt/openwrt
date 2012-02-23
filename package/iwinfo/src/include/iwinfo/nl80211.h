/*
 * iwinfo - Wireless Information Library - NL80211 Headers
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 */

#ifndef __IWINFO_NL80211_H_
#define __IWINFO_NL80211_H_

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/un.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "iwinfo.h"
#include "iwinfo/utils.h"
#include "iwinfo/api/nl80211.h"

struct nl80211_state {
	struct nl_sock *nl_sock;
	struct nl_cache *nl_cache;
	struct genl_family *nl80211;
	struct genl_family *nlctrl;
};

struct nl80211_msg_conveyor {
	struct nl_msg *msg;
	struct nl_cb *cb;
};

struct nl80211_event_conveyor {
	int wait;
	int recv;
};

struct nl80211_group_conveyor {
	const char *name;
	int id;
};

struct nl80211_rssi_rate {
	int16_t rate;
	int8_t  rssi;
};

struct nl80211_array_buf {
	void *buf;
	int count;
};

int nl80211_probe(const char *ifname);
int nl80211_get_mode(const char *ifname, char *buf);
int nl80211_get_ssid(const char *ifname, char *buf);
int nl80211_get_bssid(const char *ifname, char *buf);
int nl80211_get_country(const char *ifname, char *buf);
int nl80211_get_channel(const char *ifname, int *buf);
int nl80211_get_frequency(const char *ifname, int *buf);
int nl80211_get_frequency_offset(const char *ifname, int *buf);
int nl80211_get_txpower(const char *ifname, int *buf);
int nl80211_get_txpower_offset(const char *ifname, int *buf);
int nl80211_get_bitrate(const char *ifname, int *buf);
int nl80211_get_signal(const char *ifname, int *buf);
int nl80211_get_noise(const char *ifname, int *buf);
int nl80211_get_quality(const char *ifname, int *buf);
int nl80211_get_quality_max(const char *ifname, int *buf);
int nl80211_get_encryption(const char *ifname, char *buf);
int nl80211_get_assoclist(const char *ifname, char *buf, int *len);
int nl80211_get_txpwrlist(const char *ifname, char *buf, int *len);
int nl80211_get_scanlist(const char *ifname, char *buf, int *len);
int nl80211_get_freqlist(const char *ifname, char *buf, int *len);
int nl80211_get_countrylist(const char *ifname, char *buf, int *len);
int nl80211_get_hwmodelist(const char *ifname, int *buf);
int nl80211_get_mbssid_support(const char *ifname, int *buf);
int nl80211_get_hardware_id(const char *ifname, char *buf);
int nl80211_get_hardware_name(const char *ifname, char *buf);
void nl80211_close(void);

static const struct iwinfo_ops nl80211_ops = {
	.channel          = nl80211_get_channel,
	.frequency        = nl80211_get_frequency,
	.frequency_offset = nl80211_get_frequency_offset,
	.txpower          = nl80211_get_txpower,
	.txpower_offset   = nl80211_get_txpower_offset,
	.bitrate          = nl80211_get_bitrate,
	.signal           = nl80211_get_signal,
	.noise            = nl80211_get_noise,
	.quality          = nl80211_get_quality,
	.quality_max      = nl80211_get_quality_max,
	.mbssid_support   = nl80211_get_mbssid_support,
	.hwmodelist       = nl80211_get_hwmodelist,
	.mode             = nl80211_get_mode,
	.ssid             = nl80211_get_ssid,
	.bssid            = nl80211_get_bssid,
	.country          = nl80211_get_country,
	.hardware_id      = nl80211_get_hardware_id,
	.hardware_name    = nl80211_get_hardware_name,
	.encryption       = nl80211_get_encryption,
	.assoclist        = nl80211_get_assoclist,
	.txpwrlist        = nl80211_get_txpwrlist,
	.scanlist         = nl80211_get_scanlist,
	.freqlist         = nl80211_get_freqlist,
	.countrylist      = nl80211_get_countrylist,
	.close            = nl80211_close
};

#endif
