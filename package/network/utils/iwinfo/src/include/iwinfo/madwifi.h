/*
 * iwinfo - Wireless Information Library - Madwifi Headers
 *
 *   Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
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

#ifndef __IWINFO_MADWIFI_H_
#define __IWINFO_MADWIFI_H_

#include <fcntl.h>

#include "iwinfo.h"
#include "iwinfo/utils.h"
#include "iwinfo/api/madwifi.h"

int madwifi_probe(const char *ifname);
int madwifi_get_mode(const char *ifname, int *buf);
int madwifi_get_ssid(const char *ifname, char *buf);
int madwifi_get_bssid(const char *ifname, char *buf);
int madwifi_get_country(const char *ifname, char *buf);
int madwifi_get_channel(const char *ifname, int *buf);
int madwifi_get_frequency(const char *ifname, int *buf);
int madwifi_get_frequency_offset(const char *ifname, int *buf);
int madwifi_get_txpower(const char *ifname, int *buf);
int madwifi_get_txpower_offset(const char *ifname, int *buf);
int madwifi_get_bitrate(const char *ifname, int *buf);
int madwifi_get_signal(const char *ifname, int *buf);
int madwifi_get_noise(const char *ifname, int *buf);
int madwifi_get_quality(const char *ifname, int *buf);
int madwifi_get_quality_max(const char *ifname, int *buf);
int madwifi_get_encryption(const char *ifname, char *buf);
int madwifi_get_assoclist(const char *ifname, char *buf, int *len);
int madwifi_get_txpwrlist(const char *ifname, char *buf, int *len);
int madwifi_get_scanlist(const char *ifname, char *buf, int *len);
int madwifi_get_freqlist(const char *ifname, char *buf, int *len);
int madwifi_get_countrylist(const char *ifname, char *buf, int *len);
int madwifi_get_hwmodelist(const char *ifname, int *buf);
int madwifi_get_mbssid_support(const char *ifname, int *buf);
int madwifi_get_hardware_id(const char *ifname, char *buf);
int madwifi_get_hardware_name(const char *ifname, char *buf);
void madwifi_close(void);

static const struct iwinfo_ops madwifi_ops = {
	.channel          = madwifi_get_channel,
	.frequency        = madwifi_get_frequency,
	.frequency_offset = madwifi_get_frequency_offset,
	.txpower          = madwifi_get_txpower,
	.txpower_offset   = madwifi_get_txpower_offset,
	.bitrate          = madwifi_get_bitrate,
	.signal           = madwifi_get_signal,
	.noise            = madwifi_get_noise,
	.quality          = madwifi_get_quality,
	.quality_max      = madwifi_get_quality_max,
	.mbssid_support   = madwifi_get_mbssid_support,
	.hwmodelist       = madwifi_get_hwmodelist,
	.mode             = madwifi_get_mode,
	.ssid             = madwifi_get_ssid,
	.bssid            = madwifi_get_bssid,
	.country          = madwifi_get_country,
	.hardware_id      = madwifi_get_hardware_id,
	.hardware_name    = madwifi_get_hardware_name,
	.encryption       = madwifi_get_encryption,
	.assoclist        = madwifi_get_assoclist,
	.txpwrlist        = madwifi_get_txpwrlist,
	.scanlist         = madwifi_get_scanlist,
	.freqlist         = madwifi_get_freqlist,
	.countrylist      = madwifi_get_countrylist,
	.close            = madwifi_close
};

#endif
