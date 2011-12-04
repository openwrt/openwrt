/*
 * iwinfo - Wireless Information Library - Linux Wireless Extension Headers
 *
 *   Copyright (C) 2009-2010 Jo-Philipp Wich <xm@subsignal.org>
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

#ifndef __IWINFO_WEXT_H_
#define __IWINFO_WEXT_H_

#include <fcntl.h>

#include "iwinfo.h"
#include "iwinfo/utils.h"
#include "iwinfo/api/wext.h"


int wext_probe(const char *ifname);
int wext_get_mode(const char *ifname, char *buf);
int wext_get_ssid(const char *ifname, char *buf);
int wext_get_bssid(const char *ifname, char *buf);
int wext_get_country(const char *ifname, char *buf);
int wext_get_channel(const char *ifname, int *buf);
int wext_get_frequency(const char *ifname, int *buf);
int wext_get_frequency_offset(const char *ifname, int *buf);
int wext_get_txpower(const char *ifname, int *buf);
int wext_get_txpower_offset(const char *ifname, int *buf);
int wext_get_bitrate(const char *ifname, int *buf);
int wext_get_signal(const char *ifname, int *buf);
int wext_get_noise(const char *ifname, int *buf);
int wext_get_quality(const char *ifname, int *buf);
int wext_get_quality_max(const char *ifname, int *buf);
int wext_get_encryption(const char *ifname, char *buf);
int wext_get_assoclist(const char *ifname, char *buf, int *len);
int wext_get_txpwrlist(const char *ifname, char *buf, int *len);
int wext_get_scanlist(const char *ifname, char *buf, int *len);
int wext_get_freqlist(const char *ifname, char *buf, int *len);
int wext_get_countrylist(const char *ifname, char *buf, int *len);
int wext_get_hwmodelist(const char *ifname, int *buf);
int wext_get_mbssid_support(const char *ifname, int *buf);
int wext_get_hardware_id(const char *ifname, char *buf);
int wext_get_hardware_name(const char *ifname, char *buf);
void wext_close(void);

static const struct iwinfo_ops wext_ops = {
	.channel          = wext_get_channel,
	.frequency        = wext_get_frequency,
	.frequency_offset = wext_get_frequency_offset,
	.txpower          = wext_get_txpower,
	.txpower_offset   = wext_get_txpower_offset,
	.bitrate          = wext_get_bitrate,
	.signal           = wext_get_signal,
	.noise            = wext_get_noise,
	.quality          = wext_get_quality,
	.quality_max      = wext_get_quality_max,
	.mbssid_support   = wext_get_mbssid_support,
	.hwmodelist       = wext_get_hwmodelist,
	.mode             = wext_get_mode,
	.ssid             = wext_get_ssid,
	.bssid            = wext_get_bssid,
	.country          = wext_get_country,
	.hardware_id      = wext_get_hardware_id,
	.hardware_name    = wext_get_hardware_name,
	.encryption       = wext_get_encryption,
	.assoclist        = wext_get_assoclist,
	.txpwrlist        = wext_get_txpwrlist,
	.scanlist         = wext_get_scanlist,
	.freqlist         = wext_get_freqlist,
	.countrylist      = wext_get_countrylist,
	.close            = wext_close
};

#endif
