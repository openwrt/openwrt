/*
 * iwinfo - Wireless Information Library - Broadcom wl.o Backend
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
 *
 * This code is based on the wlc.c utility published by OpenWrt.org .
 */

#include "iwinfo/wl.h"
#include "iwinfo/wext.h"

static int wl_ioctl(const char *name, int cmd, void *buf, int len)
{
	struct ifreq ifr;
	wl_ioctl_t ioc;

	/* do it */
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;

	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_data = (caddr_t) &ioc;

	return iwinfo_ioctl(SIOCDEVPRIVATE, &ifr);
}

static int wl_iovar(const char *name, const char *cmd, const char *arg,
					int arglen, void *buf, int buflen)
{
	int cmdlen = strlen(cmd) + 1;

	memcpy(buf, cmd, cmdlen);

	if (arg && arglen > 0)
		memcpy(buf + cmdlen, arg, arglen);

	return wl_ioctl(name, WLC_GET_VAR, buf, buflen);
}

static struct wl_maclist * wl_read_assoclist(const char *ifname)
{
	struct wl_maclist *macs;
	int maclen = 4 + WL_MAX_STA_COUNT * 6;

	if ((macs = (struct wl_maclist *) malloc(maclen)) != NULL)
	{
		memset(macs, 0, maclen);
		macs->count = WL_MAX_STA_COUNT;

		if (!wl_ioctl(ifname, WLC_GET_ASSOCLIST, macs, maclen))
			return macs;

		free(macs);
	}

	return NULL;
}


int wl_probe(const char *ifname)
{
	int magic;
	return (!wl_ioctl(ifname, WLC_GET_MAGIC, &magic, sizeof(magic)) &&
			(magic == WLC_IOCTL_MAGIC));
}

void wl_close(void)
{
	/* Nop */
}

int wl_get_mode(const char *ifname, int *buf)
{
	int ret = -1;
	int ap, infra, passive;

	if ((ret = wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap))))
		return ret;

	if ((ret = wl_ioctl(ifname, WLC_GET_INFRA, &infra, sizeof(infra))))
		return ret;

	if ((ret = wl_ioctl(ifname, WLC_GET_PASSIVE, &passive, sizeof(passive))))
		return ret;

	if (passive)
		*buf = IWINFO_OPMODE_MONITOR;
	else if (!infra)
		*buf = IWINFO_OPMODE_ADHOC;
	else if (ap)
		*buf = IWINFO_OPMODE_MASTER;
	else
		*buf = IWINFO_OPMODE_CLIENT;

	return 0;
}

int wl_get_ssid(const char *ifname, char *buf)
{
	int ret = -1;
	wlc_ssid_t ssid;

	if (!(ret = wl_ioctl(ifname, WLC_GET_SSID, &ssid, sizeof(ssid))))
		memcpy(buf, ssid.ssid, ssid.ssid_len);

	return ret;
}

int wl_get_bssid(const char *ifname, char *buf)
{
	int ret = -1;
	char bssid[6];

	if (!(ret = wl_ioctl(ifname, WLC_GET_BSSID, bssid, 6)))
		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			(uint8_t)bssid[0], (uint8_t)bssid[1], (uint8_t)bssid[2],
			(uint8_t)bssid[3], (uint8_t)bssid[4], (uint8_t)bssid[5]
		);

	return ret;
}

int wl_get_channel(const char *ifname, int *buf)
{
	return wl_ioctl(ifname, WLC_GET_CHANNEL, buf, sizeof(buf));
}

int wl_get_frequency(const char *ifname, int *buf)
{
	return wext_get_frequency(ifname, buf);
}

int wl_get_txpower(const char *ifname, int *buf)
{
	/* WLC_GET_VAR "qtxpower" */
	return wext_get_txpower(ifname, buf);
}

int wl_get_bitrate(const char *ifname, int *buf)
{
	int ret = -1;
	int rate = 0;

	if( !(ret = wl_ioctl(ifname, WLC_GET_RATE, &rate, sizeof(rate))) && (rate > 0))
		*buf = ((rate / 2) * 1000) + ((rate & 1) ? 500 : 0);

	return ret;
}

int wl_get_signal(const char *ifname, int *buf)
{
	unsigned int ap, rssi, i, rssi_count;
	int ioctl_req_version = 0x2000;
	char tmp[WLC_IOCTL_MAXLEN];
	struct wl_maclist *macs = NULL;
	wl_sta_rssi_t starssi;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN);

	if (!wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap)) && !ap)
	{
		*buf = tmp[WL_BSS_RSSI_OFFSET];
	}
	else
	{
		rssi = rssi_count = 0;

		/* Calculate average rssi from conntected stations */
		if ((macs = wl_read_assoclist(ifname)) != NULL)
		{
			for (i = 0; i < macs->count; i++)
			{
				memcpy(starssi.mac, &macs->ea[i], 6);

				if (!wl_ioctl(ifname, WLC_GET_RSSI, &starssi, 12))
				{
					rssi -= starssi.rssi;
					rssi_count++;
				}
			}

			free(macs);
		}

		*buf = (rssi == 0 || rssi_count == 0) ? 1 : -(rssi / rssi_count);
	}

	return 0;
}

int wl_get_noise(const char *ifname, int *buf)
{
	unsigned int ap, noise;
	int ioctl_req_version = 0x2000;
	char tmp[WLC_IOCTL_MAXLEN];

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN);

	if ((wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap)) < 0) || ap)
	{
		if (wl_ioctl(ifname, WLC_GET_PHY_NOISE, &noise, sizeof(noise)) < 0)
			noise = 0;
	}
	else
	{
		noise = tmp[WL_BSS_NOISE_OFFSET];
	}

	*buf = noise;

	return 0;
}

int wl_get_quality(const char *ifname, int *buf)
{
	return wext_get_quality(ifname, buf);
}

int wl_get_quality_max(const char *ifname, int *buf)
{
	return wext_get_quality_max(ifname, buf);
}

int wl_get_encryption(const char *ifname, char *buf)
{
	uint32_t wsec, wauth, wpa;
	struct iwinfo_crypto_entry *c = (struct iwinfo_crypto_entry *)buf;

	if( wl_ioctl(ifname, WLC_GET_WPA_AUTH, &wpa,   sizeof(uint32_t)) ||
	    wl_ioctl(ifname, WLC_GET_WSEC,     &wsec,  sizeof(uint32_t)) ||
		wl_ioctl(ifname, WLC_GET_AUTH,     &wauth, sizeof(uint32_t)) )
			return -1;

	switch(wsec)
	{
		case 2:
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;
			break;

		case 4:
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;
			break;

		case 6:
			c->pair_ciphers |= IWINFO_CIPHER_TKIP;
			c->pair_ciphers |= IWINFO_CIPHER_CCMP;
			break;
	}

	switch(wpa)
	{
		case 0:
			if (wsec && !wauth)
				c->auth_algs |= IWINFO_AUTH_OPEN;

			else if (wsec && wauth)
				c->auth_algs |= IWINFO_AUTH_SHARED;

			/* ToDo: evaluate WEP key lengths */
			c->pair_ciphers = IWINFO_CIPHER_WEP40 | IWINFO_CIPHER_WEP104;
			c->auth_suites |= IWINFO_KMGMT_NONE;
			break;

		case 2:
			c->wpa_version = 1;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 4:
			c->wpa_version = 1;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		case 32:
		case 64:
			c->wpa_version = 2;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 66:
			c->wpa_version = 3;
			c->auth_suites |= IWINFO_KMGMT_8021x;
			break;

		case 128:
			c->wpa_version = 2;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		case 132:
			c->wpa_version = 3;
			c->auth_suites |= IWINFO_KMGMT_PSK;
			break;

		default:
			break;
	}

	c->enabled = (c->wpa_version || c->auth_algs) ? 1 : 0;
	c->group_ciphers = c->pair_ciphers;

	return 0;
}

int wl_get_enctype(const char *ifname, char *buf)
{
	uint32_t wsec, wpa;
	char algo[11];

	if( wl_ioctl(ifname, WLC_GET_WPA_AUTH, &wpa, sizeof(uint32_t)) ||
	    wl_ioctl(ifname, WLC_GET_WSEC, &wsec, sizeof(uint32_t)) )
			return -1;

	switch(wsec)
	{
		case 2:
			sprintf(algo, "TKIP");
			break;

		case 4:
			sprintf(algo, "CCMP");
			break;

		case 6:
			sprintf(algo, "TKIP, CCMP");
			break;
	}

	switch(wpa)
	{
		case 0:
			sprintf(buf, "%s", wsec ? "WEP" : "None");
			break;

		case 2:
			sprintf(buf, "WPA 802.1X (%s)", algo);
			break;

		case 4:
			sprintf(buf, "WPA PSK (%s)", algo);
			break;

		case 32:
			sprintf(buf, "802.1X (%s)", algo);
			break;

		case 64:
			sprintf(buf, "WPA2 802.1X (%s)", algo);
			break;

		case 66:
			sprintf(buf, "mixed WPA/WPA2 802.1X (%s)", algo);
			break;

		case 128:
			sprintf(buf, "WPA2 PSK (%s)", algo);
			break;

		case 132:
			sprintf(buf, "mixed WPA/WPA2 PSK (%s)", algo);
			break;

		default:
			sprintf(buf, "Unknown");
	}

	return 0;
}

static void wl_get_assoclist_cb(const char *ifname,
							    struct iwinfo_assoclist_entry *e)
{
	wl_sta_info_t sta = { 0 };

	if (!wl_iovar(ifname, "sta_info", e->mac, 6, &sta, sizeof(sta)) &&
		(sta.ver >= 2))
	{
		e->inactive     = sta.idle * 1000;
		e->rx_packets   = sta.rx_ucast_pkts;
		e->tx_packets   = sta.tx_pkts;
		e->rx_rate.rate = sta.rx_rate;
		e->tx_rate.rate = sta.tx_rate;

		/* ToDo: 11n */
		e.rx_rate.mcs = -1;
		e.tx_rate.mcs = -1;
	}
}

int wl_get_assoclist(const char *ifname, char *buf, int *len)
{
	int i, j, noise;
	int ap, infra, passive;
	char line[128];
	char macstr[18];
	char devstr[IFNAMSIZ];
	struct wl_maclist *macs;
	struct wl_sta_rssi rssi;
	struct iwinfo_assoclist_entry entry;
	FILE *arp;

	ap = infra = passive = 0;

	wl_ioctl(ifname, WLC_GET_AP, &ap, sizeof(ap));
	wl_ioctl(ifname, WLC_GET_INFRA, &infra, sizeof(infra));
	wl_ioctl(ifname, WLC_GET_PASSIVE, &passive, sizeof(passive));

	if (wl_get_noise(ifname, &noise))
		noise = 0;

	if ((ap || infra || passive) && ((macs = wl_read_assoclist(ifname)) != NULL))
	{
		for (i = 0, j = 0; i < macs->count; i++, j += sizeof(struct iwinfo_assoclist_entry))
		{
			memset(&entry, 0, sizeof(entry));
			memcpy(rssi.mac, &macs->ea[i], 6);

			if (!wl_ioctl(ifname, WLC_GET_RSSI, &rssi, sizeof(struct wl_sta_rssi)))
				entry.signal = (rssi.rssi - 0x100);
			else
				entry.signal = 0;

			entry.noise = noise;
			memcpy(entry.mac, &macs->ea[i], 6);
			wl_get_assoclist_cb(ifname, &entry);

			memcpy(&buf[j], &entry, sizeof(entry));
		}

		*len = j;
		free(macs);
		return 0;
	}
	else if ((arp = fopen("/proc/net/arp", "r")) != NULL)
	{
		j = 0;

		while (fgets(line, sizeof(line), arp) != NULL)
		{
			if (sscanf(line, "%*s 0x%*d 0x%*d %17s %*s %s", macstr, devstr) && !strcmp(devstr, ifname))
			{
				rssi.mac[0] = strtol(&macstr[0],  NULL, 16);
				rssi.mac[1] = strtol(&macstr[3],  NULL, 16);
				rssi.mac[2] = strtol(&macstr[6],  NULL, 16);
				rssi.mac[3] = strtol(&macstr[9],  NULL, 16);
				rssi.mac[4] = strtol(&macstr[12], NULL, 16);
				rssi.mac[5] = strtol(&macstr[15], NULL, 16);

				if (!wl_ioctl(ifname, WLC_GET_RSSI, &rssi, sizeof(struct wl_sta_rssi)))
					entry.signal = (rssi.rssi - 0x100);
				else
					entry.signal = 0;

				entry.noise = noise;
				memcpy(entry.mac, rssi.mac, 6);
				memcpy(&buf[j], &entry, sizeof(entry));

				j += sizeof(entry);
			}
		}

		*len = j;
		(void) fclose(arp);
		return 0;
	}

	return -1;
}

int wl_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	struct iwinfo_txpwrlist_entry entry;
	uint8_t dbm[11] = { 0, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24 };
	uint8_t mw[11]  = { 1, 3, 6, 10, 15, 25, 39, 63, 100, 158, 251 };
	int i;

	for (i = 0; i < 11; i++)
	{
		entry.dbm = dbm[i];
		entry.mw  = mw[i];
		memcpy(&buf[i*sizeof(entry)], &entry, sizeof(entry));
	}

	*len = 11 * sizeof(entry);
	return 0;
}

int wl_get_scanlist(const char *ifname, char *buf, int *len)
{
	return wext_get_scanlist(ifname, buf, len);
}

int wl_get_freqlist(const char *ifname, char *buf, int *len)
{
	return wext_get_freqlist(ifname, buf, len);
}

int wl_get_country(const char *ifname, char *buf)
{
	char ccode[WLC_CNTRY_BUF_SZ];

	if (!wl_ioctl(ifname, WLC_GET_COUNTRY, ccode, WLC_CNTRY_BUF_SZ))
	{
		/* IL0 -> World */
		if (!strcmp(ccode, "IL0"))
			sprintf(buf, "00");

		/* YU -> RS */
		else if (!strcmp(ccode, "YU"))
			sprintf(buf, "RS");

		else
			memcpy(buf, ccode, 2);

		return 0;
	}

	return -1;
}

int wl_get_countrylist(const char *ifname, char *buf, int *len)
{
	int i, count;
	char cdata[WLC_IOCTL_MAXLEN];
	struct iwinfo_country_entry *c = (struct iwinfo_country_entry *)buf;
	wl_country_list_t *cl = (wl_country_list_t *)cdata;

	cl->buflen = sizeof(cdata);

	if (!wl_ioctl(ifname, WLC_GET_COUNTRY_LIST, cl, cl->buflen))
	{
		for (i = 0, count = 0; i < cl->count; i++, c++)
		{
			sprintf(c->ccode, &cl->country_abbrev[i * WLC_CNTRY_BUF_SZ]);
			c->iso3166 = c->ccode[0] * 256 + c->ccode[1];

			/* IL0 -> World */
			if (!strcmp(c->ccode, "IL0"))
				c->iso3166 = 0x3030;

			/* YU -> RS */
			else if (!strcmp(c->ccode, "YU"))
				c->iso3166 = 0x5253;
		}

		*len = (i * sizeof(struct iwinfo_country_entry));
		return 0;
	}

	return -1;
}

int wl_get_hwmodelist(const char *ifname, int *buf)
{
	return wext_get_hwmodelist(ifname, buf);
}

int wl_get_mbssid_support(const char *ifname, int *buf)
{
	wlc_rev_info_t revinfo;

	/* Multi bssid support only works on corerev >= 9 */
	if (!wl_ioctl(ifname, WLC_GET_REVINFO, &revinfo, sizeof(revinfo)))
	{
		if (revinfo.corerev >= 9)
		{
			*buf = 1;
			return 0;
		}
	}

	return -1;
}

int wl_get_hardware_id(const char *ifname, char *buf)
{
	wlc_rev_info_t revinfo;
	struct iwinfo_hardware_id *ids = (struct iwinfo_hardware_id *)buf;

	if (wl_ioctl(ifname, WLC_GET_REVINFO, &revinfo, sizeof(revinfo)))
		return -1;

	ids->vendor_id = revinfo.vendorid;
	ids->device_id = revinfo.deviceid;
	ids->subsystem_vendor_id = revinfo.boardvendor;
	ids->subsystem_device_id = revinfo.boardid;

	return 0;
}

int wl_get_hardware_name(const char *ifname, char *buf)
{
	struct iwinfo_hardware_id ids;

	if (wl_get_hardware_id(ifname, (char *)&ids))
		return -1;

	sprintf(buf, "Broadcom BCM%04X", ids.device_id);

	return 0;
}

int wl_get_txpower_offset(const char *ifname, int *buf)
{
	FILE *p;
	char off[8];

	*buf = 0;

	if ((p = popen("/usr/sbin/nvram get opo", "r")) != NULL)
	{
		if (fread(off, 1, sizeof(off), p))
			*buf = strtoul(off, NULL, 16);

		pclose(p);
	}

	return 0;
}

int wl_get_frequency_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}
