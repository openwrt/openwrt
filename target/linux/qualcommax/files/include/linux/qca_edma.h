/* SPDX-License-Identifier: GPL-2.0-or-later OR MIT */
/* The Qualcomm EDMA conduit and the PPE switch behind it. */

#ifndef __LINUX_QCA_EDMA_H
#define __LINUX_QCA_EDMA_H

#include <linux/netdevice.h>

struct dsa_switch;

/**
 * struct qca_edma_switch_ops - what the switch behind the conduit provides
 * @flow_xmit: send on a frame the switch routed to the CPU port on flow
 *	@flow, finished; returns false to leave it to the receive path
 * @vp_rx: take a frame the switch returned that was sent with the virtual
 *	source port @port, with skb->data at its Ethernet header; returns false
 *	when no such port exists, and the frame is dropped
 */
struct qca_edma_switch_ops {
	bool (*flow_xmit)(struct dsa_switch *ds, struct sk_buff *skb, u32 flow);
	bool (*vp_rx)(struct dsa_switch *ds, struct sk_buff *skb, u32 port);
};

int qca_edma_switch_register(struct net_device *conduit, struct dsa_switch *ds,
			     const struct qca_edma_switch_ops *ops);
void qca_edma_switch_unregister(struct net_device *conduit);
netdev_tx_t qca_edma_port_xmit(struct sk_buff *skb, struct net_device *conduit,
			       unsigned int port);
/* What qca_edma_src_xmit() did with a frame it took. */
#define QCA_EDMA_SRC_SENT	0
#define QCA_EDMA_SRC_PARKED	1
#define QCA_EDMA_SRC_DROPPED	2

int qca_edma_src_xmit(struct sk_buff *skb, struct net_device *conduit,
		      unsigned int port);

/**
 * struct qca_edma_src_stats - what the transmits that name a source port did
 * @parked: frames that waited for room
 * @drained: parked frames sent
 * @dropped: frames freed for want of room, or on the way down
 * @park_drv: parks with the queue stopped for its ring
 * @park_stack: parks with the queue stopped for its byte limit
 * @park_ring: parks with the queue running, the ring refusing the frame
 * @park_order: parks behind frames already waiting, with the queue running
 *
 * A park counts under every stop it found, so one with both stops set is in
 * @park_drv and @park_stack.
 */
struct qca_edma_src_stats {
	u64 parked;
	u64 drained;
	u64 dropped;
	u64 park_drv;
	u64 park_stack;
	u64 park_ring;
	u64 park_order;
};

void qca_edma_src_stats(struct net_device *conduit,
			struct qca_edma_src_stats *st);

#endif
