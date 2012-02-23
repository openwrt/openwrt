/*
 * iwinfo - Wireless Information Library - Linux Wireless Extension Backend
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
 * Parts of this code are derived from the Linux wireless tools, iwlib.c,
 * iwlist.c and iwconfig.c in particular.
 */

#include "iwinfo.h"
#include "iwinfo/wext.h"

static double wext_freq2float(const struct iw_freq *in)
{
	int		i;
	double	res = (double) in->m;
	for(i = 0; i < in->e; i++) res *= 10;
	return res;
}

static inline int wext_freq2mhz(const struct iw_freq *in)
{
	int i;

	if( in->e == 6 )
	{
		return in->m;
	}
	else
	{
		return (int)(wext_freq2float(in) / 1000000);
	}
}

static inline int wext_ioctl(const char *ifname, int cmd, struct iwreq *wrq)
{
	if( !strncmp(ifname, "mon.", 4) )
		strncpy(wrq->ifr_name, &ifname[4], IFNAMSIZ);
	else
		strncpy(wrq->ifr_name, ifname, IFNAMSIZ);

	return iwinfo_ioctl(cmd, wrq);
}


int wext_probe(const char *ifname)
{
	struct iwreq wrq;

	if(wext_ioctl(ifname, SIOCGIWNAME, &wrq) >= 0)
		return 1;

	return 0;
}

void wext_close(void)
{
	/* Nop */
}

int wext_get_mode(const char *ifname, int *buf)
{
	struct iwreq wrq;

	if(wext_ioctl(ifname, SIOCGIWMODE, &wrq) >= 0)
	{
		switch(wrq.u.mode)
		{
			case 1:
				*buf = IWINFO_OPMODE_ADHOC;
				break;

			case 2:
				*buf = IWINFO_OPMODE_CLIENT;
				break;

			case 3:
				*buf = IWINFO_OPMODE_MASTER;
				break;

			case 6:
				*buf = IWINFO_OPMODE_MONITOR;
				break;

			default:
				*buf = IWINFO_OPMODE_UNKNOWN;
				break;
		}

		return 0;
	}

	return -1;
}

int wext_get_ssid(const char *ifname, char *buf)
{
	struct iwreq wrq;

	wrq.u.essid.pointer = (caddr_t) buf;
	wrq.u.essid.length  = IW_ESSID_MAX_SIZE + 1;
	wrq.u.essid.flags   = 0;

	if(wext_ioctl(ifname, SIOCGIWESSID, &wrq) >= 0)
		return 0;

	return -1;
}

int wext_get_bssid(const char *ifname, char *buf)
{
	struct iwreq wrq;

	if(wext_ioctl(ifname, SIOCGIWAP, &wrq) >= 0)
	{
		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			(uint8_t)wrq.u.ap_addr.sa_data[0], (uint8_t)wrq.u.ap_addr.sa_data[1],
			(uint8_t)wrq.u.ap_addr.sa_data[2], (uint8_t)wrq.u.ap_addr.sa_data[3],
			(uint8_t)wrq.u.ap_addr.sa_data[4], (uint8_t)wrq.u.ap_addr.sa_data[5]);

		return 0;
	}

	return -1;
}

int wext_get_bitrate(const char *ifname, int *buf)
{
	struct iwreq wrq;

	if(wext_ioctl(ifname, SIOCGIWRATE, &wrq) >= 0)
	{
		*buf = (wrq.u.bitrate.value / 1000);
		return 0;
	}

	return -1;
}

int wext_get_channel(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_range range;
	double freq;
	int i;

	if(wext_ioctl(ifname, SIOCGIWFREQ, &wrq) >= 0)
	{
		if( wrq.u.freq.m >= 1000 )
		{
			freq = wext_freq2float(&wrq.u.freq);
			wrq.u.data.pointer = (caddr_t) &range;
			wrq.u.data.length  = sizeof(struct iw_range);
			wrq.u.data.flags   = 0;

			if(wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0)
			{
				for(i = 0; i < range.num_frequency; i++)
				{
					if( wext_freq2float(&range.freq[i]) == freq )
					{
						*buf = range.freq[i].i;
						return 0;
					}
				}
			}
		}
		else
		{
			*buf = wrq.u.freq.m;
			return 0;
		}
	}

	return -1;
}

int wext_get_frequency(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_range range;
	int i, channel;

	if(wext_ioctl(ifname, SIOCGIWFREQ, &wrq) >= 0)
	{
		/* We got a channel number instead ... */
		if( wrq.u.freq.m < 1000 )
		{
			channel = wrq.u.freq.m;
			wrq.u.data.pointer = (caddr_t) &range;
			wrq.u.data.length  = sizeof(struct iw_range);
			wrq.u.data.flags   = 0;

			if(wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0)
			{
				for(i = 0; i < range.num_frequency; i++)
				{
					if( range.freq[i].i == channel )
					{
						*buf = wext_freq2mhz(&range.freq[i]);
						return 0;
					}
				}
			}
		}
		else
		{
			*buf = wext_freq2mhz(&wrq.u.freq);
			return 0;
		}
	}

	return -1;
}

int wext_get_txpower(const char *ifname, int *buf)
{
	struct iwreq wrq;

	wrq.u.txpower.flags = 0;

	if(wext_ioctl(ifname, SIOCGIWTXPOW, &wrq) >= 0)
	{
		if(wrq.u.txpower.flags & IW_TXPOW_MWATT)
			*buf = iwinfo_mw2dbm(wrq.u.txpower.value);
		else
			*buf = wrq.u.txpower.value;

		return 0;
	}

	return -1;
}

int wext_get_signal(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_statistics stats;

	wrq.u.data.pointer = (caddr_t) &stats;
	wrq.u.data.length  = sizeof(struct iw_statistics);
	wrq.u.data.flags   = 1;

	if(wext_ioctl(ifname, SIOCGIWSTATS, &wrq) >= 0)
	{
		*buf = (stats.qual.updated & IW_QUAL_DBM)
			? (stats.qual.level - 0x100) : stats.qual.level;

		return 0;
	}

	return -1;
}

int wext_get_noise(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_statistics stats;

	wrq.u.data.pointer = (caddr_t) &stats;
	wrq.u.data.length  = sizeof(struct iw_statistics);
	wrq.u.data.flags   = 1;

	if(wext_ioctl(ifname, SIOCGIWSTATS, &wrq) >= 0)
	{
		*buf = (stats.qual.updated & IW_QUAL_DBM)
			? (stats.qual.noise - 0x100) : stats.qual.noise;

		return 0;
	}

	return -1;
}

int wext_get_quality(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_statistics stats;

	wrq.u.data.pointer = (caddr_t) &stats;
	wrq.u.data.length  = sizeof(struct iw_statistics);
	wrq.u.data.flags   = 1;

	if(wext_ioctl(ifname, SIOCGIWSTATS, &wrq) >= 0)
	{
		*buf = stats.qual.qual;
		return 0;
	}

	return -1;
}

int wext_get_quality_max(const char *ifname, int *buf)
{
	struct iwreq wrq;
	struct iw_range range;

	wrq.u.data.pointer = (caddr_t) &range;
	wrq.u.data.length  = sizeof(struct iw_range);
	wrq.u.data.flags   = 0;

	if(wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0)
	{
		*buf = range.max_qual.qual;
		return 0;
	}

	return -1;
}

int wext_get_assoclist(const char *ifname, char *buf, int *len)
{
	/* Stub */
	return -1;
}

int wext_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	struct iwreq wrq;
	struct iw_range range;
	struct iwinfo_txpwrlist_entry entry;
	int i;

	wrq.u.data.pointer = (caddr_t) &range;
	wrq.u.data.length  = sizeof(struct iw_range);
	wrq.u.data.flags   = 0;

	if( (wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0) &&
	    (range.num_txpower > 0) && (range.num_txpower <= IW_MAX_TXPOWER) &&
	    !(range.txpower_capa & IW_TXPOW_RELATIVE)
	) {
		for( i = 0; i < range.num_txpower; i++ )
		{
			if( range.txpower_capa & IW_TXPOW_MWATT )
			{
				entry.dbm = iwinfo_mw2dbm(range.txpower[i]);
				entry.mw  = range.txpower[i];
			}

			/* Madwifi does neither set mW not dBm caps, also iwlist assumes
			 * dBm if mW is not set, so don't check here... */
			else /* if( range.txpower_capa & IW_TXPOW_DBM ) */
			{
				entry.dbm = range.txpower[i];
				entry.mw  = iwinfo_dbm2mw(range.txpower[i]);
			}

			memcpy(&buf[i*sizeof(entry)], &entry, sizeof(entry));
		}

		*len = i * sizeof(entry);
		return 0;
	}

	return -1;
}

int wext_get_freqlist(const char *ifname, char *buf, int *len)
{
	struct iwreq wrq;
	struct iw_range range;
	struct iwinfo_freqlist_entry entry;
	int i, bl;

	wrq.u.data.pointer = (caddr_t) &range;
	wrq.u.data.length  = sizeof(struct iw_range);
	wrq.u.data.flags   = 0;

	if(wext_ioctl(ifname, SIOCGIWRANGE, &wrq) >= 0)
	{
		bl = 0;

		for(i = 0; i < range.num_frequency; i++)
		{
			entry.mhz        = wext_freq2mhz(&range.freq[i]);
			entry.channel    = range.freq[i].i;
			entry.restricted = 0;

			memcpy(&buf[bl], &entry, sizeof(struct iwinfo_freqlist_entry));
			bl += sizeof(struct iwinfo_freqlist_entry);
		}

		*len = bl;
		return 0;
	}

	return -1;
}

int wext_get_country(const char *ifname, char *buf)
{
	sprintf(buf, "00");
	return 0;
}

int wext_get_countrylist(const char *ifname, char *buf, int *len)
{
	/* Stub */
	return -1;
}

int wext_get_hwmodelist(const char *ifname, int *buf)
{
	char chans[IWINFO_BUFSIZE] = { 0 };
	struct iwinfo_freqlist_entry *e = NULL;
	int len = 0;

	*buf = 0;

	if( !wext_get_freqlist(ifname, chans, &len) )
	{
		for( e = (struct iwinfo_freqlist_entry *)chans; e->channel; e++ )
		{
			if( e->channel <= 14 )
			{
				*buf |= IWINFO_80211_B;
				*buf |= IWINFO_80211_G;
			}
			else
			{
				*buf |= IWINFO_80211_A;
			}
		}

		return 0;
	}

	return -1;
}

int wext_get_encryption(const char *ifname, char *buf)
{
	/* No reliable crypto info in wext */
	return -1;
}

int wext_get_mbssid_support(const char *ifname, int *buf)
{
	/* No multi bssid support atm */
	return -1;
}

static char * wext_sysfs_ifname_file(const char *ifname, const char *path)
{
	FILE *f;
	static char buf[128];
	char *rv = NULL;

	snprintf(buf, sizeof(buf), "/sys/class/net/%s/%s", ifname, path);

	if ((f = fopen(buf, "r")) != NULL)
	{
		memset(buf, 0, sizeof(buf));

		if (fread(buf, 1, sizeof(buf), f))
			rv = buf;

		fclose(f);
	}

	return rv;
}

int wext_get_hardware_id(const char *ifname, char *buf)
{
	char *data;
	struct iwinfo_hardware_id *id = (struct iwinfo_hardware_id *)buf;

	memset(id, 0, sizeof(struct iwinfo_hardware_id));

	data = wext_sysfs_ifname_file(ifname, "device/vendor");
	if (data)
		id->vendor_id = strtoul(data, NULL, 16);

	data = wext_sysfs_ifname_file(ifname, "device/device");
	if (data)
		id->device_id = strtoul(data, NULL, 16);

	data = wext_sysfs_ifname_file(ifname, "device/subsystem_device");
	if (data)
		id->subsystem_device_id = strtoul(data, NULL, 16);

	data = wext_sysfs_ifname_file(ifname, "device/subsystem_vendor");
	if (data)
		id->subsystem_vendor_id = strtoul(data, NULL, 16);

	return (id->vendor_id > 0 && id->device_id > 0) ? 0 : -1;
}

int wext_get_hardware_name(const char *ifname, char *buf)
{
	sprintf(buf, "Generic WEXT");
	return 0;
}

int wext_get_txpower_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}

int wext_get_frequency_offset(const char *ifname, int *buf)
{
	/* Stub */
	*buf = 0;
	return -1;
}
