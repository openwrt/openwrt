/*
 * Copyright 2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <net/ip.h>

#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "wme.h"

#define CHILD_QDISC_OPS pfifo_qdisc_ops

static inline int WLAN_FC_IS_QOS_DATA(u16 fc)
{
	return (fc & 0x8C) == 0x88;
}


ieee80211_txrx_result
ieee80211_rx_h_parse_qos(struct ieee80211_txrx_data *rx)
{
	u8 *data = rx->skb->data;
	int tid;

	/* does the frame have a qos control field? */
	if (WLAN_FC_IS_QOS_DATA(rx->fc)) {
		u8 *qc = data + ieee80211_get_hdrlen(rx->fc) - QOS_CONTROL_LEN;
		/* frame has qos control */
		tid = qc[0] & QOS_CONTROL_TID_MASK;
	} else {
		if (unlikely(WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_MGMT)) {
			/* Separate TID for management frames */
			tid = NUM_RX_DATA_QUEUES - 1;
		} else {
			/* no qos control present */
			tid = 0; /* 802.1d - Best Effort */
		}
	}
#ifdef CONFIG_IEEE80211_DEBUG_COUNTERS
	I802_DEBUG_INC(rx->local->wme_rx_queue[tid]);
	if (rx->sta) {
		I802_DEBUG_INC(rx->sta->wme_rx_queue[tid]);
	}
#endif /* CONFIG_IEEE80211_DEBUG_COUNTERS */

	rx->u.rx.queue = tid;
	/* Set skb->priority to 1d tag if highest order bit of TID is not set.
	 * For now, set skb->priority to 0 for other cases. */
	rx->skb->priority = (tid > 7) ? 0 : tid;

	return TXRX_CONTINUE;
}


ieee80211_txrx_result
ieee80211_rx_h_remove_qos_control(struct ieee80211_txrx_data *rx)
{
	u16 fc = rx->fc;
	u8 *data = rx->skb->data;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) data;

	if (!WLAN_FC_IS_QOS_DATA(fc))
		return TXRX_CONTINUE;

	/* remove the qos control field, update frame type and meta-data */
	memmove(data + 2, data, ieee80211_get_hdrlen(fc) - 2);
	hdr = (struct ieee80211_hdr *) skb_pull(rx->skb, 2);
	/* change frame type to non QOS */
	rx->fc = fc &= ~(WLAN_FC_STYPE_QOS_DATA << 4);
	hdr->frame_control = cpu_to_le16(fc);

	return TXRX_CONTINUE;
}


/* maximum number of hardware queues we support. */
#define TC_80211_MAX_QUEUES 8

struct ieee80211_sched_data
{
	struct tcf_proto *filter_list;
	struct Qdisc *queues[TC_80211_MAX_QUEUES];
	struct sk_buff_head requeued[TC_80211_MAX_QUEUES];
};


/* given a data frame determine the 802.1p/1d tag to use */
static inline unsigned classify_1d(struct sk_buff *skb, struct Qdisc *qd)
{
	struct iphdr *ip;
	int dscp;
	int offset;

#ifdef CONFIG_NET_SCHED
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct tcf_result res = { -1, 0 };

	/* if there is a user set filter list, call out to that */
	if (q->filter_list) {
		tc_classify(skb, q->filter_list, &res);
		if (res.class != -1)
			return res.class;
	}
#endif /* CONFIG_NET_SCHED */

	/* skb->priority values from 256->263 are magic values to
	 * directly indicate a specific 802.1d priority.
	 * This is used to allow 802.1d priority to be passed directly in
	 * from VLAN tags, etc. */
	if (skb->priority >= 256 && skb->priority <= 263)
		return skb->priority - 256;
	
	/* check there is a valid IP header present */
	offset = ieee80211_get_hdrlen_from_skb(skb) + 8 /* LLC + proto */;
	if (skb->protocol != __constant_htons(ETH_P_IP) ||
	    skb->len < offset + sizeof(*ip))
		return 0;

	ip = (struct iphdr *) (skb->data + offset);

	dscp = ip->tos & 0xfc;
	switch (dscp) {
	case 0x20:
		return 2;
	case 0x40:
		return 1;
	case 0x60:
		return 3;
	case 0x80:
		return 4;
	case 0xa0:
		return 5;
	case 0xc0:
		return 6;
	case 0xe0:
		return 7;
	default:
		return 0;
	}
}




static inline int wme_downgrade_ac(struct sk_buff *skb)
{
	switch (skb->priority) {
	case 6:
	case 7:
		skb->priority = 5; /* VO -> VI */
		return 0;
	case 4:
	case 5:
		skb->priority = 3; /* VI -> BE */
		return 0;
	case 0:
	case 3:
		skb->priority = 2; /* BE -> BK */
		return 0;
	default:
		return -1;
	}
}


/* positive return value indicates which queue to use 
 * negative return value indicates to drop the frame */
static inline int classify80211(struct sk_buff *skb, struct Qdisc *qd)
{
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_tx_packet_data *pkt_data =
		(struct ieee80211_tx_packet_data *) skb->cb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	unsigned short fc = le16_to_cpu(hdr->frame_control);
	int qos;
	const int ieee802_1d_to_ac[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };
	
	if (unlikely(pkt_data->magic != IEEE80211_CB_MAGIC))
		return -1;

	/* see if frame is data or non data frame */
	if (unlikely(WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_DATA)) {
		/* management frames go on AC_VO queue, but are sent
		* without QoS control fields */
		return IEEE80211_TX_QUEUE_DATA0;
	}
	
	if (unlikely(pkt_data->sdata->type == IEEE80211_SUB_IF_TYPE_MGMT)) {
		/* Data frames from hostapd (mainly, EAPOL) use AC_VO
		* and they will include QoS control fields if
		* the target STA is using WME. */
		skb->priority = 7;
		return ieee802_1d_to_ac[skb->priority];
	}
	

	/* is this a QoS frame? */
	qos = fc & (WLAN_FC_STYPE_QOS_DATA << 4);
	

	if (!qos) {
		skb->priority = 0; /* required for correct WPA/11i MIC */
		return ieee802_1d_to_ac[skb->priority];
	}
	
	/* use the data classifier to determine what 802.1d tag the
	* data frame has */
	skb->priority = classify_1d(skb, qd);
	

	/* incase we are a client verify acm is not set for this ac */
	for (; unlikely(local->wmm_acm & BIT(skb->priority)); )
	{
		if (wme_downgrade_ac(skb)) {
			/* No AC with lower priority has acm=0,
			* drop packet. */
			return -1;
		}
	}
	
	/* look up which queue to use for frames with this 1d tag */
	return ieee802_1d_to_ac[skb->priority];
}


static int wme_qdiscop_enqueue(struct sk_buff *skb, struct Qdisc* qd)
{
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_tx_packet_data *pkt_data =
		(struct ieee80211_tx_packet_data *) skb->cb;
	struct ieee80211_tx_control *control = &pkt_data->control;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	unsigned short fc = le16_to_cpu(hdr->frame_control);
	struct Qdisc *qdisc;
	int err, queue;

	if (control->requeue) {
		skb_queue_tail(&q->requeued[control->queue], skb);
		return 0;
	}

	queue = classify80211(skb, qd);
	
	/* now we know the 1d priority, fill in the QoS header if there is one */
	if (WLAN_FC_IS_QOS_DATA(fc)) {
		struct qos_control *qc = (struct qos_control *) (skb->data + ieee80211_get_hdrlen(fc) - 2);
		u8 *p = (u8 *) qc;
		*p++ = 0; /* do this due to gcc's lack of optimization on bitfield ops */
		*p = 0;
		qc->tag1d = skb->priority;
		if (local->wifi_wme_noack_test)
			qc->ack_policy = 1;
	}

	if (unlikely(queue >= local->hw->queues)) {
#if 0
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s - queue=%d (hw does not "
			       "support) -> %d\n",
			       __func__, queue, local->hw->queues - 1);
		}
#endif
		queue = local->hw->queues - 1;
	}
	
	if (unlikely(queue < 0)) {
			kfree_skb(skb);
			err = NET_XMIT_DROP;
	} else {
		control->queue = (unsigned int) queue;
		qdisc = q->queues[queue];
		err = qdisc->enqueue(skb, qdisc);
		if (err == NET_XMIT_SUCCESS) {
			qd->q.qlen++;
			qd->bstats.bytes += skb->len;
			qd->bstats.packets++;
			return NET_XMIT_SUCCESS;
		}
	}
	qd->qstats.drops++;
	return err;
}


/* TODO: clean up the cases where master_hard_start_xmit
 * returns non 0 - it shouldn't ever do that. Once done we 
 * can remove this function */
static int wme_qdiscop_requeue(struct sk_buff *skb, struct Qdisc* qd)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_tx_packet_data *pkt_data =
		(struct ieee80211_tx_packet_data *) skb->cb;
	struct Qdisc *qdisc;
	int err;

	/* we recorded which queue to use earlier! */
	qdisc = q->queues[pkt_data->control.queue];

	if ((err = qdisc->ops->requeue(skb, qdisc)) == 0) {
		qd->q.qlen++;
		return 0;
	}
	qd->qstats.drops++;
	return err;
}


static struct sk_buff *wme_qdiscop_dequeue(struct Qdisc* qd)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct net_device *dev = qd->dev;
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hw *hw = local->hw;
	struct ieee80211_tx_queue_stats stats;
	struct sk_buff *skb;
	struct Qdisc *qdisc;
	int queue;

	/* find which hardware queues have space in them */
	hw->get_tx_stats(dev, &stats);

	/* check all the h/w queues in numeric/priority order */
	for (queue = 0; queue < hw->queues; queue++) {
		/* see if there is room in this hardware queue */
		if (stats.data[queue].len >= stats.data[queue].limit)
			continue;

		/* there is space - try and get a frame */
		skb = skb_dequeue(&q->requeued[queue]);
		if (skb)
			return skb;

		qdisc = q->queues[queue];
		skb = qdisc->dequeue(qdisc);
		if (skb) {
			qd->q.qlen--;
			return skb;
		}
	}
	/* returning a NULL here when all the h/w queues are full means we
	 * never need to call netif_stop_queue in the driver */
	return NULL;
}


static void wme_qdiscop_reset(struct Qdisc* qd)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	int queue;

	/* QUESTION: should we have some hardware flush functionality here? */

	for (queue = 0; queue < hw->queues; queue++) {
		skb_queue_purge(&q->requeued[queue]);
		qdisc_reset(q->queues[queue]);
	}
	qd->q.qlen = 0;
}


static void wme_qdiscop_destroy(struct Qdisc* qd)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	struct tcf_proto *tp;
	int queue;

	while ((tp = q->filter_list) != NULL) {
		q->filter_list = tp->next;
		tp->ops->destroy(tp);
	}

	for (queue=0; queue < hw->queues; queue++) {
		skb_queue_purge(&q->requeued[queue]);
		qdisc_destroy(q->queues[queue]);
		q->queues[queue] = &noop_qdisc;
	}
}


/* called whenever parameters are updated on existing qdisc */
static int wme_qdiscop_tune(struct Qdisc *qd, struct rtattr *opt)
{
/*	struct ieee80211_sched_data *q = qdisc_priv(qd);
*/
	/* check our options block is the right size */
	/* copy any options to our local structure */
/*	Ignore options block for now - always use static mapping
	struct tc_ieee80211_qopt *qopt = RTA_DATA(opt);
	
	if (opt->rta_len < RTA_LENGTH(sizeof(*qopt)))
		return -EINVAL;
	memcpy(q->tag2queue, qopt->tag2queue, sizeof(qopt->tag2queue));
*/
	return 0;
}


/* called during initial creation of qdisc on device */
static int wme_qdiscop_init(struct Qdisc *qd, struct rtattr *opt)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct net_device *dev = qd->dev;
	struct ieee80211_local *local = dev->priv;
	int queues = local->hw->queues;
	int err = 0, i;
	
	/* check this device is an ieee80211 master type device */
	if (dev->type != ARPHRD_IEEE80211)
		return -EINVAL;

	/* check that there is no qdisc currently attached to device
	 * this ensures that we will be the root qdisc. (I can't find a better
	 * way to test this explicitly) */
	if (dev->qdisc_sleeping != &noop_qdisc)
		return -EINVAL;

	if (qd->flags & TCQ_F_INGRESS)
		return -EINVAL;

	/* if options were passed in, set them */
	if (opt) {
		err = wme_qdiscop_tune(qd, opt);
	}

	/* create child queues */
	for (i = 0; i < queues; i++) {
		skb_queue_head_init(&q->requeued[i]);
		q->queues[i] = qdisc_create_dflt(qd->dev, &CHILD_QDISC_OPS);
		if (q->queues[i] == 0) {
			q->queues[i] = &noop_qdisc;
			printk(KERN_ERR "%s child qdisc %i creation failed", dev->name, i);
		}
	}

	return err;
}

static int wme_qdiscop_dump(struct Qdisc *qd, struct sk_buff *skb)
{
/*	struct ieee80211_sched_data *q = qdisc_priv(qd);
	unsigned char *p = skb->tail;
	struct tc_ieee80211_qopt opt;

	memcpy(&opt.tag2queue, q->tag2queue, TC_80211_MAX_TAG + 1);
	RTA_PUT(skb, TCA_OPTIONS, sizeof(opt), &opt);
*/	return skb->len;
/*
rtattr_failure:
	skb_trim(skb, p - skb->data);*/
	return -1;
}


static int wme_classop_graft(struct Qdisc *qd, unsigned long arg,
			     struct Qdisc *new, struct Qdisc **old)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	unsigned long queue = arg - 1;

	if (queue >= hw->queues)
		return -EINVAL;

	if (new == NULL)
		new = &noop_qdisc;

	sch_tree_lock(qd);
	*old = q->queues[queue];
	q->queues[queue] = new;
	qdisc_reset(*old);
	sch_tree_unlock(qd);

	return 0;
}


static struct Qdisc *
wme_classop_leaf(struct Qdisc *qd, unsigned long arg)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	unsigned long queue = arg - 1;

	if (queue >= hw->queues)
		return NULL;

	return q->queues[queue];
}


static unsigned long wme_classop_get(struct Qdisc *qd, u32 classid)
{
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	unsigned long queue = TC_H_MIN(classid);

	if (queue - 1 >= hw->queues)
		return 0;

	return queue;
}


static unsigned long wme_classop_bind(struct Qdisc *qd, unsigned long parent,
				      u32 classid)
{
	return wme_classop_get(qd, classid);
}


static void wme_classop_put(struct Qdisc *q, unsigned long cl)
{
	/* printk(KERN_DEBUG "entering %s\n", __func__); */
}


static int wme_classop_change(struct Qdisc *qd, u32 handle, u32 parent,
			      struct rtattr **tca, unsigned long *arg)
{
	unsigned long cl = *arg;
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	/* printk(KERN_DEBUG "entering %s\n", __func__); */

	if (cl - 1 > hw->queues)
		return -ENOENT;

	/* TODO: put code to program hardware queue parameters here,
	 * to allow programming from tc command line */

	return 0;
}


/* we don't support deleting hardware queues
 * when we add WMM-SA support - TSPECs may be deleted here */
static int wme_classop_delete(struct Qdisc *qd, unsigned long cl)
{
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	/* printk(KERN_DEBUG "entering %s\n", __func__); */

	if (cl - 1 > hw->queues)
		return -ENOENT;
	return 0;
}


static int wme_classop_dump_class(struct Qdisc *qd, unsigned long cl,
				  struct sk_buff *skb, struct tcmsg *tcm)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	/* printk(KERN_DEBUG "entering %s\n", __func__); */

	if (cl - 1 > hw->queues)
		return -ENOENT;
	tcm->tcm_handle = TC_H_MIN(cl);
	tcm->tcm_parent = qd->handle;
	tcm->tcm_info = q->queues[cl-1]->handle; /* do we need this? */
	return 0;
}


static void wme_classop_walk(struct Qdisc *qd, struct qdisc_walker *arg)
{
	struct ieee80211_local *local = qd->dev->priv;
	struct ieee80211_hw *hw = local->hw;
	int queue;
	/* printk(KERN_DEBUG "entering %s\n", __func__); */

	if (arg->stop)
		return;

	for (queue = 0; queue < hw->queues; queue++) {
		if (arg->count < arg->skip) {
			arg->count++;
			continue;
		}
		/* we should return classids for our internal queues here
		 * as well as the external ones */
		if (arg->fn(qd, queue+1, arg) < 0) {
			arg->stop = 1;
			break;
		}
		arg->count++;
	}
}


static struct tcf_proto ** wme_classop_find_tcf(struct Qdisc *qd,
						unsigned long cl)
{
	struct ieee80211_sched_data *q = qdisc_priv(qd);
	/* printk("entering %s\n", __func__); */

	if (cl)
		return NULL;

	return &q->filter_list;
}


/* this qdisc is classful (i.e. has classes, some of which may have leaf qdiscs attached)
 * - these are the operations on the classes */
static struct Qdisc_class_ops class_ops =
{
	.graft = wme_classop_graft,
	.leaf = wme_classop_leaf,

	.get = wme_classop_get,
	.put = wme_classop_put,
	.change = wme_classop_change,
	.delete = wme_classop_delete,
	.walk = wme_classop_walk,

	.tcf_chain = wme_classop_find_tcf,
	.bind_tcf = wme_classop_bind,
	.unbind_tcf = wme_classop_put,

	.dump = wme_classop_dump_class,
};


/* queueing discipline operations */
static struct Qdisc_ops wme_qdisc_ops =
{
	.next = NULL,
	.cl_ops = &class_ops,
	.id = "ieee80211",
	.priv_size = sizeof(struct ieee80211_sched_data),

	.enqueue = wme_qdiscop_enqueue,
	.dequeue = wme_qdiscop_dequeue,
	.requeue = wme_qdiscop_requeue,
	.drop = NULL, /* drop not needed since we are always the root qdisc */

	.init = wme_qdiscop_init,
	.reset = wme_qdiscop_reset,
	.destroy = wme_qdiscop_destroy,
	.change = wme_qdiscop_tune,

	.dump = wme_qdiscop_dump,
};


void ieee80211_install_qdisc(struct net_device *dev)
{
	struct Qdisc *qdisc;

	qdisc = qdisc_create_dflt(dev, &wme_qdisc_ops);
	if (qdisc == NULL) {
		printk(KERN_ERR "%s: qdisc installation failed\n", dev->name);
		return;
	}
	
	/* same handle as would be allocated by qdisc_alloc_handle() */
	qdisc->handle = 0x80010000;

	write_lock(&qdisc_tree_lock);
	list_add_tail(&qdisc->list, &dev->qdisc_list);
	dev->qdisc_sleeping = qdisc;
	write_unlock(&qdisc_tree_lock);
}


int ieee80211_wme_register(void)
{
	int err = 0;

#ifdef CONFIG_NET_SCHED
	err = register_qdisc(&wme_qdisc_ops);
#endif
	return err;
}


void ieee80211_wme_unregister(void)
{
#ifdef CONFIG_NET_SCHED
	unregister_qdisc(&wme_qdisc_ops);
#endif
}
