// insert header here
// mbm. gpl.

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>
#include <linux/wireless.h>

#include <net/iw_handler.h>
#include <wlioctl.h>

#define DEBUG

static struct net_device *dev;

/* The frequency of each channel in MHz */
const long channel_frequency[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};
#define NUM_CHANNELS ( sizeof(channel_frequency) / sizeof(channel_frequency[0]) )

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

	range->min_rts = 0;
	if (wl_ioctl(dev, WLC_GET_RTS, &range->max_rts, sizeof(int)) < 0)
		range->max_rts = 2347;

	range->min_frag = 256;
	
	if (wl_ioctl(dev, WLC_GET_FRAG, &range->max_frag, sizeof(int)) < 0)
		range->max_frag = 2346;

	range->min_pmp = 0;
	range->max_pmp = 65535000;
	range->min_pmt = 0;
	range->max_pmt = 65535 * 1000;

	range->txpower_capa = IW_TXPOW_MWATT;

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
			wrqu->txpower.value = 0;
			if (wl_ioctl(dev,WLC_GET_TXPWR, &(wrqu->txpower.value), sizeof(int)) < 0)
				return -EINVAL;
			wrqu->txpower.fixed = 0;
			wrqu->txpower.disabled = 0;
			wrqu->txpower.flags = IW_TXPOW_MWATT;
			break;
		}
		case SIOCSIWTXPOW:
		{
			if (wrqu->txpower.flags != IW_TXPOW_MWATT)
				return -EINVAL;

			if (wl_ioctl(dev, WLC_SET_TXPWR, &wrqu->txpower.value, sizeof(int)) < 0)
				return -EINVAL;
		}
		case SIOCGIWENCODE:
		{
			wrqu->data.flags = IW_ENCODE_DISABLED;
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
			
			if (wl_ioctl(dev, WLC_SET_PASSIVE, &passive, sizeof(passive)) < 0)
				return -EINVAL;
			if (wl_ioctl(dev, WLC_SET_MONITOR, &passive, sizeof(passive)) < 0)
				return -EINVAL;
			if (wl_ioctl(dev, WLC_SET_WET, &wet, sizeof(wet)) < 0)
				return -EINVAL;
			if (ap >= 0)
				if (wl_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap)) < 0)
					return -EINVAL;
			if (infra >= 0)
				if (wl_ioctl(dev, WLC_SET_INFRA, &infra, sizeof(infra)) < 0)
					return -EINVAL;

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
	NULL,			/* SIOCSIWAP */
	wlcompat_ioctl,		/* SIOCGIWAP */
	NULL,			/* -- hole -- */
	NULL,			/* SIOCGIWAPLIST */
	NULL,			/* -- hole -- */
	NULL,			/* -- hole -- */
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
	NULL,			/* SIOCSIWENCODE */
	wlcompat_ioctl,		/* SIOCGIWENCODE */
};

static const struct iw_handler_def      wlcompat_handler_def =
{
	.standard	= (iw_handler *) wlcompat_handler,
	.num_standard	= sizeof(wlcompat_handler)/sizeof(iw_handler),
	.private	= NULL,
	.num_private	= 0,
	.private_args	= NULL, 
	.num_private_args = 0,
};

#ifdef DEBUG
static int (*old_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
static int new_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd) {
	int ret = old_ioctl(dev,ifr,cmd);
	printk("dev: %s ioctl: 0x%04x\n",dev->name,cmd);
	if (cmd==SIOCDEVPRIVATE) {
		int x;
		wl_ioctl_t *ioc = (wl_ioctl_t *)ifr->ifr_data;
		unsigned char *buf = ioc->buf;
		printk("   cmd: %d buf: 0x%08x len: %d\n",ioc->cmd,&(ioc->buf),ioc->len);
		printk("   ->");
		for (x=0;x<ioc->len && x<128 ;x++) {
			printk("%02X",buf[x]);
		}
		printk("\n");
	}
	return ret;
}
#endif

static int __init wlcompat_init()
{
	dev = dev_get_by_name("eth1");
#ifdef DEBUG
	old_ioctl = dev->do_ioctl;
	dev->do_ioctl = new_ioctl;
#endif
	dev->wireless_handlers = (struct iw_handler_def *)&wlcompat_handler_def;
	return 0;
}

static void __exit wlcompat_exit()
{
	dev->wireless_handlers = NULL;
#ifdef DEBUG
	dev->do_ioctl = old_ioctl;
#endif
	return;
}

EXPORT_NO_SYMBOLS;
MODULE_AUTHOR("openwrt.org");
MODULE_LICENSE("GPL");

module_init(wlcompat_init);
module_exit(wlcompat_exit);
