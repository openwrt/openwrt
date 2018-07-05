/*
 *  Realtek RTL865X-style built-in ethernet mac driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Based on Realtek RTL865X asic driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "re865x.h"
#include <asm/c-lexra.h>
#include <linux/icmp.h>
#include <linux/proc_fs.h>

#define DBG(fmt, args...)	pr_info(fmt, ## args)

#define RE865X_DEFAULT_MSG_ENABLE	\
	(NETIF_MSG_DRV			\
	| NETIF_MSG_PROBE		\
	| NETIF_MSG_LINK		\
	| NETIF_MSG_TIMER		\
	| NETIF_MSG_IFDOWN		\
	| NETIF_MSG_IFUP		\
	| NETIF_MSG_RX_ERR		\
	| NETIF_MSG_TX_ERR)

static int re865x_msg_level = -1;

module_param_named(msg_level, re865x_msg_level, int, 0);
MODULE_PARM_DESC(msg_level, "Message level (-1=defaults,0=none,...,16=all)");

static void re865x_ring_free(struct re865x_ring *ring)
{
	if (ring->pkthdr_descs_cpu)
		dma_free_coherent(NULL, ring->num_descs * MBUF_DESC_SIZE,
				  ring->pkthdr_descs_cpu,
				  ring->pkthdr_descs_dma);

	if (ring->mbuf_descs_cpu)
		dma_free_coherent(NULL, ring->num_descs * MBUF_DESC_SIZE,
				  ring->mbuf_descs_cpu, ring->mbuf_descs_dma);

	if (ring->pkdhdrs_cpu)
		dma_free_coherent(NULL, ring->num_descs * MBUF_DESC_SIZE,
				  ring->pkdhdrs_cpu, ring->pkdhdrs_dma);

	if (ring->mbufs_cpu)
		dma_free_coherent(NULL, ring->num_descs * MBUF_DESC_SIZE,
				  ring->mbufs_cpu, ring->mbufs_dma);

	if (ring->pkthdrs)
		kfree(ring->pkthdrs);

	if (ring->mbufs)
		kfree(ring->mbufs);
}

static int re865x_ring_alloc(struct re865x_ring *ring, bool is_rx)
{
	int err;

	ring->pkthdr_descs_cpu = dma_alloc_coherent(NULL,
					ring->num_descs * sizeof (u32),
					&ring->pkthdr_descs_dma, GFP_ATOMIC);
	if (!ring->pkthdr_descs_cpu) {
		err = -ENOMEM;
		goto err;
	}

	if (is_rx) {
		ring->mbuf_descs_cpu = dma_alloc_coherent(NULL,
					ring->num_descs * sizeof (u32),
					&ring->mbuf_descs_dma, GFP_ATOMIC);
		if (!ring->mbuf_descs_cpu) {
			err = -ENOMEM;
			goto err;
		}
	} else {
		ring->mbuf_descs_cpu = NULL;
	}

	ring->pkdhdrs_cpu = dma_alloc_coherent(NULL,
					ring->num_descs * PKDHDR_DESC_SIZE,
					&ring->pkdhdrs_dma, GFP_ATOMIC);
	if (!ring->pkdhdrs_cpu) {
		err = -ENOMEM;
		goto err;
	}

	ring->mbufs_cpu = dma_alloc_coherent(NULL,
					ring->num_descs * MBUF_DESC_SIZE,
					&ring->mbufs_dma, GFP_ATOMIC);
	if (!ring->mbufs_cpu) {
		err = -ENOMEM;
		goto err;
	}

	ring->pkthdrs = kzalloc(ring->num_descs * sizeof(*ring->pkthdrs),
				GFP_KERNEL);
	if (!ring->pkthdrs) {
		err = -ENOMEM;
		goto err;
	}

	ring->mbufs = kzalloc(ring->num_descs * sizeof(*ring->mbufs),
			      GFP_KERNEL);
	if (!ring->mbufs) {
		err = -ENOMEM;
		goto err;
	}

	return 0;

err:
	if (ring->mbufs)
		kfree(ring->mbufs);

	if (ring->pkthdrs)
		kfree(ring->pkthdrs);

	if (ring->mbufs_cpu)
		dma_free_coherent(NULL, ring->num_descs * MBUF_DESC_SIZE,
				  ring->mbufs_cpu, ring->mbufs_dma);

	if (ring->pkdhdrs_cpu)
		dma_free_coherent(NULL, ring->num_descs * PKDHDR_DESC_SIZE,
				  ring->pkdhdrs_cpu, ring->pkdhdrs_dma);

	if (ring->mbuf_descs_cpu)
		dma_free_coherent(NULL, ring->num_descs * sizeof (u32),
				  ring->mbuf_descs_cpu, ring->mbuf_descs_dma);

	if (ring->pkthdr_descs_cpu)
		dma_free_coherent(NULL, ring->num_descs * sizeof (u32),
				  ring->pkthdr_descs_cpu,
				  ring->pkthdr_descs_dma);

	return err;
}

static void re865x_ring_tx_init(struct re865x *re, struct re865x_ring *ring,
				bool is_dummy)
{
	struct net_device *dev = re->dev;
	u8 *pkthdrs_p;
	u8 *mbufs_p;
	dma_addr_t pkthdrs_dma_p;
	dma_addr_t mbufs_dma_p;
	unsigned int i;

	pkthdrs_p = ring->pkdhdrs_cpu;
	mbufs_p = ring->mbufs_cpu;

	pkthdrs_dma_p = ring->pkdhdrs_dma;
	mbufs_dma_p = ring->mbufs_dma;

	for (i = 0; i < ring->num_descs; i++) {
		ring->pkthdrs[i] = (struct pktHdr *) pkthdrs_p;
		ring->mbufs[i] = (struct mBuf *) mbufs_p;

		ring->pkthdrs[i]->ph_mbuf = mbufs_dma_p;
		ring->pkthdrs[i]->ph_len = 0;
		ring->pkthdrs[i]->ph_flags = PKTHDR_FLAG_USED | PKTHDR_FLAG_PKT_OUTGOING;
		ring->pkthdrs[i]->ph_type = PKTHDR_TYPE_ETHERNET;
		ring->pkthdrs[i]->ph_portlist = ALL_PORT_MASK;

		ring->mbufs[i]->m_pkthdr = pkthdrs_dma_p;
		ring->mbufs[i]->m_next = 0;
		ring->mbufs[i]->m_flags = MBUF_FLAG_USED | MBUF_FLAG_EXT | MBUF_FLAG_PKTHDR | MBUF_FLAG_EOR;
		ring->mbufs[i]->m_extsize = RE865X_MBUF_CLUSTER_SIZE;
		ring->mbufs[i]->m_data = ring->mbufs[i]->m_extbuf = 0;

		ring->pkthdr_descs_cpu[i] = pkthdrs_dma_p;

		ring->mbufs[i]->tx.skb = NULL;

		pkthdrs_p += PKDHDR_DESC_SIZE;
		mbufs_p += MBUF_DESC_SIZE;
		pkthdrs_dma_p += PKDHDR_DESC_SIZE;
		mbufs_dma_p += MBUF_DESC_SIZE;

		re865x_desc_clear_own(&ring->pkthdr_descs_cpu[i]);
	}

	/* set wrap */
	ring->pkthdr_descs_cpu[ring->num_descs - 1] |= NIC_DESC_WRAP;

	/* flush descriptors */
	wmb();

	ring->top = 0;
	ring->bottom = 0;

	netdev_reset_queue(dev);
}

static void re865x_ring_tx_clean(struct re865x *re)
{
	struct re865x_ring *ring = &re->tx_ring;
	struct net_device *dev = re->dev;
	u32 bytes_compl = 0, pkts_compl = 0;
	unsigned int index;

	for (; ring->bottom < ring->top; ring->bottom++) {
		index = re865x_ring_bottom_to_index(ring);
		if (re865x_desc_is_owned(ring->pkthdr_descs_cpu[index])) {
			re865x_desc_clear_own(&ring->pkthdr_descs_cpu[index]);
			dev->stats.tx_errors++;
		}

		if (ring->mbufs[index]->tx.skb) {
			bytes_compl += ring->mbufs[index]->tx.skb->len;
			pkts_compl++;
			dev_kfree_skb_any(ring->mbufs[index]->tx.skb);
		}

		ring->mbufs[index]->tx.skb = NULL;
	}

	/* flush descriptors */
	wmb();

	netdev_completed_queue(dev, pkts_compl, bytes_compl);
}

static bool re865x_fill_rx_buf(struct re865x *re, unsigned int index,
			int offset, void *(*alloc)(unsigned int size))
{
	struct re865x_ring *ring = &re->rx_ring;
	void *data;
	int mbuf_idx;

	data = alloc(re865x_buffer_size(re));
	if (!data)
		return false;

	mbuf_idx = (ring->pkthdrs[index]->ph_mbuf - ring->mbufs_dma) / MBUF_DESC_SIZE;

	ring->mbufs[mbuf_idx]->rx.buf = data;
	ring->mbufs[mbuf_idx]->rx.dma_addr = dma_map_single(&re->dev->dev, data, re->rx_buf_size,
						DMA_FROM_DEVICE);

	ring->mbufs[mbuf_idx]->m_data = ring->mbufs[mbuf_idx]->m_extbuf = (u32) ring->mbufs[mbuf_idx]->rx.dma_addr + offset;

	re865x_desc_set_own(&ring->pkthdr_descs_cpu[index]);
	re865x_desc_set_own(&ring->mbuf_descs_cpu[mbuf_idx]);

	return true;
}

static int re865x_ring_rx_init(struct re865x *re, struct re865x_ring *ring)
{
	u8 *pkthdrs_p;
	u8 *mbufs_p;
	dma_addr_t pkthdrs_dma_p;
	dma_addr_t mbufs_dma_p;
	unsigned int i;
	int ret = 0;

	pkthdrs_p = ring->pkdhdrs_cpu;
	mbufs_p = ring->mbufs_cpu;

	pkthdrs_dma_p = ring->pkdhdrs_dma;
	mbufs_dma_p = ring->mbufs_dma;

	for (i = 0; i < ring->num_descs; i++) {
		ring->pkthdrs[i] = (struct pktHdr *) pkthdrs_p;
		ring->mbufs[i] = (struct mBuf *) mbufs_p;

		ring->pkthdrs[i]->ph_mbuf = mbufs_dma_p;
		ring->pkthdrs[i]->ph_len = 0;
		ring->pkthdrs[i]->ph_flags = PKTHDR_FLAG_USED | PKTHDR_FLAG_PKT_INCOMING;
		ring->pkthdrs[i]->ph_type = PKTHDR_TYPE_ETHERNET;
		ring->pkthdrs[i]->ph_portlist = 0;

		ring->mbufs[i]->m_pkthdr = pkthdrs_dma_p;
		ring->mbufs[i]->m_next = 0;
		ring->mbufs[i]->m_flags = MBUF_FLAG_USED | MBUF_FLAG_EXT | MBUF_FLAG_PKTHDR | MBUF_FLAG_EOR;
		ring->mbufs[i]->m_extsize = RE865X_MBUF_CLUSTER_SIZE;
		ring->mbufs[i]->m_data = ring->mbufs[i]->m_extbuf = 0;

		ring->pkthdr_descs_cpu[i] = pkthdrs_dma_p;
		ring->mbuf_descs_cpu[i] = mbufs_dma_p;

		ring->mbufs[i]->rx.buf = NULL;
		ring->mbufs[i]->rx.dma_addr = 0;

		pkthdrs_p += PKDHDR_DESC_SIZE;
		mbufs_p += MBUF_DESC_SIZE;
		pkthdrs_dma_p += PKDHDR_DESC_SIZE;
		mbufs_dma_p += MBUF_DESC_SIZE;

		re865x_desc_clear_own(&ring->pkthdr_descs_cpu[i]);
		re865x_desc_clear_own(&ring->mbuf_descs_cpu[i]);
	}

	/* set wrap */
	ring->pkthdr_descs_cpu[ring->num_descs - 1] |= NIC_DESC_WRAP;
	ring->mbuf_descs_cpu[ring->num_descs - 1] |= NIC_DESC_WRAP;

	ring->top = 0;
	ring->bottom = 0;

	do {
		i = re865x_ring_top_to_index(ring);
		if (!re865x_fill_rx_buf(re, i, NET_SKB_PAD, netdev_alloc_frag)) {
			ret = -ENOMEM;
			break;
		}
		if (ring->top - ring->bottom >= ring->num_descs - 1)
			break;
		ring->top++;
	} while (1);

	/* flush descriptors */
	wmb();

	return ret;
}

static int re865x_ring_rx_refill(struct re865x *re)
{
	struct re865x_ring *ring = &re->rx_ring;
	unsigned int count;
	unsigned int index;
	unsigned int mbuf_idx;

	count = 0;
	do {
		index = re865x_ring_top_to_index(ring);

		mbuf_idx = re865x_ring_mbuf_to_index(ring, index);

		if (!ring->mbufs[mbuf_idx]->rx.buf) {
			if (!re865x_fill_rx_buf(re, index, NET_SKB_PAD, napi_alloc_frag))
				break;
		}

		if (ring->top - ring->bottom >= ring->num_descs - 1)
			break;

		ring->top++;
		count++;
	} while (1);

	/* flush descriptors */
	wmb();

	return count;
}

static void re865x_ring_rx_clean(struct re865x *re)
{
	struct re865x_ring *ring = &re->rx_ring;
	int i;
	unsigned int mbuf_idx;

	for (i = 0; i < ring->num_descs; i++) {
		mbuf_idx = re865x_ring_mbuf_to_index(ring, i);

		if (ring->mbufs[mbuf_idx]->rx.buf) {
			dma_unmap_single(&re->dev->dev, ring->mbufs[mbuf_idx]->rx.dma_addr,
					 re->rx_buf_size, DMA_FROM_DEVICE);
			skb_free_frag(ring->mbufs[mbuf_idx]->rx.buf);

			re865x_desc_clear_own(&ring->mbuf_descs_cpu[mbuf_idx]);
		}

		re865x_desc_clear_own(&ring->pkthdr_descs_cpu[i]);
	}
}

static int re865x_rings_init(struct re865x *re)
{
	int ret;

	ret = re865x_ring_alloc(&re->tx_ring, false);
	if (ret)
		return ret;

	ret = re865x_ring_alloc(&re->tx_ring_dummy, false);
	if (ret)
		return ret;

	ret = re865x_ring_alloc(&re->rx_ring, true);
	if (ret)
		return ret;

	return 0;
}

static void re865x_rings_reset(struct re865x *re)
{
	re865x_ring_rx_clean(re);

	re865x_ring_tx_clean(re);
	netdev_reset_queue(re->dev);
}

static void re865x_rings_cleanup(struct re865x *re)
{
	re865x_ring_free(&re->rx_ring);
	re865x_ring_free(&re->tx_ring);
	re865x_ring_free(&re->tx_ring_dummy);
}

static void re865x_sw_set_cpu_port_macaddr(struct re865x *re, unsigned char *mac)
{
	// XXX: set mac address to switch lookup table?
}

static void re865x_nic_reset(struct re865x *re)
{
	re865x_reg_rmw(re, NIC_CONTROL_REG, 0, NIC_CONTROL_SOFT_RESET);
}

static int re865x_hw_setup(struct re865x *re)
{
	int ret;

	re865x_ring_tx_init(re, &re->tx_ring, false);

	ret = re865x_ring_rx_init(re, &re->rx_ring);
	if (ret)
		return ret;

	re865x_nic_reset(re);

	re865x_interrupt_enable(re);

	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_0_REG, re->rx_ring.pkthdr_descs_dma);
	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_1_REG, 0);
	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_2_REG, 0);
	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_3_REG, 0);
	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_4_REG, 0);
	re865x_reg_write(re, NIC_RX_PKTHDR_DESC_5_REG, 0);

	re865x_reg_write(re, NIC_RX_MBUF_DESC_REG, re->rx_ring.mbuf_descs_dma);

	re865x_reg_write(re, NIC_TX_PKTHDR_DESC_0_REG, re->tx_ring.pkthdr_descs_dma);
	re865x_reg_write(re, NIC_TX_PKTHDR_DESC_1_REG, re->tx_ring_dummy.pkthdr_descs_dma);

	return 0;
}

static int re865x_reset_nic(struct re865x *re)
{
	int ret;

	re865x_reg_rmw(re, NIC_CONTROL_REG, 0, NIC_CONTROL_TX_STOP);

	re865x_ring_rx_clean(re);
	re865x_ring_tx_clean(re);

	ret = re865x_hw_setup(re);
	
	/* hw_start */
	re865x_reg_write(re, NIC_CONTROL_REG,
		NIC_CONTROL_TX_CMD | NIC_CONTROL_RX_CMD |
		(NIC_CONTROL_BURST_SIZE_32B << NIC_CONTROL_BURST_SIZE_SHIFT) |
		(NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_2KB << NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_SHIFT));

	return ret;
}

static int re865x_open(struct net_device *dev)
{
	struct re865x *re = netdev_priv(dev);
	int ret;

	netif_carrier_off(dev);
	re->rx_buf_size = SKB_DATA_ALIGN(RE865X_MBUF_CLUSTER_SIZE + NET_SKB_PAD);

	ret = re865x_hw_setup(re);
	if (ret)
		goto err;

	/* setup mac address */
	re865x_sw_set_cpu_port_macaddr(re, dev->dev_addr);

	napi_enable(&re->napi);

	netif_start_queue(re->dev);

	netif_carrier_on(re->dev);
	
	/* hw_start */
	re865x_reg_write(re, NIC_CONTROL_REG,
		NIC_CONTROL_TX_CMD | NIC_CONTROL_RX_CMD |
		(NIC_CONTROL_BURST_SIZE_32B << NIC_CONTROL_BURST_SIZE_SHIFT) |
		(NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_2KB << NIC_CONTROL_EXTMBUF_CLUSTER_SIZE_SHIFT));

	return 0;

err:
	re865x_rings_reset(re);
	return ret;
}

static int re865x_stop(struct net_device *dev)
{
	struct re865x *re = netdev_priv(dev);
	unsigned long flags;

	netif_carrier_off(dev);

	spin_lock_irqsave(&re->lock, flags);

	netif_stop_queue(re->dev);

	re865x_reg_rmw(re, NIC_CONTROL_REG, 0, NIC_CONTROL_TX_STOP);
	re865x_nic_reset(re);

	napi_disable(&re->napi);
	del_timer_sync(&re->oom_timer);

	spin_unlock_irqrestore(&re->lock, flags);

	re865x_rings_reset(re);

	return 0;
}

static netdev_tx_t re865x_hard_start_xmit(struct sk_buff *skb,
					  struct net_device *dev)
{
	struct re865x *re = netdev_priv(dev);
	struct re865x_ring *ring = &re->tx_ring;
	struct pktHdr *ph_desc;
	struct mBuf *ph_mbuf;
	struct iphdr *ip;
	dma_addr_t dma_addr;
	int i;

	if (skb->len < RE865X_DMA_MIN_SIZE) {
		if (skb_pad(skb, RE865X_DMA_MIN_SIZE - skb->len) < 0)
			goto err_drop;
		skb->len = RE865X_DMA_MIN_SIZE;
		skb_set_tail_pointer(skb, RE865X_DMA_MIN_SIZE);
	}

	dma_addr = dma_map_single(&dev->dev, skb->data, skb->len,
				  DMA_TO_DEVICE);

	/* setup descriptor fields */
	i = re865x_ring_top_to_index(ring);

	ph_desc = ring->pkthdrs[i];
	ph_mbuf = ring->mbufs[i];

	ph_desc->ph_len = skb->len + ETH_FCS_LEN;
	ph_mbuf->m_len = ph_mbuf->m_extsize = ph_desc->ph_len;
	ph_mbuf->m_data = ph_mbuf->m_extbuf = dma_addr;

	ph_desc->ph_type = PKTHDR_TYPE_ETHERNET;
	ph_desc->ph_flags = PKTHDR_FLAG_USED | PKTHDR_FLAG_PKT_OUTGOING;
	ph_desc->ph_flags2.tx.txCVlanTagAutoAdd = 0;
	ph_desc->ph_portlist = ALL_PORT_MASK;
	ph_desc->ph_vlanId = 0;

	/* tx checksum offload */
	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		switch (ntohs(skb->protocol)) {
		case ETH_P_8021Q:
			ph_desc->ph_vlanTagged = 1;
		case ETH_P_IP:
			ph_desc->ph_type = PKTHDR_TYPE_IP;
			ip = (struct iphdr *) skb_network_header(skb);
			switch (ip->protocol) {
			case IPPROTO_TCP:
				ph_desc->ph_type = PKTHDR_TYPE_TCP;
				ph_desc->ph_flags |= PKTHDR_FLAG_CSUM_TCPUDP;
				break;
			case IPPROTO_UDP:
				ph_desc->ph_type = PKTHDR_TYPE_UDP;
				ph_desc->ph_flags |= PKTHDR_FLAG_CSUM_TCPUDP;
				break;
			}
		}
	}

	/* vlan tag offload */
	if (skb_vlan_tag_present(skb)) {
		u8 members, untag_members;
		u16 vid = skb_vlan_tag_get(skb);

		if (rtl865x_get_vlan_port_members(&re->sw, vid, &members, &untag_members)) {
			ph_desc->ph_vlanId = vid;
			ph_desc->ph_portlist = members;
			ph_desc->ph_flags2.tx.txCVlanTagAutoAdd = members & (~untag_members);
		}
	} else {
		if (re->sw.vlan_enabled) {
			/* TODO: wait for switch's delay */
			goto err_drop_free_skb;
		}
	}

	ring->mbufs[i]->tx.skb = skb;

	netdev_sent_queue(dev, skb->len);

	skb_tx_timestamp(skb);

	ring->top++;

	if (ring->top - ring->bottom >= ring->num_descs - 1) {
		DBG("%s: tx queue full\n", dev->name);
		netif_stop_queue(dev);
	}

	re865x_desc_set_own(&ring->pkthdr_descs_cpu[i]);

	/* flush descriptor */
	wmb();

	/* enable TX engine */
	re865x_reg_rmw(re, NIC_CONTROL_REG, 0, NIC_CONTROL_TX_EN);

	return NETDEV_TX_OK;

err_drop_free_skb:
	dev_kfree_skb(skb);

err_drop:
	dev->stats.tx_dropped++;

	return NETDEV_TX_OK;
}

static void re865x_tx_timeout(struct net_device *dev)
{
	struct re865x *re = netdev_priv(dev);
	unsigned long flags;

	if (netif_msg_tx_err(re))
		pr_info("%s: tx timeout\n", re->dev->name);

	netif_stop_queue(dev);

	spin_lock_irqsave(&re->lock, flags);
	re865x_interrupt_disable(re);
	spin_unlock_irqrestore(&re->lock, flags);

	re865x_reset_nic(re);

	netif_wake_queue(dev);

	napi_schedule(&re->napi);
}


static int re865x_tx_packets(struct re865x *re)
{
	struct net_device *dev = re->dev;
	struct re865x_ring *ring = &re->tx_ring;
	unsigned long status;
	int sent = 0;
	int bytes_compl = 0;

	for (; ring->bottom < ring->top; ring->bottom++) {
		unsigned int i = re865x_ring_bottom_to_index(ring);

		if (re865x_desc_is_owned(ring->pkthdr_descs_cpu[i]))
			break;

		dma_unmap_single(&dev->dev, ring->mbufs[i]->m_extbuf, ring->mbufs[i]->tx.skb->len, DMA_TO_DEVICE);

		bytes_compl += ring->mbufs[i]->tx.skb->len;

		dev_kfree_skb_any(ring->mbufs[i]->tx.skb);
		ring->mbufs[i]->tx.skb = NULL;

		sent++;
	}

	dev->stats.tx_bytes += bytes_compl;
	dev->stats.tx_packets += sent;

	if (!sent) {
		status = re865x_interrupt_status(re);
		if (unlikely(status & INTERRUPT_TX)) {
			dev_warn(&dev->dev, "suspicious interrupt for tx completion\n");
			re865x_interrupt_acknowledge(re, INTERRUPT_TX);
		}
		return 0;
	}

	re865x_interrupt_acknowledge(re, INTERRUPT_TX);

	netdev_completed_queue(dev, sent, bytes_compl);
	if ((ring->top - ring->bottom) < (ring->num_descs * 3) / 4)
		netif_wake_queue(dev);

	return sent;
}

static int re865x_rx_packets(struct re865x *re, int limit)
{
	struct net_device *dev = re->dev;
	struct re865x_ring *ring = &re->rx_ring;
	struct sk_buff_head queue;
	struct sk_buff *skb;
	unsigned long status;
	int done = 0;
	int mbuf_idx;
	struct mBuf *pmbuf;
	int offset;

	skb_queue_head_init(&queue);

	while (done < limit) {
		unsigned int i = re865x_ring_bottom_to_index(ring);
		int pktlen;

		if (re865x_desc_is_owned(ring->pkthdr_descs_cpu[i])) {
			re865x_interrupt_acknowledge(re, INTERRUPT_RX);
			break;
		}

		mbuf_idx = re865x_ring_mbuf_to_index(ring, i);

		pmbuf = ring->mbufs[mbuf_idx];

		pktlen = ring->pkthdrs[i]->ph_len;
		pktlen -= ETH_FCS_LEN;

		offset = pmbuf->m_data - pmbuf->m_extbuf;

		dma_unmap_single(&dev->dev, ring->mbufs[mbuf_idx]->rx.dma_addr,
				 re->rx_buf_size, DMA_FROM_DEVICE);

		dev->stats.rx_packets++;
		dev->stats.rx_bytes += pktlen;

		skb = build_skb(ring->mbufs[mbuf_idx]->rx.buf, re865x_buffer_size(re));
		if (!skb) {
			skb_free_frag(ring->mbufs[mbuf_idx]->rx.buf);
			goto next;
		}

		skb_reserve(skb, NET_SKB_PAD + offset);
		skb_put(skb, pktlen);

		skb->dev = dev;
		skb->ip_summed = CHECKSUM_NONE;

		/* rx checksum offload */
		if (dev->features | NETIF_F_RXCSUM) {
			if ((ring->pkthdrs[i]->ph_type == PKTHDR_TYPE_TCP) ||
			    (ring->pkthdrs[i]->ph_type == PKTHDR_TYPE_UDP)) {
				if (ring->pkthdrs[i]->ph_flags | PKTHDR_FLAG_CSUM_TCPUDP)
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			} else if (ring->pkthdrs[i]->ph_type == PKTHDR_TYPE_IP) {
				if (ring->pkthdrs[i]->ph_flags | PKTHDR_FLAG_CSUM_IP)
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}
		}

		/* vlan untag offload */
		skb->protocol = eth_type_trans(skb, dev);

		if (dev->features | NETIF_F_HW_VLAN_CTAG_RX) {
			u16 vid = ring->pkthdrs[i]->ph_vlanId;

			if (rtl865x_cpu_port_is_tagged(&re->sw, vid)) {
				if (skb->protocol != htons(ETH_P_8021Q))
					__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vid);
			}
		}

		__skb_queue_tail(&queue, skb);

next:
		ring->mbufs[mbuf_idx]->rx.buf = NULL;
		ring->mbufs[mbuf_idx]->rx.dma_addr = 0;
		done++;

		ring->bottom++;
	}

	if (re865x_ring_rx_refill(re)) {
		status = re865x_interrupt_status(re);
		if (unlikely(status & INTERRUPT_RX_EMPTY)) {
			dev->stats.rx_fifo_errors++;

			/* re-enable RX */
			re865x_interrupt_acknowledge(re, INTERRUPT_RX_EMPTY);
		}
	}

	while ((skb = __skb_dequeue(&queue)) != NULL)
		netif_receive_skb(skb);

	return done;
}

static int re865x_poll(struct napi_struct *napi, int limit)
{
	struct re865x *re = container_of(napi, struct re865x, napi);
	struct net_device *dev = re->dev;
	struct re865x_ring *rx_ring = &re->rx_ring;
	unsigned long flags, status;
	int tx_done;
	int rx_done;
	int mbuf_idx;

	tx_done = re865x_tx_packets(re);
	rx_done = re865x_rx_packets(re, limit);

	mbuf_idx = re865x_ring_mbuf_to_index(rx_ring, re865x_ring_top_to_index(rx_ring));

	if (rx_ring->mbufs[mbuf_idx]->rx.buf == NULL)
		goto oom;

	status = re865x_interrupt_status(re);

	if (rx_done < limit) {
		if (status & INTERRUPT_RX)
			goto more;

		if (status & INTERRUPT_TX)
			goto more;

		napi_complete(napi);

		/* enable interrupts */
		spin_lock_irqsave(&re->lock, flags);
		re865x_interrupt_enable(re);
		spin_unlock_irqrestore(&re->lock, flags);
		return rx_done;
	}

more:
	return limit;

oom:
	if (netif_msg_rx_err(re))
		pr_info("%s: out of memory\n", dev->name);

	mod_timer(&re->oom_timer, jiffies + RE865X_OOM_REFILL);
	napi_complete(napi);
	return 0;
}

static irqreturn_t re865x_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct re865x *re = netdev_priv(dev);
	u32 status;

	status = re865x_interrupt_status(re);

	if (unlikely(!status))
		return IRQ_NONE;

	if (unlikely(status & INTERRUPT_ERR)) {
		if (status & NIC_INTERRUPT_TX_DESC_0_ERROR) {
			re865x_interrupt_acknowledge(re, NIC_INTERRUPT_TX_DESC_0_ERROR);
			dev_err(&dev->dev, "TX descriptor error\n");
		}
		if (status & NIC_INTERRUPT_RX_DESC_0_ERROR) {
			re865x_interrupt_acknowledge(re, NIC_INTERRUPT_RX_DESC_0_ERROR);
			dev_err(&dev->dev, "RX descriptor error\n");
		}
	}

	if (likely(status & INTERRUPT_POLL)) {
		re865x_interrupt_disable(re);
		napi_schedule(&re->napi);
	}

	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void re865x_netpoll(struct net_device *dev)
{
	disable_irq(dev->irq);
	re865x_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

static void re865x_oom_timer_handler(unsigned long data)
{
	struct net_device *dev = (struct net_device *) data;
	struct re865x *re = netdev_priv(dev);

	napi_schedule(&re->napi);
}

static int re865x_change_mtu(struct net_device *dev, int new_mtu)
{
	unsigned int max_cluster_len;

	max_cluster_len = re865x_max_cluster_len(new_mtu);
	if (new_mtu < 60 || max_cluster_len > RE865X_MBUF_CLUSTER_SIZE)
		return -EINVAL;

	if (netif_running(dev))
		return -EBUSY;

	dev->mtu = new_mtu;
	return 0;
}

static int re865x_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	switch (cmd) {
	case SIOCSIFHWADDR:
		if (copy_from_user
			(dev->dev_addr, ifr->ifr_data, sizeof(dev->dev_addr)))
			return -EFAULT;
		return 0;

	case SIOCGIFHWADDR:
		if (copy_to_user
			(ifr->ifr_data, dev->dev_addr, sizeof(dev->dev_addr)))
			return -EFAULT;
		return 0;
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static const struct net_device_ops re865x_netdev_ops = {
	.ndo_open		= re865x_open,
	.ndo_stop		= re865x_stop,
	.ndo_start_xmit		= re865x_hard_start_xmit,
	.ndo_do_ioctl		= re865x_do_ioctl,
	.ndo_tx_timeout		= re865x_tx_timeout,
	.ndo_change_mtu		= re865x_change_mtu,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= re865x_netpoll,
#endif
};

static int re865x_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct resource *res;
	struct re865x_platform_data *pdata;
	struct re865x *re;
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		err = -ENXIO;
		goto err_out;
	}

	dev = alloc_etherdev(sizeof(*re));
	if (!dev) {
		dev_err(&pdev->dev, "alloc_etherdev failed\n");
		err = -ENOMEM;
		goto err_out;
	}

	SET_NETDEV_DEV(dev, &pdev->dev);

	re = netdev_priv(dev);
	re->pdev = pdev;
	re->dev = dev;
	re->msg_enable = netif_msg_init(re865x_msg_level,
					RE865X_DEFAULT_MSG_ENABLE);
	spin_lock_init(&re->lock);

	/* NIC base */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "nic_base");
	if (!res) {
		dev_err(&pdev->dev, "no mnic_base resource found\n");
		err = -ENXIO;
		goto err_out;
	}

	re->nic_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!re->nic_base) {
		dev_err(&pdev->dev, "unable to ioremap nic_base\n");
		err = -ENOMEM;
		goto err_free_dev;
	}

	/* SWCORE base */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sw_base");
	if (!res) {
		dev_err(&pdev->dev, "no sw_base resource found\n");
		err = -ENXIO;
		goto err_free_dev;
	}

	re->sw_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!re->sw_base) {
		dev_err(&pdev->dev, "unable to ioremap sw_base\n");
		err = -ENOMEM;
		goto err_free_dev;
	}

	/* SWTABLE base */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "swtbl_base");
	if (!res) {
		dev_err(&pdev->dev, "no swtbl_base resource found\n");
		err = -ENXIO;
		goto err_free_dev;
	}

	re->swtbl_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!re->swtbl_base) {
		dev_err(&pdev->dev, "unable to ioremap swtbl_base\n");
		err = -ENOMEM;
		goto err_free_dev;
	}

	/* disable interrupts, clear pending interrupts */
	re865x_reg_write(re, NIC_INTERRUPT_MASK_REG, 0);
	re865x_reg_write(re, NIC_INTERRUPT_STATUS_REG, 0);

	dev->irq = platform_get_irq(pdev, 0);
	err = request_irq(dev->irq, re865x_interrupt, 0x0, dev->name, dev);
	if (err) {
		dev_err(&pdev->dev, "unable to request IRQ %d\n", dev->irq);
		goto err_unmap_base;
	}

	dev->base_addr = (unsigned long)re->nic_base;
	dev->netdev_ops = &re865x_netdev_ops;
	dev->ethtool_ops = &re865x_ethtool_ops;

	dev->hw_features = NETIF_F_IP_CSUM | NETIF_F_RXCSUM_BIT |
		NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX;
	dev->features |= dev->hw_features;

	re->rx_ring.num_descs = RE865X_RX_RINGS_SIZE;
	re->tx_ring.num_descs = RE865X_TX_RINGS_SIZE;
	re->tx_ring_dummy.num_descs = 2;

	err = re865x_rings_init(re);
	if (err) {
		dev_err(&pdev->dev, "unable to initialize NIC rings\n");
		goto err_unmap_base;
	}

	/* out-of-memory timer */
	init_timer(&re->oom_timer);
	re->oom_timer.data = (unsigned long) dev;
	re->oom_timer.function = re865x_oom_timer_handler;

	/* mac address */
	memcpy(dev->dev_addr, pdata->mac_addr, ETH_ALEN);

	netif_napi_add(dev, &re->napi, re865x_poll, RE865X_NAPI_WEIGHT);

	/* stop tx queue then reset */
	re865x_reg_rmw(re, NIC_CONTROL_REG, 0, NIC_CONTROL_TX_STOP);
	re865x_nic_reset(re);

	platform_set_drvdata(pdev, dev);

	err = register_netdev(dev);
	if (err) {
		dev_err(&pdev->dev, "unable to register net device\n");
		goto err_free_irq;
	}

	pr_info("%s: Realtek RE865X NIC at 0x%08lx, irq %d\n",
		dev->name, dev->base_addr, dev->irq);

	/* initialize built-in switch */
	re->sw.parent = &pdev->dev;
	re->sw.dev = dev;
	re->sw.swcore_base = re->sw_base;
	re->sw.swtbl_base = re->swtbl_base;

	err = rtl865x_switch_probe(&re->sw);
	if (err)
		goto err_free_irq;

	return 0;

err_free_irq:
	free_irq(dev->irq, dev);
err_unmap_base:
	iounmap(re->nic_base);
err_free_dev:
	kfree(dev);
err_out:
	platform_set_drvdata(pdev, NULL);
	return err;
}

static int re865x_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);

	if (dev) {
		struct re865x *re = netdev_priv(dev);

		rtl865x_switch_remove(&re->sw);
		unregister_netdev(dev);
		free_irq(dev->irq, dev);
		iounmap(re->nic_base);
		iounmap(re->sw_base);
		iounmap(re->swtbl_base);
		re865x_rings_cleanup(re);
		kfree(dev);
		platform_set_drvdata(pdev, NULL);
	}

	return 0;
}

static struct platform_driver re865x_driver = {
	.probe		= re865x_probe,
	.remove		= re865x_remove,
	.driver = {
		.name	= DRV_NAME,
	}
};

static int __init re865x_module_init(void)
{
	int ret;

	ret = platform_driver_register(&re865x_driver);
	if (ret)
		goto err_out;

	return 0;

err_out:
	return ret;
}

static void __exit re865x_module_exit(void)
{
	platform_driver_unregister(&re865x_driver);
}

module_init(re865x_module_init);
module_exit(re865x_module_exit);

MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Weijie Gao <hackpascal@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
