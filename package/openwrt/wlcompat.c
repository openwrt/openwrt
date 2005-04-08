// insert header here
// mbm. gpl.

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>

#include <net/iw_handler.h>
#include <wlioctl.h>

#define DEBUG

static struct net_device *dev;

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
			wl_ioctl(dev,WLC_GET_CHANNEL, &ci, sizeof(ci));
			wrqu->freq.m = ci.target_channel;
			wrqu->freq.e = 0;
			break;
		}
		case SIOCGIWAP:
		{
			wrqu->ap_addr.sa_family = ARPHRD_ETHER;
			wl_ioctl(dev,WLC_GET_BSSID,wrqu->ap_addr.sa_data,6);
			break;
		}
		case SIOCGIWESSID:
		{
			wlc_ssid_t ssid;
			wl_ioctl(dev,WLC_GET_SSID, &ssid, sizeof(wlc_ssid_t));
			wrqu->essid.flags = wrqu->data.flags = 1;
			wrqu->essid.length = wrqu->data.length = ssid.SSID_len + 1;
			memcpy(extra,ssid.SSID,ssid.SSID_len + 1);
			break;
		}
		case SIOCGIWRTS:
		{
			wl_ioctl(dev,WLC_GET_RTS,&(wrqu->rts.value),sizeof(int));
			break;
		}
		case SIOCGIWFRAG:
		{
			wl_ioctl(dev,WLC_GET_FRAG,&(wrqu->frag.value),sizeof(int));
			break;
		}
		case SIOCGIWTXPOW:
		{
			wrqu->txpower.value = 0;
			wl_ioctl(dev,WLC_GET_TXPWR, &(wrqu->txpower.value), sizeof(int));
			wrqu->txpower.fixed = 0;
			wrqu->txpower.disabled = 0;
			wrqu->txpower.flags = IW_TXPOW_MWATT;
			break;
		}
		case SIOCGIWENCODE:
		{
			wrqu->data.flags = IW_ENCODE_DISABLED;
			break;
		}
	}
	return 0;
}

static const iw_handler	 wlcompat_handler[] = {
	NULL,			/* SIOCSIWNAME */
	wlcompat_ioctl,		/* SIOCGIWNAME */
	NULL,			/* SIOCSIWNWID */
	NULL,			/* SIOCGIWNWID */
	NULL,			/* SIOCSIWFREQ */
	wlcompat_ioctl,		/* SIOCGIWFREQ */
	NULL,			/* SIOCSIWMODE */
	NULL,			/* SIOCGIWMODE */
	NULL,			/* SIOCSIWSENS */
	NULL,			/* SIOCGIWSENS */
	NULL,			/* SIOCSIWRANGE */
	NULL,			/* SIOCGIWRANGE */
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
	NULL,			/* SIOCSIWESSID */
	wlcompat_ioctl,		/* SIOCGIWESSID */
	NULL,			/* SIOCSIWNICKN */
	NULL,			/* SIOCGIWNICKN */
	NULL,			/* -- hole -- */
	NULL,			/* -- hole -- */
	NULL,			/* SIOCSIWRATE */
	NULL,			/* SIOCGIWRATE */
	NULL,			/* SIOCSIWRTS */
	wlcompat_ioctl,		/* SIOCGIWRTS */
	NULL,			/* SIOCSIWFRAG */
	wlcompat_ioctl,		/* SIOCGIWFRAG */
	NULL,			/* SIOCSIWTXPOW */
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
