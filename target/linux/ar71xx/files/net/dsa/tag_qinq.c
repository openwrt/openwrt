/*
 * net/dsa/tag_qinq.c - QinQ tag format handling
 * Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file was based on:
 *    net/dsa/tag_edsa.c - Ethertype DSA tagging
 *    Copyright (c) 2008-2009 Marvell Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/etherdevice.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/if_vlan.h>

#include "dsa_priv.h"

netdev_tx_t qinq_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct dsa_slave_priv *p = netdev_priv(dev);
	struct vlan_ethhdr *veth;
	unsigned int len;
	int ret;

	if (skb_cow_head(skb, VLAN_HLEN) < 0)
		goto out_free_skb;

	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);
	skb->mac_header -= VLAN_HLEN;

	/* setup VLAN header fields */
	veth->h_vlan_proto = htons(ETH_P_QINQ);
	veth->h_vlan_TCI = htons(p->port);

	len = skb->len;
	skb->protocol = htons(ETH_P_QINQ);
	skb->dev = p->parent->dst->master_netdev;

	ret = dev_queue_xmit(skb);
	if (unlikely(ret != NET_XMIT_SUCCESS))
		goto out_dropped;

	dev->stats.tx_packets++;
	dev->stats.tx_bytes += len;

	return NETDEV_TX_OK;

 out_free_skb:
	kfree_skb(skb);
 out_dropped:
	dev->stats.tx_dropped++;
	return NETDEV_TX_OK;
}

static int qinq_rcv(struct sk_buff *skb, struct net_device *dev,
		    struct packet_type *pt, struct net_device *orig_dev)
{
	struct dsa_switch_tree *dst;
	struct dsa_switch *ds;
	struct vlan_hdr *vhdr;
	int source_port;

	dst = dev->dsa_ptr;
	if (unlikely(dst == NULL))
		goto out_drop;
	ds = dst->ds[0];

	skb = skb_unshare(skb, GFP_ATOMIC);
	if (skb == NULL)
		goto out;

	if (unlikely(!pskb_may_pull(skb, VLAN_HLEN)))
		goto out_drop;

	vhdr = (struct vlan_hdr *)skb->data;
	source_port = ntohs(vhdr->h_vlan_TCI) & VLAN_VID_MASK;
	if (source_port >= DSA_MAX_PORTS || ds->ports[source_port] == NULL)
		goto out_drop;

	/* Remove the outermost VLAN tag and update checksum. */
	skb_pull_rcsum(skb, VLAN_HLEN);
	memmove(skb->data - ETH_HLEN,
		skb->data - ETH_HLEN - VLAN_HLEN,
		2 * ETH_ALEN);

	skb->dev = ds->ports[source_port];
	skb_push(skb, ETH_HLEN);
	skb->pkt_type = PACKET_HOST;
	skb->protocol = eth_type_trans(skb, skb->dev);

	skb->dev->stats.rx_packets++;
	skb->dev->stats.rx_bytes += skb->len;

	netif_receive_skb(skb);

	return 0;

 out_drop:
	kfree_skb(skb);
 out:
	return 0;
}

static struct packet_type qinq_packet_type __read_mostly = {
	.type	= cpu_to_be16(ETH_P_QINQ),
	.func	= qinq_rcv,
};

static int __init qinq_init_module(void)
{
	dev_add_pack(&qinq_packet_type);
	return 0;
}
module_init(qinq_init_module);

static void __exit qinq_cleanup_module(void)
{
	dev_remove_pack(&qinq_packet_type);
}
module_exit(qinq_cleanup_module);
