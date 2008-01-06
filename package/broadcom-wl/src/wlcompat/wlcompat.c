/*
 * wlcompat.c
 *
 * Copyright (C) 2005      Mike Baker
 * Copyright (C) 2005-2007 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id$
 */


#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <net/iw_handler.h>
#include <asm/uaccess.h>

#include <typedefs.h>
#include <bcmutils.h>
#include <wlioctl.h>

char buf[WLC_IOCTL_MAXLEN];
static struct net_device *dev;
#ifndef DEBUG
static int random = 1;
#endif
#ifndef WL_WEXT
static struct iw_statistics wstats;
static int last_mode = -1;
static int scan_cur = 0;

/* The frequency of each channel in MHz */
const long channel_frequency[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};
#define NUM_CHANNELS ( sizeof(channel_frequency) / sizeof(channel_frequency[0]) )
#endif

#define SCAN_RETRY_MAX	5
#define RNG_POLL_FREQ	1

typedef struct internal_wsec_key {
	uint8 index;		// 0x00
	uint8 unknown_1;	// 0x01
	uint8 type;		// 0x02
	uint8 unknown_2[7];	// 0x03
	uint8 len;		// 0x0a
	uint8 pad[3];
	char data[32];		// 0x0e
} wkey;


#ifdef DEBUG
void print_buffer(int len, unsigned char *buf);
#endif

static int wl_ioctl(struct net_device *dev, int cmd, void *buf, int len)
{
	mm_segment_t old_fs = get_fs();
	struct ifreq ifr;
	int ret;
	wl_ioctl_t ioc;
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	strncpy(ifr.ifr_name, dev->name, IFNAMSIZ);
	ifr.ifr_data = (caddr_t) &ioc;
	set_fs(KERNEL_DS);
	ret = dev->do_ioctl(dev,&ifr,SIOCDEVPRIVATE);
	set_fs (old_fs);
	return ret;
}

#if !defined(DEBUG) || !defined(WL_WEXT)
static int
wl_iovar_getbuf(struct net_device *dev, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	err = wl_ioctl(dev, WLC_GET_VAR, bufptr, buflen);

	return (err);
}

static int
wl_iovar_setbuf(struct net_device *dev, char *iovar, void *param, int paramlen, void *bufptr, int buflen)
{
	uint namelen;
	uint iolen;

	namelen = strlen(iovar) + 1;	 /* length of iovar name plus null */
	iolen = namelen + paramlen;

	/* check for overflow */
	if (iolen > buflen)
		return (BCME_BUFTOOSHORT);

	memcpy(bufptr, iovar, namelen);	/* copy iovar name including null */
	memcpy((int8*)bufptr + namelen, param, paramlen);

	return wl_ioctl(dev, WLC_SET_VAR, bufptr, iolen);
}

static int
wl_iovar_set(struct net_device *dev, char *iovar, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return wl_iovar_setbuf(dev, iovar, param, paramlen, smbuf, sizeof(smbuf));
}

static int
wl_iovar_get(struct net_device *dev, char *iovar, void *bufptr, int buflen)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int ret;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (buflen > sizeof(smbuf)) {
		ret = wl_iovar_getbuf(dev, iovar, NULL, 0, bufptr, buflen);
	} else {
		ret = wl_iovar_getbuf(dev, iovar, NULL, 0, smbuf, sizeof(smbuf));
		if (ret == 0)
			memcpy(bufptr, smbuf, buflen);
	}

	return ret;
}

#ifdef notyet
/*
 * format a bsscfg indexed iovar buffer
 */
static int
wl_bssiovar_mkbuf(char *iovar, int bssidx, void *param, int paramlen, void *bufptr, int buflen,
                  int *plen)
{
	char *prefix = "bsscfg:";
	int8* p;
	uint prefixlen;
	uint namelen;
	uint iolen;

	prefixlen = strlen(prefix);	/* length of bsscfg prefix */
	namelen = strlen(iovar) + 1;	/* length of iovar name + null */
	iolen = prefixlen + namelen + sizeof(int) + paramlen;

	/* check for overflow */
	if (buflen < 0 || iolen > (uint)buflen) {
		*plen = 0;
		return BCME_BUFTOOSHORT;
	}

	p = (int8*)bufptr;

	/* copy prefix, no null */
	memcpy(p, prefix, prefixlen);
	p += prefixlen;

	/* copy iovar name including null */
	memcpy(p, iovar, namelen);
	p += namelen;

	/* bss config index as first param */
	memcpy(p, &bssidx, sizeof(int32));
	p += sizeof(int32);

	/* parameter buffer follows */
	if (paramlen)
		memcpy(p, param, paramlen);

	*plen = iolen;
	return 0;
}

/*
 * set named & bss indexed driver variable to buffer value
 */
static int
wl_bssiovar_setbuf(struct net_device *dev, char *iovar, int bssidx, void *param, int paramlen, void *bufptr,
                   int buflen)
{
	int err;
	int iolen;

	err = wl_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &iolen);
	if (err)
		return err;

	return wl_ioctl(dev, WLC_SET_VAR, bufptr, iolen);
}

/*
 * get named & bss indexed driver variable buffer value
 */
static int
wl_bssiovar_getbuf(struct net_device *dev, char *iovar, int bssidx, void *param, int paramlen, void *bufptr,
                   int buflen)
{
	int err;
	int iolen;

	err = wl_bssiovar_mkbuf(iovar, bssidx, param, paramlen, bufptr, buflen, &iolen);
	if (err)
		return err;

	return wl_ioctl(dev, WLC_GET_VAR, bufptr, buflen);
}

/*
 * set named & bss indexed driver variable to buffer value
 */
static int
wl_bssiovar_set(struct net_device *dev, char *iovar, int bssidx, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN];

	return wl_bssiovar_setbuf(dev, iovar, bssidx, param, paramlen, smbuf, sizeof(smbuf));
}

/*
 * get named & bss indexed driver variable buffer value
 */
static int
wl_bssiovar_get(struct net_device *dev, char *iovar, int bssidx, void *outbuf, int len)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int err;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (len > (int)sizeof(smbuf)) {
		err = wl_bssiovar_getbuf(dev, iovar, bssidx, NULL, 0, outbuf, len);
	} else {
		memset(smbuf, 0, sizeof(smbuf));
		err = wl_bssiovar_getbuf(dev, iovar, bssidx, NULL, 0, smbuf, sizeof(smbuf));
		if (err == 0)
			memcpy(outbuf, smbuf, len);
	}

	return err;
}
#endif
#endif

#ifndef WL_WEXT

int get_primary_key(struct net_device *dev)
{
	int key, val;
	
	for (key = val = 0; (key < 4) && (val == 0); key++) {
		val = key;
		if (wl_ioctl(dev, WLC_GET_KEY_PRIMARY, &val, sizeof(val)) < 0)
			return -EINVAL;
	}
	return key;
}


static int wlcompat_ioctl_getiwrange(struct net_device *dev,
				    char *extra)
{
	int i, k;
	struct iw_range *range;

	range = (struct iw_range *) extra;
	memset(extra, 0, sizeof(struct iw_range));

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = WIRELESS_EXT;
	
	range->min_nwid = range->max_nwid = 0;
	
	range->num_channels = NUM_CHANNELS;
	k = 0;
	for (i = 0; i < NUM_CHANNELS; i++) {
		range->freq[k].i = i + 1;
		range->freq[k].m = channel_frequency[i] * 100000;
		range->freq[k].e = 1;
		k++;
		if (k >= IW_MAX_FREQUENCIES)
			break;
	}
	range->num_frequency = k;
	range->sensitivity = 3;

	/* nbd: don't know what this means, but other drivers set it this way */
	range->pmp_flags = IW_POWER_PERIOD;
	range->pmt_flags = IW_POWER_TIMEOUT;
	range->pm_capa = IW_POWER_PERIOD | IW_POWER_TIMEOUT | IW_POWER_UNICAST_R;

	range->min_pmp = 0;
	range->max_pmp = 65535000;
	range->min_pmt = 0;
	range->max_pmt = 65535 * 1000;

	range->max_qual.qual = 0;
	range->max_qual.level = 0;
	range->max_qual.noise = 0;
	
	range->min_rts = 0;
	if (wl_iovar_get(dev, "rtsthresh", &range->max_rts, sizeof(int)) < 0)
		range->max_rts = 2347;

	range->min_frag = 256;
	
	if (wl_iovar_get(dev, "fragthresh", &range->max_frag, sizeof(int)) < 0)
		range->max_frag = 2346;

	range->txpower_capa = IW_TXPOW_DBM;

	return 0;
}


static int wlcompat_set_scan(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra)
{
	int ap = 0;
	wl_scan_params_t params;

	memset(&params, 0, sizeof(params));

	/* use defaults (same parameters as wl scan) */
	memset(&params.bssid, 0xff, sizeof(params.bssid));
	params.bss_type = DOT11_BSSTYPE_ANY;
	params.scan_type = -1;
	params.nprobes = -1;
	params.active_time = -1;
	params.passive_time = -1;
	params.home_time = -1;
	
	/* can only scan in STA mode */
	wl_ioctl(dev, WLC_GET_AP, &last_mode, sizeof(last_mode));
	if (last_mode > 0) {
		/* switch to ap mode, scan result query will switch back */
		wl_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap));

		/* wait 250 msec after mode change */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(msecs_to_jiffies(250));
	}

	scan_cur = SCAN_RETRY_MAX;
	while (scan_cur-- && (wl_ioctl(dev, WLC_SCAN, &params, 64) < 0)) {
		/* sometimes the driver takes a few tries... */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(msecs_to_jiffies(250));
	}

	if (!scan_cur) 
		return -EINVAL;
	
	scan_cur = 0;

	/* wait at least 2 seconds for results */
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(msecs_to_jiffies(2000));
	
	return 0;
}


struct iw_statistics *wlcompat_get_wireless_stats(struct net_device *dev)
{
	struct wl_bss_info *bss_info = (struct wl_bss_info *) buf;
	get_pktcnt_t pkt;
	unsigned int rssi, noise, ap;
	
	memset(&wstats, 0, sizeof(wstats));
	memset(&pkt, 0, sizeof(pkt));
	memset(buf, 0, sizeof(buf));
	bss_info->version = 0x2000;
	wl_ioctl(dev, WLC_GET_BSS_INFO, bss_info, WLC_IOCTL_MAXLEN);
	wl_ioctl(dev, WLC_GET_PKTCNTS, &pkt, sizeof(pkt));

	rssi = 0;
	if ((wl_ioctl(dev, WLC_GET_AP, &ap, sizeof(ap)) < 0) || ap) {
		if (wl_ioctl(dev, WLC_GET_PHY_NOISE, &noise, sizeof(noise)) < 0)
			noise = 0;
	} else {
		// somehow the structure doesn't fit here
		rssi = buf[82];
		noise = buf[84];
	}
	rssi = (rssi == 0 ? 1 : rssi);
	wstats.qual.updated = 0x10;
	if (rssi <= 1) 
		wstats.qual.updated |= 0x20;
	if (noise <= 1)
		wstats.qual.updated |= 0x40;

	if ((wstats.qual.updated & 0x60) == 0x60)
		return NULL;

	wstats.qual.level = rssi;
	wstats.qual.noise = noise;
	wstats.discard.misc = pkt.rx_bad_pkt;
	wstats.discard.retries = pkt.tx_bad_pkt;

	return &wstats;
}

static int wlcompat_get_scan(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra)
{
	wl_scan_results_t *results = (wl_scan_results_t *) buf;
	wl_bss_info_t *bss_info;
	char *info_ptr;
	char *current_ev = extra;
	char *current_val;
	char *end_buf = extra + IW_SCAN_MAX_DATA;
	struct iw_event iwe;
	int i, j;
	int rssi, noise;
	
	memset(buf, 0, WLC_IOCTL_MAXLEN);
	results->buflen = WLC_IOCTL_MAXLEN - sizeof(wl_scan_results_t);
	
	if (wl_ioctl(dev, WLC_SCAN_RESULTS, buf, WLC_IOCTL_MAXLEN) < 0)
		return -EAGAIN;
	
	if ((results->count <= 0) && (scan_cur++ < SCAN_RETRY_MAX))
		return -EAGAIN;
	
	bss_info = &(results->bss_info[0]);
	info_ptr = (char *) bss_info;
	for (i = 0; i < results->count; i++) {
		/* send the cell address (must be sent first) */
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(&iwe.u.ap_addr.sa_data, &bss_info->BSSID, sizeof(bss_info->BSSID));
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_ADDR_LEN);
		
		/* send the ESSID */
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.length = bss_info->SSID_len;
		if (iwe.u.data.length > IW_ESSID_MAX_SIZE)
			iwe.u.data.length = IW_ESSID_MAX_SIZE;
		iwe.u.data.flags = 1;
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, bss_info->SSID);

		/* send mode */
		if  (bss_info->capability & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) {
			iwe.cmd = SIOCGIWMODE;
			if (bss_info->capability & DOT11_CAP_ESS)
				iwe.u.mode = IW_MODE_MASTER;
			else if (bss_info->capability & DOT11_CAP_IBSS)
				iwe.u.mode = IW_MODE_ADHOC;
			current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_UINT_LEN);
		}

		/* send frequency/channel info */
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.e = 0;
		iwe.u.freq.m = bss_info->chanspec & WL_CHANSPEC_CHAN_MASK;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_FREQ_LEN);

		/* add quality statistics */
		iwe.cmd = IWEVQUAL;
		iwe.u.qual.qual = 0;
		iwe.u.qual.level = bss_info->RSSI;
		iwe.u.qual.noise = bss_info->phy_noise;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
	
		/* send encryption capability */
		iwe.cmd = SIOCGIWENCODE;
		iwe.u.data.pointer = NULL;
		iwe.u.data.length = 0;
		if (bss_info->capability & DOT11_CAP_PRIVACY)
			iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, NULL);

		/* send rate information */
		iwe.cmd = SIOCGIWRATE;
		current_val = current_ev + IW_EV_LCP_LEN;
		iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
		
		for(j = 0 ; j < bss_info->rateset.count ; j++) {
			iwe.u.bitrate.value = ((bss_info->rateset.rates[j] & 0x7f) * 500000);
			current_val = iwe_stream_add_value(current_ev, current_val, end_buf, &iwe, IW_EV_PARAM_LEN);
		}
		if((current_val - current_ev) > IW_EV_LCP_LEN)
			current_ev = current_val;

		info_ptr += sizeof(wl_bss_info_t);
		if (bss_info->ie_length % 4)
			info_ptr += bss_info->ie_length + 4 - (bss_info->ie_length % 4);
		else
			info_ptr += bss_info->ie_length;
		bss_info = (wl_bss_info_t *) info_ptr;
	}
	
	wrqu->data.length = (current_ev - extra);
	wrqu->data.flags = 0;

	if (last_mode > 0)
		/* switch back to ap mode */
		wl_ioctl(dev, WLC_SET_AP, &last_mode, sizeof(last_mode));
	
	return 0;
}

static int wlcompat_ioctl(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra)
{
	switch (info->cmd) {
		case SIOCGIWNAME:
			strcpy(wrqu->name, "IEEE 802.11-DS");
			break;
		case SIOCGIWFREQ:
		{
			channel_info_t ci;

			if (wl_ioctl(dev,WLC_GET_CHANNEL, &ci, sizeof(ci)) < 0)
				return -EINVAL;

			wrqu->freq.m = ci.target_channel;
			wrqu->freq.e = 0;
			break;
		}
		case SIOCSIWFREQ:
		{
			if (wrqu->freq.m == -1) {
				wrqu->freq.m = 0;
				if (wl_ioctl(dev, WLC_SET_CHANNEL, &wrqu->freq.m, sizeof(int)) < 0)
					return -EINVAL;
			} else {
				if (wrqu->freq.e == 1) {
					int channel = 0;
					int f = wrqu->freq.m / 100000;
					while ((channel < NUM_CHANNELS + 1) && (f != channel_frequency[channel]))
						channel++;
					
					if (channel == NUM_CHANNELS) // channel not found
						return -EINVAL;

					wrqu->freq.e = 0;
					wrqu->freq.m = channel + 1;
				}
				if ((wrqu->freq.e == 0) && (wrqu->freq.m < 1000)) {
					if (wl_ioctl(dev, WLC_SET_CHANNEL, &wrqu->freq.m, sizeof(int)) < 0)
						return -EINVAL;
				} else {
					return -EINVAL;
				}
			}
			break;
		}
		case SIOCSIWAP:
		{
			int ap = 0;
			int infra = 0;
			rw_reg_t reg;

			memset(&reg, 0, sizeof(reg));

			if (wrqu->ap_addr.sa_family != ARPHRD_ETHER)
				return -EINVAL;

			if (wl_ioctl(dev, WLC_GET_AP, &ap, sizeof(ap)) < 0)
				return -EINVAL;

			if (wl_ioctl(dev, WLC_GET_INFRA, &infra, sizeof(infra)) < 0)
				return -EINVAL;

			if (!infra) 
				wl_ioctl(dev, WLC_SET_BSSID, wrqu->ap_addr.sa_data, 6);

			if (wl_ioctl(dev, ((ap || !infra) ? WLC_SET_BSSID : WLC_REASSOC), wrqu->ap_addr.sa_data, 6) < 0)
				return -EINVAL;

			break;
		}
		case SIOCGIWAP:
		{
			wrqu->ap_addr.sa_family = ARPHRD_ETHER;
			if (wl_ioctl(dev,WLC_GET_BSSID,wrqu->ap_addr.sa_data,6) < 0)
				return -EINVAL;
			break;
		}
		case SIOCGIWESSID:
		{
			wlc_ssid_t ssid;
			
			if (wl_ioctl(dev,WLC_GET_SSID, &ssid, sizeof(wlc_ssid_t)) < 0)
				return -EINVAL;

			wrqu->essid.flags = wrqu->data.flags = 1;
			wrqu->essid.length = wrqu->data.length = ssid.SSID_len + 1;
			memcpy(extra,ssid.SSID,ssid.SSID_len + 1);
			break;
		}
		case SIOCSIWESSID:
		{
			wlc_ssid_t ssid;
			memset(&ssid, 0, sizeof(ssid));
			ssid.SSID_len = strlen(extra);
			if (ssid.SSID_len > 32)
				ssid.SSID_len = 32;
			memcpy(ssid.SSID, extra, ssid.SSID_len);
			if (wl_ioctl(dev, WLC_SET_SSID, &ssid, sizeof(ssid)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCGIWRTS:
		{
			if (wl_iovar_get(dev, "rtsthresh", &(wrqu->rts.value), sizeof(int)) < 0) 
				return -EINVAL;
			break;
		}
		case SIOCSIWRTS:
		{
			if (wl_iovar_set(dev, "rtsthresh", &(wrqu->rts.value), sizeof(int)) < 0) 
				return -EINVAL;
			break;
		}
		case SIOCGIWFRAG:
		{
			if (wl_iovar_get(dev, "fragthresh", &(wrqu->frag.value), sizeof(int)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCSIWFRAG:
		{
			if (wl_iovar_set(dev, "fragthresh", &(wrqu->frag.value), sizeof(int)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCGIWTXPOW:
		{
			int radio, override;

			wl_ioctl(dev, WLC_GET_RADIO, &radio, sizeof(int));
			
			if (wl_iovar_get(dev, "qtxpower", &(wrqu->txpower.value), sizeof(int)) < 0)
				return -EINVAL;
			
			override = (wrqu->txpower.value & WL_TXPWR_OVERRIDE) == WL_TXPWR_OVERRIDE;
			wrqu->txpower.value &= ~WL_TXPWR_OVERRIDE;
			if (!override && (wrqu->txpower.value > 76))
				wrqu->txpower.value = 76;
			wrqu->txpower.value /= 4;
				
			wrqu->txpower.fixed = 0;
			wrqu->txpower.disabled = radio;
			wrqu->txpower.flags = IW_TXPOW_DBM;
			break;
		}
		case SIOCSIWTXPOW:
		{
			/* This is weird: WLC_SET_RADIO with 1 as argument disables the radio */
			int radio = wrqu->txpower.disabled;

			wl_ioctl(dev, WLC_SET_RADIO, &radio, sizeof(int));
			
			if (!wrqu->txpower.disabled && (wrqu->txpower.value > 0)) {
				int value;
				
				if (wl_iovar_get(dev, "qtxpower", &value, sizeof(int)) < 0)
					return -EINVAL;
				
				value &= WL_TXPWR_OVERRIDE;
				wrqu->txpower.value *= 4;
				wrqu->txpower.value |= value;
				
				if (wrqu->txpower.flags != IW_TXPOW_DBM)
					return -EINVAL;
				
				if (wrqu->txpower.value > 0)
					if (wl_iovar_set(dev, "qtxpower", &(wrqu->txpower.value), sizeof(int)) < 0)
						return -EINVAL;
			}
			break;
		}
		case SIOCSIWENCODE:
		{
			int val = 0, wep = 1, wrestrict = 1;
			int index = (wrqu->data.flags & IW_ENCODE_INDEX) - 1;

			if (index < 0)
				index = get_primary_key(dev);
			
			if (wrqu->data.flags & IW_ENCODE_DISABLED) {
				wep = 0;
				if (wl_ioctl(dev, WLC_SET_WSEC, &wep, sizeof(val)) < 0)
					return -EINVAL;
				return 0;
			}

			if (wl_ioctl(dev, WLC_SET_WSEC, &wep, sizeof(val)) < 0)
				return -EINVAL;

			if (wrqu->data.flags & IW_ENCODE_OPEN)
				wrestrict = 0;
			
			if (wrqu->data.pointer && (wrqu->data.length > 0) && (wrqu->data.length <= 16)) {
				wl_wsec_key_t key;
				memset(&key, 0, sizeof(key));

				key.flags = WL_PRIMARY_KEY;
				key.len = wrqu->data.length;
				key.index = index;
				memcpy(key.data, wrqu->data.pointer, wrqu->data.length);

				if (wl_ioctl(dev, WLC_SET_KEY, &key, sizeof(key)) < 0)
					return -EINVAL;
			}

			if (index >= 0)
				wl_ioctl(dev, WLC_SET_KEY_PRIMARY, &index, sizeof(index));
			
			if (wrestrict >= 0)
				wl_ioctl(dev, WLC_SET_WEP_RESTRICT, &wrestrict, sizeof(wrestrict));

			break;
		}
		case SIOCGIWENCODE:
		{
			int val;
			int key = get_primary_key(dev);
			int *info_addr; 
			wkey *wep_key;
			
			if (wl_ioctl(dev, WLC_GET_WSEC, &val, sizeof(val)) < 0)
				return -EINVAL;
			if (!(val & WEP_ENABLED)) {
				wrqu->data.flags = IW_ENCODE_DISABLED;
				break;
			}

			key = get_primary_key(dev);
			wrqu->data.flags = IW_ENCODE_ENABLED;

			/* the driver apparently doesn't allow us to read the wep key */
			wrqu->data.flags |= IW_ENCODE_NOKEY;

			break;
		}
		case SIOCGIWRANGE:
		{
			return wlcompat_ioctl_getiwrange(dev, extra);
			break;
		}
		case SIOCSIWMODE:
		{
			int ap = -1, infra = -1, passive = 0, wet = 0;

			wl_ioctl(dev, WLC_GET_WET, &wet, sizeof(wet));
			switch (wrqu->mode) {
				case IW_MODE_MONITOR:
					passive = 1;
					break;
				case IW_MODE_ADHOC:
					infra = 0;
					ap = 0;
					break;
				case IW_MODE_MASTER:
					infra = 1;
					ap = 1;
					break;
				case IW_MODE_INFRA:
					infra = 1;
					ap = 0;
					wet = 0;
					break;
				case IW_MODE_REPEAT:
					infra = 1;
					ap = 0;
					wet = 1;
					break;
				default:
					return -EINVAL;
			}

			wl_ioctl(dev, WLC_SET_PASSIVE, &passive, sizeof(passive));
			wl_ioctl(dev, WLC_SET_MONITOR, &passive, sizeof(passive));
			if ((ap == 0) && (infra == 1))
				wl_ioctl(dev, WLC_SET_WET, &wet, sizeof(wet));
			if (ap >= 0)
				wl_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap));
			if (infra >= 0)
				wl_ioctl(dev, WLC_SET_INFRA, &infra, sizeof(infra));

			break;
		}
		case SIOCGIWMODE:
		{
			int ap, infra, wet, passive;

			if (wl_ioctl(dev, WLC_GET_AP, &ap, sizeof(ap)) < 0)
				return -EINVAL;
			if (wl_ioctl(dev, WLC_GET_INFRA, &infra, sizeof(infra)) < 0)
				return -EINVAL;
			if (wl_ioctl(dev, WLC_GET_PASSIVE, &passive, sizeof(passive)) < 0)
				return -EINVAL;

			if (passive) {
				wrqu->mode = IW_MODE_MONITOR;
			} else if (!infra) {
				wrqu->mode = IW_MODE_ADHOC;
			} else {
				if (ap) {
					wrqu->mode = IW_MODE_MASTER;
				} else {
					wrqu->mode = IW_MODE_INFRA;
				}
			}
			break;
		}
		default:
			return -EINVAL;
			break;
	}
	
	return 0;
}

static const iw_handler	 wlcompat_handler[] = {
	NULL,			/* SIOCSIWCOMMIT */
	wlcompat_ioctl,		/* SIOCGIWNAME */
	NULL,			/* SIOCSIWNWID */
	NULL,			/* SIOCGIWNWID */
	wlcompat_ioctl,		/* SIOCSIWFREQ */
	wlcompat_ioctl,		/* SIOCGIWFREQ */
	wlcompat_ioctl,		/* SIOCSIWMODE */
	wlcompat_ioctl,		/* SIOCGIWMODE */
	NULL,			/* SIOCSIWSENS */
	NULL,			/* SIOCGIWSENS */
	NULL,			/* SIOCSIWRANGE, unused */
	wlcompat_ioctl,		/* SIOCGIWRANGE */
	NULL,			/* SIOCSIWPRIV */
	NULL,			/* SIOCGIWPRIV */
	NULL,			/* SIOCSIWSTATS */
	NULL,			/* SIOCGIWSTATS */
	iw_handler_set_spy,	/* SIOCSIWSPY */
	iw_handler_get_spy,	/* SIOCGIWSPY */
	iw_handler_set_thrspy,  /* SIOCSIWTHRSPY */
	iw_handler_get_thrspy,  /* SIOCGIWTHRSPY */
	wlcompat_ioctl,		/* SIOCSIWAP */
	wlcompat_ioctl,		/* SIOCGIWAP */
	NULL,			/* -- hole -- */
	NULL,			/* SIOCGIWAPLIST */
	wlcompat_set_scan,	/* SIOCSIWSCAN */
	wlcompat_get_scan,	/* SIOCGIWSCAN */
	wlcompat_ioctl,		/* SIOCSIWESSID */
	wlcompat_ioctl,		/* SIOCGIWESSID */
	NULL,			/* SIOCSIWNICKN */
	NULL,			/* SIOCGIWNICKN */
	NULL,			/* -- hole -- */
	NULL,			/* -- hole -- */
	NULL,			/* SIOCSIWRATE */
	NULL,			/* SIOCGIWRATE */
	wlcompat_ioctl,		/* SIOCSIWRTS */
	wlcompat_ioctl,		/* SIOCGIWRTS */
	wlcompat_ioctl,		/* SIOCSIWFRAG */
	wlcompat_ioctl,		/* SIOCGIWFRAG */
	wlcompat_ioctl,		/* SIOCSIWTXPOW */
	wlcompat_ioctl,		/* SIOCGIWTXPOW */
	NULL,			/* SIOCSIWRETRY */
	NULL,			/* SIOCGIWRETRY */
	wlcompat_ioctl,		/* SIOCSIWENCODE */
	wlcompat_ioctl,		/* SIOCGIWENCODE */
};


static const struct iw_handler_def wlcompat_handler_def =
{
	.standard	= (iw_handler *) wlcompat_handler,
	.num_standard	= sizeof(wlcompat_handler)/sizeof(iw_handler),
};

#endif

#ifdef DEBUG
void print_buffer(int len, unsigned char *buf) {
	int x;
	if (buf != NULL) {
		for (x=0;x<len && x<180 ;x++) {
			if ((x % 4) == 0)
				printk(" ");
			printk("%02X",buf[x]);
		}
	} else {
		printk(" NULL");
	}
	printk("\n");

}
#endif
static int (*old_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
static int new_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd) {
	int ret = 0;
	
#ifdef DEBUG
	printk("dev: %s ioctl: 0x%04x\n",dev->name,cmd);
	if (cmd==SIOCDEVPRIVATE) {
		wl_ioctl_t *ioc = (wl_ioctl_t *)ifr->ifr_data;
		unsigned char *buf = ioc->buf;
		printk("   cmd: %d buf: 0x%08x len: %d\n",ioc->cmd,&(ioc->buf),ioc->len);
		printk("   send: ->");
		print_buffer(ioc->len, buf);
		ret = old_ioctl(dev,ifr,cmd);
		printk("   recv: ->");
		print_buffer(ioc->len, buf);
		printk("   ret: %d\n", ret);
	} else
#endif
	{
		ret = old_ioctl(dev,ifr,cmd);
	}
	return ret;
}

#ifndef DEBUG
static struct timer_list rng_timer;
static spinlock_t rng_lock = SPIN_LOCK_UNLOCKED;

static void rng_timer_tick(unsigned long n)
{
	struct net_device *dev = (struct net_device *) n;
	unsigned long flags;
	u16 data[4];
	int i, ret;
	
	ret = 0;
	spin_lock_irqsave(&rng_lock, flags);
	for (i = 0; i < 3; i++) {
		ret |= wl_iovar_get(dev, "rand", &data[i], sizeof(u16));
	}
	spin_unlock_irqrestore(&rng_lock, flags);

	if (!ret)
		batch_entropy_store(*((u32 *) &data[0]), *((u32 *) &data[2]), (jiffies % 255));

	mod_timer(&rng_timer, jiffies + (HZ/RNG_POLL_FREQ));
}
#endif

static int __init wlcompat_init()
{
	int found = 0, i;
	char devname[4] = "wl0";

	while (!found && (dev = dev_get_by_name(devname))) {
		if ((wl_ioctl(dev, WLC_GET_MAGIC, &i, sizeof(i)) == 0) && (i == WLC_IOCTL_MAGIC))
			found = 1;
		devname[2]++;
	}


	if (!found) {
		printk("No Broadcom devices found.\n");
		return -ENODEV;
	}

	old_ioctl = dev->do_ioctl;
	dev->do_ioctl = new_ioctl;
#ifndef WL_WEXT	
	dev->wireless_handlers = (struct iw_handler_def *)&wlcompat_handler_def;
	dev->get_wireless_stats = wlcompat_get_wireless_stats;
#endif

#ifndef DEBUG
	if (random) {
		init_timer(&rng_timer);
		rng_timer.function = rng_timer_tick;
		rng_timer.data = (unsigned long) dev;
		rng_timer_tick((unsigned long) dev);
	}
#endif
	
#ifdef DEBUG
	printk("broadcom driver private data: 0x%08x\n", dev->priv);
#endif
	return 0;
}

static void __exit wlcompat_exit()
{
#ifndef DEBUG
	if (random)
		del_timer(&rng_timer);
#endif
#ifndef WL_WEXT	
	dev->get_wireless_stats = NULL;
	dev->wireless_handlers = NULL;
#endif
	dev->do_ioctl = old_ioctl;
	return;
}

EXPORT_NO_SYMBOLS;
MODULE_AUTHOR("openwrt.org");
MODULE_LICENSE("GPL");

#ifndef DEBUG
module_param(random, int, 0);
#endif
module_init(wlcompat_init);
module_exit(wlcompat_exit);
