/*
 **************************************************************************
 * Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * nss_tstamp.c
 *	NSS Tstamp APIs
 */

#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <net/route.h>
#include <net/ip6_route.h>
#include "nss_tx_rx_common.h"
#include "nss_tstamp.h"
#include "nss_tstamp_stats.h"

#define NSS_TSTAMP_HEADER_SIZE max(sizeof(struct nss_tstamp_h2n_pre_hdr), sizeof(struct nss_tstamp_n2h_pre_hdr))

/*
 * Notify data structure
 */
struct nss_tstamp_notify_data {
	nss_tstamp_msg_callback_t tstamp_callback;
	void *app_data;
};

static struct nss_tstamp_notify_data nss_tstamp_notify = {
	.tstamp_callback = NULL,
	.app_data = NULL,
};

static struct net_device_stats *nss_tstamp_ndev_stats(struct net_device *ndev);

/*
 * dummy netdevice ops
 */
static const struct net_device_ops nss_tstamp_ndev_ops = {
	.ndo_get_stats = nss_tstamp_ndev_stats,
};

/*
 * nss_tstamp_ndev_setup()
 *	Dummy setup for net_device handler
 */
static void nss_tstamp_ndev_setup(struct net_device *ndev)
{
	return;
}

/*
 * nss_tstamp_ndev_stats()
 *	Return net device stats
 */
static struct net_device_stats *nss_tstamp_ndev_stats(struct net_device *ndev)
{
	return &ndev->stats;
}

/*
 * nss_tstamp_verify_if_num()
 *	Verify if_num passed to us.
 */
static bool nss_tstamp_verify_if_num(uint32_t if_num)
{
	return (if_num == NSS_TSTAMP_TX_INTERFACE) || (if_num == NSS_TSTAMP_RX_INTERFACE);
}

/*
 * nss_tstamp_interface_handler()
 *	Handle NSS -> HLOS messages for TSTAMP Statistics
 */
static void nss_tstamp_interface_handler(struct nss_ctx_instance *nss_ctx,
		struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_tstamp_msg *ntm = (struct nss_tstamp_msg *)ncm;
	nss_tstamp_msg_callback_t cb;

	if (!nss_tstamp_verify_if_num(ncm->interface)) {
		nss_warning("%px: invalid interface %d for tstamp_tx", nss_ctx, ncm->interface);
		return;
	}

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_TSTAMP_MSG_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for tstamp", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_tstamp_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	switch (ntm->cm.type) {
	case NSS_TSTAMP_MSG_TYPE_SYNC_STATS:
		nss_tstamp_stats_sync(nss_ctx, &ntm->msg.stats, ncm->interface);
		break;
	default:
		nss_warning("%px: Unknown message type %d",
				 nss_ctx, ncm->type);
		return;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_tstamp_notify.tstamp_callback;
		ncm->app_data = (nss_ptr_t)nss_tstamp_notify.app_data;
	}

	/*
	 * Do we have a callback?
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_tstamp_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ntm);
}

/*
 * nss_tstamp_copy_data()
 *	Copy timestamps from received nss frame into skb
 */
static void nss_tstamp_copy_data(struct nss_tstamp_n2h_pre_hdr *ntm, struct sk_buff *skb)
{
	struct skb_shared_hwtstamps *tstamp;

	tstamp = skb_hwtstamps(skb);
	tstamp->hwtstamp = ktime_set(ntm->ts_data_hi, ntm->ts_data_lo);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 16, 0))
	tstamp->syststamp = ktime_set(ntm->ts_data_hi, ntm->ts_data_lo);
#endif
}

/*
 * nss_tstamp_get_dev()
 *	Get the net_device associated with the packet.
 */
static struct net_device *nss_tstamp_get_dev(struct sk_buff *skb)
{
	struct dst_entry *dst;
	struct net_device *dev;
	struct rtable *rt;
	struct flowi6 fl6;
	uint32_t ip_addr;

	/*
	 * It seems like the data came over IPsec, hence indicate
	 * it to the Linux over this interface
	 */
	skb_reset_network_header(skb);
	skb_reset_mac_header(skb);

	skb->pkt_type = PACKET_HOST;

	switch (ip_hdr(skb)->version) {
	case IPVERSION:
		ip_addr = ip_hdr(skb)->saddr;

		rt = ip_route_output(&init_net, ip_addr, 0, 0, 0);
		if (IS_ERR(rt)) {
			return NULL;
		}

		dst = (struct dst_entry *)rt;
		skb->protocol = cpu_to_be16(ETH_P_IP);
		break;

	case 6:
		memset(&fl6, 0, sizeof(fl6));
		memcpy(&fl6.daddr, &ipv6_hdr(skb)->saddr, sizeof(fl6.daddr));

		dst = ip6_route_output(&init_net, NULL, &fl6);
		if (IS_ERR(dst)) {
			return NULL;
		}

		skb->protocol = cpu_to_be16(ETH_P_IPV6);
		break;

	default:
		nss_warning("%px:could not get dev for the skb\n", skb);
		return NULL;
	}

	dev = dst->dev;
	dev_hold(dev);

	dst_release(dst);
	return dev;
}

/*
 * nss_tstamp_buf_receive()
 *	Receive nss exception packets.
 */
static void nss_tstamp_buf_receive(struct net_device *ndev, struct sk_buff *skb, struct napi_struct *napi)
{
	struct nss_tstamp_n2h_pre_hdr *n2h_hdr = (struct nss_tstamp_n2h_pre_hdr *)skb->data;
	struct nss_ctx_instance *nss_ctx;
	struct net_device *dev;
	uint32_t tstamp_sz;

	BUG_ON(!n2h_hdr);

	tstamp_sz = n2h_hdr->ts_hdr_sz;
	if (tstamp_sz > (NSS_TSTAMP_HEADER_SIZE)) {
		goto free;
	}

	nss_ctx = &nss_top_main.nss[nss_top_main.tstamp_handler_id];
	BUG_ON(!nss_ctx);

	skb_pull_inline(skb, tstamp_sz);

	/*
	 * copy the time stamp and convert into ktime_t
	 */
	nss_tstamp_copy_data(n2h_hdr, skb);
	if (unlikely(n2h_hdr->ts_tx)) {
		/*
		 * We are in TX Path
		 */
		skb_tstamp_tx(skb, skb_hwtstamps(skb));

		ndev->stats.tx_packets++;
		ndev->stats.tx_bytes += skb->len;
		goto free;
	}

	/*
	 * We are in RX path.
	 */
	dev = nss_cmn_get_interface_dev(nss_ctx, n2h_hdr->ts_ifnum);
	if (!dev) {
		ndev->stats.rx_dropped++;
		goto free;
	}

	/*
	 * Hold the dev until we finish
	 */
	dev_hold(dev);

	switch(dev->type) {
	case NSS_IPSEC_ARPHRD_IPSEC:
		/*
		 * Release the prev dev reference
		 */
		dev_put(dev);

		/*
		 * find the actual IPsec tunnel device
		 */
		dev = nss_tstamp_get_dev(skb);
		break;

	default:
		/*
		 * This is a plain non-encrypted data packet.
		 */
		skb->protocol = eth_type_trans(skb, dev);
		break;
	}

	skb->skb_iif = dev->ifindex;
	skb->dev = dev;

	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += skb->len;

	netif_receive_skb(skb);

	/*
	 * release the device as we are done
	 */
	dev_put(dev);
	return;
free:
	dev_kfree_skb_any(skb);
	return;
}

/*
 * nss_tstamp_tx_buf()
 *	Send data packet for tstamp processing
 */
nss_tx_status_t nss_tstamp_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *skb, uint32_t if_num)
{
	struct nss_tstamp_h2n_pre_hdr *h2n_hdr;
	int extra_head;
	int extra_tail = 0;
	char *align_data;
	uint32_t hdr_sz;

	nss_trace("%px: Tstamp If Tx packet, id:%d, data=%px", nss_ctx, NSS_TSTAMP_RX_INTERFACE, skb->data);

	/*
	 * header size + alignment size
	 */
	hdr_sz = NSS_TSTAMP_HEADER_SIZE;
	extra_head = hdr_sz - skb_headroom(skb);

	/*
	 * Expand the head for h2n_hdr
	 */
	if (extra_head > 0) {
		/*
		 * Try to accommodate using available tailroom.
		 */
		if (skb->end - skb->tail >= extra_head)
			extra_tail = -extra_head;
		if (pskb_expand_head(skb, extra_head, extra_tail, GFP_KERNEL)) {
			nss_trace("%px: expand head room failed", nss_ctx);
			return NSS_TX_FAILURE;
		}
	}

	align_data = PTR_ALIGN((skb->data - hdr_sz), sizeof(uint32_t));
	hdr_sz = (nss_ptr_t)skb->data - (nss_ptr_t)align_data;

	h2n_hdr = (struct nss_tstamp_h2n_pre_hdr *)skb_push(skb, hdr_sz);
	h2n_hdr->ts_ifnum = if_num;
	h2n_hdr->ts_tx_hdr_sz = hdr_sz;

	return nss_core_send_packet(nss_ctx, skb, NSS_TSTAMP_RX_INTERFACE, H2N_BIT_FLAG_VIRTUAL_BUFFER | H2N_BIT_FLAG_BUFFER_REUSABLE);
}
EXPORT_SYMBOL(nss_tstamp_tx_buf);

/*
 * nss_tstamp_register_netdev()
 *	register dummy netdevice for tstamp interface
 */
struct net_device *nss_tstamp_register_netdev(void)
{
	struct net_device *ndev;
	uint32_t err = 0;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 16, 0))
	ndev = alloc_netdev(sizeof(struct netdev_priv_instance), "qca-nss-tstamp", nss_tstamp_ndev_setup);
#else
	ndev = alloc_netdev(sizeof(struct netdev_priv_instance), "qca-nss-tstamp", NET_NAME_ENUM, nss_tstamp_ndev_setup);
#endif
	if (!ndev) {
		nss_warning("Tstamp: Could not allocate tstamp net_device ");
		return NULL;
	}

	ndev->netdev_ops = &nss_tstamp_ndev_ops;

	err = register_netdev(ndev);
	if (err) {
		nss_warning("Tstamp: Could not register tstamp net_device ");
		free_netdev(ndev);
		return NULL;
	}

	return ndev;
}

/*
 * nss_tstamp_notify_register()
 *	Register to receive tstamp notify messages.
 */
struct nss_ctx_instance *nss_tstamp_notify_register(nss_tstamp_msg_callback_t cb, void *app_data)
{
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.tstamp_handler_id];

	nss_tstamp_notify.tstamp_callback = cb;
	nss_tstamp_notify.app_data = app_data;

	return nss_ctx;
}
EXPORT_SYMBOL(nss_tstamp_notify_register);

/*
 * nss_tstamp_register_handler()
 */
void nss_tstamp_register_handler(struct net_device *ndev)
{
	uint32_t features = 0;
	struct nss_ctx_instance *nss_ctx;

	nss_ctx = &nss_top_main.nss[nss_top_main.tstamp_handler_id];

	nss_core_register_subsys_dp(nss_ctx, NSS_TSTAMP_TX_INTERFACE, nss_tstamp_buf_receive, NULL, NULL, ndev, features);

	nss_core_register_handler(nss_ctx, NSS_TSTAMP_TX_INTERFACE, nss_tstamp_interface_handler, NULL);

	nss_core_register_handler(nss_ctx, NSS_TSTAMP_RX_INTERFACE, nss_tstamp_interface_handler, NULL);

	nss_tstamp_stats_dentry_create();
}
