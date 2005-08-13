/*
 * wlcompat.c
 *
 * Copyright (C) 2005 Mike Baker,
 *                    Felix Fietkau <openwrt@nbd.name>
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
#include <linux/init.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>
#include <linux/wireless.h>

#include <net/iw_handler.h>
#include <wlioctl.h>

static struct net_device *dev;
static unsigned short bss_force;
char buf[WLC_IOCTL_MAXLEN];

/* The frequency of each channel in MHz */
const long channel_frequency[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};
#define NUM_CHANNELS ( sizeof(channel_frequency) / sizeof(channel_frequency[0]) )

typedef struct internal_wsec_key {
	uint8 index;		// 0x00
	uint8 unknown_1;	// 0x01
	uint8 type;		// 0x02
	uint8 unknown_2[7];	// 0x03
	uint8 len;		// 0x0a
	uint8 pad[3];
	char data[32];		// 0x0e
} wkey;


static int wlcompat_private_ioctl(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra);
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

static int wl_set_val(struct net_device *dev, char *var, void *val, int len)
{
	char buf[128];
	int buf_len;
	int ret;

	/* check for overflow */
	if ((buf_len = strlen(var)) + 1 + len > sizeof(buf))
		return -1;
	
	strcpy(buf, var);
	buf_len += 1;

	/* append int value onto the end of the name string */
	memcpy(&(buf[buf_len]), val, len);
	buf_len += len;

	ret = wl_ioctl(dev, WLC_SET_VAR, buf, buf_len);
	return ret;
}

static int wl_get_val(struct net_device *dev, char *var, void *val, int len)
{
	char buf[128];
	int buf_len;
	int ret;

	/* check for overflow */
	if ((buf_len = strlen(var)) + 1 > sizeof(buf) || len > sizeof(buf))
		return -1;
	
	strcpy(buf, var);
	if (ret = wl_ioctl(dev, WLC_GET_VAR, buf, buf_len + len))
		return ret;

	memcpy(val, buf, len);
	return 0;
}

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

	range->min_rts = 0;
	if (wl_ioctl(dev, WLC_GET_RTS, &range->max_rts, sizeof(int)) < 0)
		range->max_rts = 2347;

	range->min_frag = 256;
	
	if (wl_ioctl(dev, WLC_GET_FRAG, &range->max_frag, sizeof(int)) < 0)
		range->max_frag = 2346;

	range->txpower_capa = IW_TXPOW_DBM;

	return 0;
}


static int wlcompat_set_scan(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra)
{
	int ap = 0, oldap = 0;
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
	wl_ioctl(dev, WLC_GET_AP, &oldap, sizeof(oldap));
	if (oldap > 0)
		wl_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap));
	
	if (wl_ioctl(dev, WLC_SCAN, &params, 64) < 0)
		return -EINVAL;
	
	if (oldap > 0)
		wl_ioctl(dev, WLC_SET_AP, &oldap, sizeof(oldap));

	return 0;
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

	if (wl_ioctl(dev, WLC_SCAN_RESULTS, buf, WLC_IOCTL_MAXLEN) < 0)
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

		/* send frequency/channel info */
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.e = 0;
		iwe.u.freq.m = bss_info->channel;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_FREQ_LEN);

		/* add quality statistics */
		iwe.cmd = IWEVQUAL;
		iwe.u.qual.level = bss_info->RSSI;
		iwe.u.qual.noise = bss_info->phy_noise;
		iwe.u.qual.qual = 0;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
	
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

			if (!infra) {
				wl_ioctl(dev, WLC_SET_BSSID, wrqu->ap_addr.sa_data, 6);

				reg.size = 4;
				reg.byteoff = 0x184;
				reg.val = bss_force << 16 | bss_force;
				wl_ioctl(dev, WLC_W_REG, &reg, sizeof(reg));
				
				reg.byteoff = 0x180;
				wl_ioctl(dev, WLC_R_REG, &reg, sizeof(reg));
				reg.val = bss_force << 16;
				wl_ioctl(dev, WLC_W_REG, &reg, sizeof(reg));
			}

			if (wl_ioctl(dev, ((ap || !infra) ? WLC_SET_BSSID : WLC_REASSOC), wrqu->ap_addr.sa_data, 6) < 0)
				return -EINVAL;

			break;
		}
		case SIOCGIWAP:
		{
#ifdef DEBUG
			rw_reg_t reg;
			memset(&reg, 0, sizeof(reg));

			reg.size = 4;
			reg.byteoff = 0x184;
			wl_ioctl(dev, WLC_R_REG, &reg, sizeof(reg));
			printk("bss time = 0x%08x", reg.val);
			
			reg.byteoff = 0x180;
			wl_ioctl(dev, WLC_R_REG, &reg, sizeof(reg));
			printk("%08x\n", reg.val);
#endif
			
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
			if (ssid.SSID_len > WLC_ESSID_MAX_SIZE)
				ssid.SSID_len = WLC_ESSID_MAX_SIZE;
			memcpy(ssid.SSID, extra, ssid.SSID_len);
			if (wl_ioctl(dev, WLC_SET_SSID, &ssid, sizeof(ssid)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCGIWRTS:
		{
			if (wl_ioctl(dev,WLC_GET_RTS,&(wrqu->rts.value),sizeof(int)) < 0) 
				return -EINVAL;
			break;
		}
		case SIOCSIWRTS:
		{
			if (wl_ioctl(dev,WLC_SET_RTS,&(wrqu->rts.value),sizeof(int)) < 0) 
				return -EINVAL;
			break;
		}
		case SIOCGIWFRAG:
		{
			if (wl_ioctl(dev,WLC_GET_FRAG,&(wrqu->frag.value),sizeof(int)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCSIWFRAG:
		{
			if (wl_ioctl(dev,WLC_SET_FRAG,&(wrqu->frag.value),sizeof(int)) < 0)
				return -EINVAL;
			break;
		}
		case SIOCGIWTXPOW:
		{
			int radio;

			wl_ioctl(dev, WLC_GET_RADIO, &radio, sizeof(int));
			
			if (wl_get_val(dev, "qtxpower", &(wrqu->txpower.value), sizeof(int)) < 0)
				return -EINVAL;
			
			wrqu->txpower.value &= ~WL_TXPWR_OVERRIDE;
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
				
				if (wl_get_val(dev, "qtxpower", &value, sizeof(int)) < 0)
					return -EINVAL;
				
				value &= WL_TXPWR_OVERRIDE;
				wrqu->txpower.value *= 4;
				wrqu->txpower.value |= value;
				
				if (wrqu->txpower.flags != IW_TXPOW_DBM)
					return -EINVAL;
				
				if (wrqu->txpower.value > 0)
					if (wl_set_val(dev, "qtxpower", &(wrqu->txpower.value), sizeof(int)) < 0)
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
			
			if (wl_ioctl(dev, WLC_GET_WEP, &val, sizeof(val)) < 0)
				return -EINVAL;
			

			if (val > 0) {
				int key = get_primary_key(dev);
				
				wrqu->data.flags = IW_ENCODE_ENABLED;
				if (key-- > 0) {
					int *info_addr; 
					wkey *wep_key;
					
					info_addr = (int *) dev->priv;
					wep_key = (wkey *) ((*info_addr) + 0x2752 + (key * 0x110));
					
					wrqu->data.flags |= key + 1;
					wrqu->data.length = wep_key->len;

					memset(extra, 0, 16);
					memcpy(extra, wep_key->data, 16);
				} else {
					wrqu->data.flags |= IW_ENCODE_NOKEY;
				}
			} else {
				wrqu->data.flags = IW_ENCODE_DISABLED;
			}
			
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
			if (wl_ioctl(dev, WLC_GET_WET, &wet, sizeof(wet)) < 0)
				return -EINVAL;

			if (passive) {
				wrqu->mode = IW_MODE_MONITOR;
			} else if (!infra) {
				wrqu->mode = IW_MODE_ADHOC;
			} else {
				if (ap) {
					wrqu->mode = IW_MODE_MASTER;
				} else {
					if (wet) {
						wrqu->mode = IW_MODE_REPEAT;
					} else {
						wrqu->mode = IW_MODE_INFRA;
					}
				}
			}
			break;
		}
		default:
		{
			if (info->cmd >= SIOCIWFIRSTPRIV)
				return wlcompat_private_ioctl(dev, info, wrqu, extra);

			return -EINVAL;
		}
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


#define WLCOMPAT_SET_MONITOR		SIOCIWFIRSTPRIV + 0
#define WLCOMPAT_GET_MONITOR		SIOCIWFIRSTPRIV + 1
#define WLCOMPAT_SET_TXPWR_LIMIT	SIOCIWFIRSTPRIV + 2
#define WLCOMPAT_GET_TXPWR_LIMIT	SIOCIWFIRSTPRIV + 3
#define WLCOMPAT_SET_ANTDIV		SIOCIWFIRSTPRIV + 4
#define WLCOMPAT_GET_ANTDIV		SIOCIWFIRSTPRIV + 5
#define WLCOMPAT_SET_TXANT		SIOCIWFIRSTPRIV + 6
#define WLCOMPAT_GET_TXANT		SIOCIWFIRSTPRIV + 7
#define WLCOMPAT_SET_BSS_FORCE		SIOCIWFIRSTPRIV + 8
#define WLCOMPAT_GET_BSS_FORCE		SIOCIWFIRSTPRIV + 9


static int wlcompat_private_ioctl(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu,
			 char *extra)
{
	int *value = (int *) wrqu->name;

	switch (info->cmd) {
		case WLCOMPAT_SET_MONITOR:
		{
			if (wl_ioctl(dev, WLC_SET_MONITOR, value, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_GET_MONITOR:
		{
			if (wl_ioctl(dev, WLC_GET_MONITOR, extra, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_SET_TXPWR_LIMIT:
		{
			int val;
			

			if (wl_get_val(dev, "qtxpower", &val, sizeof(int)) < 0)
				return -EINVAL;
			
			if (*extra > 0)
				val |= WL_TXPWR_OVERRIDE;
			else
				val &= ~WL_TXPWR_OVERRIDE;
			
			if (wl_set_val(dev, "qtxpower", &val, sizeof(int)) < 0)
				return -EINVAL;
			
			break;
		}
		case WLCOMPAT_GET_TXPWR_LIMIT:
		{
			if (wl_get_val(dev, "qtxpower", value, sizeof(int)) < 0)
				return -EINVAL;

			*value = ((*value & WL_TXPWR_OVERRIDE) == WL_TXPWR_OVERRIDE ? 1 : 0);

			break;
		}
		case WLCOMPAT_SET_ANTDIV:
		{
			if (wl_ioctl(dev, WLC_SET_ANTDIV, value, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_GET_ANTDIV:
		{
			if (wl_ioctl(dev, WLC_GET_ANTDIV, extra, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_SET_TXANT:
		{
			if (wl_ioctl(dev, WLC_SET_TXANT, value, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_GET_TXANT:
		{
			if (wl_ioctl(dev, WLC_GET_TXANT, extra, sizeof(int)) < 0)
				return -EINVAL;

			break;
		}
		case WLCOMPAT_SET_BSS_FORCE:
		{
			bss_force = (unsigned short) *value;
			break;
		}
		case WLCOMPAT_GET_BSS_FORCE:
		{
			*extra = (int) bss_force;
			break;
		}
		default:
		{
			return -EINVAL;
		}
			
	}
	return 0;
}

static const struct iw_priv_args wlcompat_private_args[] = 
{
	{	WLCOMPAT_SET_MONITOR, 
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_monitor"
	},
	{	WLCOMPAT_GET_MONITOR, 
		0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		"get_monitor"
	},
	{	WLCOMPAT_SET_TXPWR_LIMIT, 
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_txpwr_force"
	},
	{	WLCOMPAT_GET_TXPWR_LIMIT, 
		0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		"get_txpwr_force"
	},
	{	WLCOMPAT_SET_ANTDIV, 
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_antdiv"
	},
	{	WLCOMPAT_GET_ANTDIV, 
		0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		"get_antdiv"
	},
	{	WLCOMPAT_SET_TXANT, 
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_txant"
	},
	{	WLCOMPAT_GET_TXANT, 
		0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		"get_txant"
	},
	{	WLCOMPAT_SET_BSS_FORCE, 
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_bss_force"
	},
	{	WLCOMPAT_GET_BSS_FORCE, 
		0,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		"get_bss_force"
	},
};

static const iw_handler wlcompat_private[] =
{
	wlcompat_private_ioctl,
	NULL
};


static const struct iw_handler_def wlcompat_handler_def =
{
	.standard	= (iw_handler *) wlcompat_handler,
	.num_standard	= sizeof(wlcompat_handler)/sizeof(iw_handler),
	.private	= wlcompat_private,
	.num_private	= 1,
	.private_args	= wlcompat_private_args,
	.num_private_args = sizeof(wlcompat_private_args) / sizeof(wlcompat_private_args[0])
};


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
	struct iwreq *iwr = (struct iwreq *) ifr;
	struct iw_request_info info;
	
#ifdef DEBUG
	printk("dev: %s ioctl: 0x%04x\n",dev->name,cmd);
#endif

	if (cmd >= SIOCIWFIRSTPRIV) {
		info.cmd = cmd;
		info.flags = 0;
		ret = wlcompat_private_ioctl(dev, &info, &(iwr->u), (char *) &(iwr->u));
#ifdef DEBUG
	} else if (cmd==SIOCDEVPRIVATE) {
		wl_ioctl_t *ioc = (wl_ioctl_t *)ifr->ifr_data;
		unsigned char *buf = ioc->buf;
		printk("   cmd: %d buf: 0x%08x len: %d\n",ioc->cmd,&(ioc->buf),ioc->len);
		printk("   send: ->");
		print_buffer(ioc->len, buf);
		ret = old_ioctl(dev,ifr,cmd);
		printk("   recv: ->");
		print_buffer(ioc->len, buf);
		printk("   ret: %d\n", ret);
#endif
	} else {
		ret = old_ioctl(dev,ifr,cmd);
	}
	return ret;
}

static int __init wlcompat_init()
{
	int found = 0, i;
	char *devname = "eth0";
	bss_force = 0;
	
	while (!found && (dev = dev_get_by_name(devname))) {
		if ((dev->wireless_handlers == NULL) && ((wl_ioctl(dev, WLC_GET_MAGIC, &i, sizeof(i)) == 0) && i == WLC_IOCTL_MAGIC))
			found = 1;
		devname[3]++;
	}
	
	if (!found) {
		printk("No Broadcom devices found.\n");
		return -ENODEV;
	}
		

	old_ioctl = dev->do_ioctl;
	dev->do_ioctl = new_ioctl;
	dev->wireless_handlers = (struct iw_handler_def *)&wlcompat_handler_def;
#ifdef DEBUG
	printk("broadcom driver private data: 0x%08x\n", dev->priv);
#endif
	return 0;
}

static void __exit wlcompat_exit()
{
	dev->wireless_handlers = NULL;
	dev->do_ioctl = old_ioctl;
	return;
}

EXPORT_NO_SYMBOLS;
MODULE_AUTHOR("openwrt.org");
MODULE_LICENSE("GPL");

module_init(wlcompat_init);
module_exit(wlcompat_exit);
