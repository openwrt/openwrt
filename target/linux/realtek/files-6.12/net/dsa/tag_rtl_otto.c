// SPDX-License-Identifier: GPL-2.0+
/*
 * net/dsa/tag_trailer.c - Trailer tag format handling
 * Copyright (c) 2008-2009 Marvell Semiconductor
 */

#include <linux/etherdevice.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "tag.h"

#define RTL_OTTO_NAME "rtl_otto"

/*
 * TODO: This driver was copied over from trailer tagging. It will be developed
 * downstream in OpenWrt in conjunction with the Realtek Otto ethernet driver.
 * For now rely on the old trailer handling and keep everything as is.
 */

static struct sk_buff *rtl_otto_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct dsa_port *dp = dsa_user_to_port(dev);
	u8 *trailer;

	trailer = skb_put(skb, 4);
	trailer[0] = 0x80;
	trailer[1] = dp->index;
	trailer[2] = 0x10;
	trailer[3] = 0x00;

	return skb;
}

static struct sk_buff *rtl_otto_rcv(struct sk_buff *skb, struct net_device *dev)
{
	u8 *trailer;
	int source_port;

	if (skb_linearize(skb))
		return NULL;

	trailer = skb_tail_pointer(skb) - 4;

	if (trailer[0] != 0x80 || (trailer[1] & 0x80) != 0x00 ||
	    (trailer[2] & 0xef) != 0x00 || trailer[3] != 0x00)
		return NULL;

	if (trailer[1] & 0x40)
		skb->offload_fwd_mark = 1;

	source_port = trailer[1] & 0x3f;

	skb->dev = dsa_conduit_find_user(dev, 0, source_port);
	if (!skb->dev)
		return NULL;

	if (pskb_trim_rcsum(skb, skb->len - 4))
		return NULL;

	return skb;
}

static const struct dsa_device_ops rtl_otto_netdev_ops = {
	.name	= RTL_OTTO_NAME,
	.proto	= DSA_TAG_PROTO_RTL_OTTO,
	.xmit	= rtl_otto_xmit,
	.rcv	= rtl_otto_rcv,
	.needed_tailroom = 4,
};

MODULE_DESCRIPTION("DSA tag driver for Realtek Otto switches (RTL83xx/RTL93xx)");
MODULE_LICENSE("GPL");
MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_RTL_OTTO, RTL_OTTO_NAME);

module_dsa_tag_driver(rtl_otto_netdev_ops);
