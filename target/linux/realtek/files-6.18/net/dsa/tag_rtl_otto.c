// SPDX-License-Identifier: GPL-2.0+

#include <linux/dsa/tag_rtl_otto.h>
#include <linux/etherdevice.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "tag.h"

#define RTL_OTTO_NAME		"rtl_otto"

static struct sk_buff *rtl_otto_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct dsa_port *dp = dsa_user_to_port(dev);
	u8 *trailer;

	/* The MAC driver consumes this software-only destination trailer. */
	trailer = skb_put(skb, RTL_OTTO_TAG_LEN);
	trailer[0] = RTL_OTTO_DEVICE_LOCAL;
	trailer[1] = dp->index;
	trailer[2] = 0xab;
	trailer[3] = 0xcd;
	trailer[4] = 0xef;

	return skb;
}

static struct sk_buff *rtl_otto_rcv(struct sk_buff *skb, struct net_device *dev)
{
	/* RX path uses METADATA_HW_PORT_MUX. This function just makes netdev_uses_dsa() happy. */
	netdev_err(dev, "ethernet driver did not set METADATA\n");

	return skb;
}

static int rtl_otto_connect(struct dsa_switch *ds)
{
	struct rtl_otto_tagger_data *tagger_data;

	tagger_data = kzalloc(sizeof(*tagger_data), GFP_KERNEL);
	if (!tagger_data)
		return -ENOMEM;

	rwlock_init(&tagger_data->cpu_device_lock);
	tagger_data->cpu_device_changing = true;
	ds->tagger_data = tagger_data;

	return 0;
}

static void rtl_otto_disconnect(struct dsa_switch *ds)
{
	kfree(ds->tagger_data);
	ds->tagger_data = NULL;
}

static const struct dsa_device_ops rtl_otto_netdev_ops = {
	.name			= RTL_OTTO_NAME,
	.proto			= DSA_TAG_PROTO_RTL_OTTO,
	.connect		= rtl_otto_connect,
	.disconnect		= rtl_otto_disconnect,
	.xmit			= rtl_otto_xmit,
	.rcv			= rtl_otto_rcv,
	.needed_tailroom	= RTL_OTTO_TAG_LEN,
};

MODULE_DESCRIPTION("DSA tag driver for Realtek Otto switches (RTL83xx/RTL93xx)");
MODULE_LICENSE("GPL");
MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_RTL_OTTO, RTL_OTTO_NAME);

module_dsa_tag_driver(rtl_otto_netdev_ops);
