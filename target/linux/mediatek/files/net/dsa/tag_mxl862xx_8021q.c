// SPDX-License-Identifier: GPL-2.0+
/*
 * net/dsa/tag_mxl862xx_8021q.c - DSA driver 802.1q based Special Tag support for MaxLinear 862xx switch chips
 *
 * Copyright (C) 2024 MaxLinear Inc.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <linux/version.h>
#include <linux/dsa/8021q.h>
#include "tag_8021q.h"
#include "tag.h"

#define MXL862_TAG_8021Q_NAME	"mxl862_8021q"

/* To define the outgoing port and to discover the incoming port
 * a special 4-byte outer VLAN tag is used by the MxL862xx.
 *
 *       Dest MAC       Src MAC    special   optional  EtherType
 *                                 outer     inner
 *                                 VLAN tag  tag(s)
 * ...| 1 2 3 4 5 6 | 1 2 3 4 5 6 | 1 2 3 4 | 1 2 3 4 | 1 2 |...
 *                                |<------->|
 */

/* special tag in TX path header */

static struct sk_buff *mxl862_8021q_tag_xmit(struct sk_buff *skb,
				      struct net_device *dev)
{
	struct dsa_port *dp = dsa_user_to_port(dev);

	u16 tx_vid = dsa_tag_8021q_standalone_vid(dp);
	u16 queue_mapping = skb_get_queue_mapping(skb);
	u8 pcp = netdev_txq_to_tc(dev, queue_mapping);
	skb_set_queue_mapping(skb, dp->index);

	dsa_8021q_xmit(skb, dev, ETH_P_8021Q,
			      ((pcp << VLAN_PRIO_SHIFT) | tx_vid));

	return skb;
}

static struct sk_buff *mxl862_8021q_tag_rcv(struct sk_buff *skb,
				      struct net_device *dev)
{
	int src_port = -1;
	int switch_id = -1;

	/* removes Outer VLAN tag */
	dsa_8021q_rcv(skb, &src_port, &switch_id, NULL, NULL);
	if (src_port == -1 || switch_id == -1) {
		dev_warn_ratelimited(&dev->dev, "Dropping packet due to invalid outer 802.1Q tag: switch %d port %d\n", switch_id, src_port);
		return NULL;
	}

	skb->dev = dsa_conduit_find_user(dev, switch_id, src_port);
	if (!skb->dev) {
		dev_warn_ratelimited(&dev->dev, "Dropping packet due to invalid source port: %d\n", src_port);
		return NULL;
	}

	dsa_default_offload_fwd_mark(skb);

	return skb;
}

static const struct dsa_device_ops mxl862_8021q_netdev_ops = {
	.name = MXL862_TAG_8021Q_NAME,
	.proto = DSA_TAG_PROTO_MXL862_8021Q,
	.xmit = mxl862_8021q_tag_xmit,
	.rcv = mxl862_8021q_tag_rcv,
	.needed_headroom	= VLAN_HLEN,
	.promisc_on_conduit	= true,
};

MODULE_LICENSE("GPL");
MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_MXL862_8021Q, MXL862_TAG_8021Q_NAME);
module_dsa_tag_driver(mxl862_8021q_netdev_ops);
