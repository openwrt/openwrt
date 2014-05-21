/*
 * iwinfo - Wireless Information Library - Broadcom wl.o Headers
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

#ifndef __IWINFO_WL_H_
#define __IWINFO_WL_H_

#include <fcntl.h>

#include "iwinfo.h"
#include "iwinfo/utils.h"
#include "iwinfo/api/broadcom.h"

int wl_probe(const char *ifname);
int wl_get_mode(const char *ifname, int *buf);
int wl_get_ssid(const char *ifname, char *buf);
int wl_get_bssid(const char *ifname, char *buf);
int wl_get_country(const char *ifname, char *buf);
int wl_get_channel(const char *ifname, int *buf);
int wl_get_frequency(const char *ifname, int *buf);
int wl_get_frequency_offset(const char *ifname, int *buf);
int wl_get_txpower(const char *ifname, int *buf);
int wl_get_txpower_offset(const char *ifname, int *buf);
int wl_get_bitrate(const char *ifname, int *buf);
int wl_get_signal(const char *ifname, int *buf);
int wl_get_noise(const char *ifname, int *buf);
int wl_get_quality(const char *ifname, int *buf);
int wl_get_quality_max(const char *ifname, int *buf);
int wl_get_enctype(const char *ifname, char *buf);
int wl_get_encryption(const char *ifname, char *buf);
int wl_get_phyname(const char *ifname, char *buf);
int wl_get_assoclist(const char *ifname, char *buf, int *len);
int wl_get_txpwrlist(const char *ifname, char *buf, int *len);
int wl_get_scanlist(const char *ifname, char *buf, int *len);
int wl_get_freqlist(const char *ifname, char *buf, int *len);
int wl_get_countrylist(const char *ifname, char *buf, int *len);
int wl_get_hwmodelist(const char *ifname, int *buf);
int wl_get_mbssid_support(const char *ifname, int *buf);
int wl_get_hardware_id(const char *ifname, char *buf);
int wl_get_hardware_name(const char *ifname, char *buf);
void wl_close(void);

static const struct iwinfo_ops wl_ops = {
	.name             = "wl",
	.probe            = wl_probe,
	.channel          = wl_get_channel,
	.frequency        = wl_get_frequency,
	.frequency_offset = wl_get_frequency_offset,
	.txpower          = wl_get_txpower,
	.txpower_offset   = wl_get_txpower_offset,
	.bitrate          = wl_get_bitrate,
	.signal           = wl_get_signal,
	.noise            = wl_get_noise,
	.quality          = wl_get_quality,
	.quality_max      = wl_get_quality_max,
	.mbssid_support   = wl_get_mbssid_support,
	.hwmodelist       = wl_get_hwmodelist,
	.mode             = wl_get_mode,
	.ssid             = wl_get_ssid,
	.bssid            = wl_get_bssid,
	.country          = wl_get_country,
	.hardware_id      = wl_get_hardware_id,
	.hardware_name    = wl_get_hardware_name,
	.encryption       = wl_get_encryption,
	.phyname          = wl_get_phyname,
	.assoclist        = wl_get_assoclist,
	.txpwrlist        = wl_get_txpwrlist,
	.scanlist         = wl_get_scanlist,
	.freqlist         = wl_get_freqlist,
	.countrylist      = wl_get_countrylist,
	.close            = wl_close
};

#endif
