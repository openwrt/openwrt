/*
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * If building without CONFIG_NET_SCHED we need a simple
 * fifo qdisc to install by default as the sub-qdisc.
 * This is a simple replacement for sch_fifo.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "wme.h"

static int pfifo_enqueue(struct sk_buff *skb, struct Qdisc* qd)
{
	struct sk_buff_head *q = qdisc_priv(qd);

	if (skb_queue_len(q) > qd->dev->tx_queue_len) {
		qd->qstats.drops++;
		kfree_skb(skb);
		return NET_XMIT_DROP;
	}
	
	skb_queue_tail(q, skb);
	qd->q.qlen++;
	qd->bstats.bytes += skb->len;
	qd->bstats.packets++;
	
	return NET_XMIT_SUCCESS;
}


static int pfifo_requeue(struct sk_buff *skb, struct Qdisc* qd)
{
	struct sk_buff_head *q = qdisc_priv(qd);

	skb_queue_head(q, skb);
	qd->q.qlen++;
	qd->bstats.bytes += skb->len;
	qd->bstats.packets++;
	
	return NET_XMIT_SUCCESS;
}


static struct sk_buff *pfifo_dequeue(struct Qdisc* qd)
{
	struct sk_buff_head *q = qdisc_priv(qd);

	return skb_dequeue(q);
}


static int pfifo_init(struct Qdisc* qd, struct rtattr *opt)
{
	struct sk_buff_head *q = qdisc_priv(qd);

	skb_queue_head_init(q);
	return 0;
}


static void pfifo_reset(struct Qdisc* qd)
{
	struct sk_buff_head *q = qdisc_priv(qd);

	skb_queue_purge(q);
	qd->q.qlen = 0;
}


static int pfifo_dump(struct Qdisc *qd, struct sk_buff *skb)
{
	return skb->len;
}


struct Qdisc_ops pfifo_qdisc_ops =
{
	.next = NULL,
	.cl_ops = NULL,
	.id = "ieee80211_pfifo",
	.priv_size = sizeof(struct sk_buff_head),

	.enqueue = pfifo_enqueue,
	.dequeue = pfifo_dequeue,
	.requeue = pfifo_requeue,
	.drop = NULL,

	.init = pfifo_init,
	.reset = pfifo_reset,
	.destroy = NULL,
	.change = NULL,

	.dump = pfifo_dump,
};

